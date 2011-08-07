#include <junk/data_set.hpp>

#include <iostream>
#include <algorithm>
#include <junk/stream_range.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/exception/diagnostic_information.hpp>

#define self (*(*this))

struct load_element
{
	typedef std::vector<boost::iostreams::mapped_file>::iterator iterator;

	load_element(iterator it, const std::string& filename, bool new_file) :
			it(it), filename(filename), new_file(new_file)
	{
	}

	void operator()(const junk::element& element)
	{
		boost::iostreams::mapped_file_params params;
		params.path = filename + '.' + element.name_pl;
		params.mode = std::ios_base::in | std::ios_base::out;

		if (new_file)
		{
			params.new_file_size = file_size_in_bytes(element);
			std::cout << "creating file '" << params.path << "' with a size of "
					  << (double) params.new_file_size / 1073741824.0
					  << " gigabytes." << std::endl;
		}

		*it++ = boost::iostreams::mapped_file(params);
	}

	iterator it;
	const std::string& filename;
	bool new_file;
};

template<>
struct pimpl<junk::data_set>::implementation
{
	implementation(const std::string& filename, bool new_file) :
			filename(filename)
	{
	}

	junk::header header;
	std::string filename;
	std::vector<boost::iostreams::mapped_file> mapped_files;
};

junk::data_set::data_set(const std::string& filename, bool new_file) :
		junk::data_set::data_set::base(filename, new_file)
{
	if (!new_file)
	{
		junk::load_header(filename, self.header);
		load(false);
	}
}

junk::header& junk::data_set::header()
{
	return self.header;
}

const junk::header& junk::data_set::header() const
{
	return self.header;
}

void junk::data_set::load(bool new_file)
{
	try
	{
		self.mapped_files.resize(self.header.elements.size());
		std::for_each(self.header.elements.begin(), self.header.elements.end(),
			load_element(self.mapped_files.begin(),	self.filename, new_file));
	}
	catch (std::exception& except)
	{
		std::cerr << boost::diagnostic_information(except);
		std::abort();
	}
}

junk::raw_data junk::data_set::raw_data(std::size_t index)
{
	assert(index < self.mapped_files.size());
	junk::raw_data ret;
	ret.data = self.mapped_files[index].data();
	ret.size = self.mapped_files[index].size();
	return ret;
}

junk::const_raw_data junk::data_set::raw_data(std::size_t index) const
{
	assert(index < self.mapped_files.size());
	junk::const_raw_data ret;
	ret.data = self.mapped_files[index].data();
	ret.size = self.mapped_files[index].size();
	return ret;
}

junk::stream_range junk::data_set::stream_range(std::size_t index)
{
	assert(index < self.header.elements.size());
	char* begin = self.mapped_files[index].data();
	char* end = begin + self.mapped_files[index].size();
	std::size_t element_size = size_in_bytes(self.header.elements[index]);
	return junk::make_stream_range(begin, end, element_size);
}

junk::const_stream_range junk::data_set::stream_range(std::size_t index) const
{
	assert(index < self.header.elements.size());
	const char* begin = self.mapped_files[index].data();
	const char* end = begin + self.mapped_files[index].size();
	std::size_t element_size = size_in_bytes(self.header.elements[index]);
	return junk::make_stream_range(begin, end, element_size);
}
