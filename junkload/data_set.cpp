#include <junk/data_set.hpp>

#include <iostream>
#include <stdexcept>
#include <junk/stream_range.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/exception/diagnostic_information.hpp>

#include <boost/spirit/home/phoenix/core/argument.hpp>
//#include <boost/spirit/home/phoenix/operator/member.hpp>
#include <boost/spirit/home/phoenix/operator/comparison.hpp>
#include <boost/spirit/home/phoenix/bind/bind_member_variable.hpp>
#include <boost/range/algorithm/find_if.hpp>

#define self (*(*this))

namespace junk
{

bool load_header(const std::string& filename, element_list& elements);
bool save_header(const std::string& filename, const element_list& elements);

} //namespace junk

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
			params.new_file_size = junk::file_size_in_bytes(element);
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
		is_open(false), filename(filename)
	{
	}

	bool is_open;
	std::string filename;
	junk::element_list elements;
	std::vector<boost::iostreams::mapped_file> mapped_files;
};

junk::data_set::data_set(const std::string& filename, bool new_file) :
		junk::data_set::data_set::base(filename, new_file)
{
	if (new_file)
		return;

	junk::load_header(filename, self.elements);
	load(false);

	BOOST_FOREACH(junk::element& elem, self.elements)
	{
		std::size_t offset = 0;
		BOOST_FOREACH(junk::attribute& attr, elem.attributes)
		{
			attr.offset = offset;
			offset += size_in_bytes(attr);
		}
	}
}

void junk::data_set::add_element(const char* name, const char* plural)
{
	junk::element elem;
	elem.name_sg = name;
	elem.name_pl = plural ? plural : elem.name_sg + 's';
	self.elements.push_back(elem);
}

void junk::data_set::add_attribute(const char* element, const char* attribute,
		junk::type type, std::size_t size)
{
	junk::element& elem = get_element(element);

	std::size_t offset = 0;
	if(!elem.attributes.empty())
	{
		junk::attribute& last = elem.attributes.back();
		offset = last.offset + size_in_bytes(last);
	}

	elem.attributes.push_back(junk::attribute(type, attribute, size, offset));
}

std::size_t junk::data_set::get_size(const char* element) const
{
	return get_element(element).size;
}

void junk::data_set::set_size(const char* element, std::size_t size)
{
	get_element(element).size = size;
}

const junk::attribute& junk::data_set::get_attribute(const char* element, const char* attribute) const
{
	const junk::element& elem = get_element(element);

	junk::attrib_list::const_iterator it = boost::range::find_if(elem.attributes,
			boost::phoenix::bind(&junk::attribute::name, boost::phoenix::arg_names::arg1) == attribute
			//boost::phoenix::arg_names::arg1->*&element::name_sg == name
			);

	if (it == elem.attributes.end())
		throw std::runtime_error("attribute not found.");

	return *it;
}

void junk::data_set::load(bool new_file)
{
	if (new_file)
		junk::save_header(self.filename, self.elements);

	try
	{
		self.mapped_files.resize(self.elements.size());
		boost::range::for_each(self.elements,
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
	assert(index < self.elements.size());
	char* begin = self.mapped_files[index].data();
	char* end = begin + self.mapped_files[index].size();
	std::size_t element_size = size_in_bytes(self.elements[index]);
	return junk::make_stream_range(begin, end, element_size);
}

junk::const_stream_range junk::data_set::stream_range(std::size_t index) const
{
	assert(index < self.elements.size());
	const char* begin = self.mapped_files[index].data();
	const char* end = begin + self.mapped_files[index].size();
	std::size_t element_size = size_in_bytes(self.elements[index]);
	return junk::make_stream_range(begin, end, element_size);
}

junk::element& junk::data_set::get_element(const char* name)
{
	junk::element_list::iterator it = boost::range::find_if(self.elements,
			boost::phoenix::bind(&junk::element::name_sg, boost::phoenix::arg_names::arg1) == name
			//boost::phoenix::arg_names::arg1->*&element::name_sg == name
			);

	if (it == self.elements.end())
		throw std::runtime_error("element not found.");

	return *it;
}

const junk::element& junk::data_set::get_element(const char* name) const
{
	junk::element_list::const_iterator it = boost::range::find_if(self.elements,
			boost::phoenix::bind(&junk::element::name_sg, boost::phoenix::arg_names::arg1) == name
			//boost::phoenix::arg_names::arg1->*&element::name_sg == name
			);

	if (it == self.elements.end())
		throw std::runtime_error("element not found.");

	return *it;
}
