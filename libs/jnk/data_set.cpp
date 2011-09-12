#include <jnk/data_set.hpp>

#include <iostream>
#include <stdexcept>
#include <jnk/stream_range.hpp>
#include <boost/foreach.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/exception/diagnostic_information.hpp>

#include <boost/spirit/home/phoenix/core/argument.hpp>
//#include <boost/spirit/home/phoenix/operator/member.hpp>
#include <boost/spirit/home/phoenix/operator/comparison.hpp>
#include <boost/spirit/home/phoenix/bind/bind_member_variable.hpp>
#include <boost/range/algorithm/find_if.hpp>

#include <boost/filesystem.hpp>

#define self (*(*this))

namespace junk
{

bool load_header(const std::string& filename, element_list& elements);
bool save_header(const std::string& filename, const element_list& elements);

} //namespace junk

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
	reload();

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

	junk::element_list::iterator it = boost::range::find_if(self.elements,
			boost::phoenix::bind(&junk::element::name_sg, boost::phoenix::arg_names::arg1) == name
			//boost::phoenix::arg_names::arg1->*&element::name_sg == name
			);

	if (it == self.elements.end())
	{
		self.elements.push_back(elem);
	}
	else
	{
		*it = elem;
	}
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

void junk::data_set::reload()
{
	bool header_changed = false;

	try
	{
		self.mapped_files.resize(self.elements.size());
		for (std::size_t i = 0; i < self.elements.size(); ++i)
		{
			boost::iostreams::mapped_file_params params;
			params.path = self.filename + '.' + self.elements[i].name_pl;
			params.mode = std::ios_base::in | std::ios_base::out;

			if (!boost::filesystem::exists(params.path))
			{
				params.new_file_size = junk::file_size_in_bytes(self.elements[i]);

				std::cout << "creating file '" << params.path << "' with a size of ";

				if (params.new_file_size > 1073741824)
				{
					std::cout << params.new_file_size / 1073741824.0 << " G";
				}
				else if (params.new_file_size > 1048576)
				{
					std::cout << params.new_file_size / 1048576.0 << " M";
				}
				else if (params.new_file_size > 1024)
				{
					std::cout << params.new_file_size / 1024.0 << " k";
				}
				else
				{
					std::cout << params.new_file_size << " ";
				}

				std::cout << "B." <<std::endl;

				header_changed = true;
			}

			self.mapped_files[i] = boost::iostreams::mapped_file(params);
		}
	}
	catch (std::exception& except)
	{
		std::cerr << boost::diagnostic_information(except);
		std::abort();
	}

	if (header_changed)
		junk::save_header(self.filename, self.elements);
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
