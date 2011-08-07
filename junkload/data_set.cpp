#include <junk/data_set.hpp>

#include <iostream>
#include <algorithm>
#include <junk/mapped_data_element.hpp>

#define self (*(*this))

struct load_element
{
	typedef std::vector<boost::iostreams::mapped_file>::iterator iterator;

	load_element(iterator it) :
			it(it)
	{
	}

	void operator()(const junk::element& element)
	{
	}

	iterator it;
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

	// deprecated
	junk::mapped_data_element vertex_map_;
	junk::mapped_data_element face_map_;
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
	// deprecated
	self.vertex_map_.open(self.header.vertex(), self.filename, new_file);
	self.face_map_.open(self.header.face(), self.filename, new_file);

	self.mapped_files.resize(self.header.elements.size());
	std::for_each(self.header.elements.begin(), self.header.elements.end(),
			load_element(self.mapped_files.begin()));
}

junk::mapped_data_element& junk::data_set::vertex_map()
{
	return self.vertex_map_;
}

const junk::mapped_data_element& junk::data_set::vertex_map() const
{
	return self.vertex_map_;
}

junk::mapped_data_element& junk::data_set::face_map()
{
	return self.face_map_;
}

const junk::mapped_data_element& junk::data_set::face_map() const
{
	return self.face_map_;
}
