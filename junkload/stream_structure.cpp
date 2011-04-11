#include "element.hpp"

#include <boost/range/algorithm/find_if.hpp>
using boost::range::find_if;

namespace stream_process
{

struct match_name
{
	match_name(const std::string& name) :
		name(name)
	{
	}

	bool operator()(const attribute& attr)
	{
		return attr.name == name;
	}

	const std::string& name;
};

bool element::has_attribute(const std::string& name) const
{
	return find_if(attributes_, match_name(name)) != attributes_.end();
}

bool element::has_attribute(const std::string& name, data_type_id id_,
		size_t array_size) const
{
	const attribute* attr_ptr = find(name);

	if (!attr_ptr)
		return false;

	if (attr_ptr->type == id_ && attr_ptr->size == array_size)
	{
		return true;
	}

	return false;
}

attribute& element::get_attribute(const std::string& name)
{
	std::vector<attribute>::iterator it = find_if(attributes_, match_name(name));
	if (it == attributes_.end())
	{
		throw std::runtime_error(
				std::string("could not find attribute with name ") + name + ".");
	}

	return *it;
}

const attribute& element::get_attribute(const std::string& name) const
{
	std::vector<attribute>::const_iterator it = find_if(attributes_, match_name(name));
	if (it == attributes_.end())
	{
		throw std::runtime_error(
				std::string("could not find attribute with name ") + name + ".");
	}

	return *it;
}

// creates an empty attribute with the specified name
attribute& element::create_attribute(const std::string& name,
		data_type_id data_type_id_, size_t array_size)
{
	attribute* new_attr = new attribute(name, data_type_id_, array_size);

	try
	{
		_add_attribute(*new_attr);
	} catch (...)
	{
		delete new_attr;
		throw;
	}

	return *new_attr;
}

void element::create_attribute(const attribute& attr)
{
	if (attr.name.empty())
	{
		throw std::runtime_error(
				std::string("attempt to add uninitialized attribute to ")
						+ " point structure failed.");
	}

	attribute* new_attr = new attribute(attr);

	try
	{
		_add_attribute(*new_attr);
	} catch (...)
	{
		delete new_attr;
		throw;
	}
}

void element::_add_attribute(attribute& attr)
{
	const std::string& name = attr.name;

	if (has_attribute(name))
	{
		throw std::runtime_error(
				std::string("could not create attribute with name ") + name
						+ " - name is already taken.");
	}

	attributes_.push_back(attr);
}

const attribute* element::find(const std::string& name) const
{
	std::vector<attribute>::const_iterator it = find_if(attributes_, match_name(name));
	if (it != attributes_.end())
		return &(*it);
	else
		return 0;
}

void element::compute_offsets()
{
	size_t offset = 0;
	for (std::vector<attribute>::iterator it = attributes_.begin(), it_end =
			attributes_.end(); it != it_end; ++it)
	{
		attribute& attr = *it;
		attr.offset = offset;
		offset += size_in_bytes(attr);
	}
}

std::string element::to_string_() const
{
	std::string structure_string;
	for (std::vector<attribute>::const_iterator it = attributes_.begin(), it_end = attributes_.end(); it != it_end; ++it)
	{
		const attribute& attr = *it;
		structure_string += attr.to_string();
	}
	return structure_string;
}

std::string element::to_header_string_() const
{
	std::string structure_string = "";
	structure_string += "# element, attribute-identifier, name, type, ";
	structure_string += " array_size, size_in_bytes, flags\n";

	for (std::vector<attribute>::const_iterator it = attributes_.begin(), it_end = attributes_.end(); it != it_end; ++it)
	{
		const attribute& attr = *it;
		if (attr.is_output())
			structure_string += attr.to_header_string("");
	}
	return structure_string;
}

void element::print(std::ostream& os) const
{
	os << to_string() << std::endl;
}

std::string element::get_name() const
{
	return std::string();
}

size_t element::get_number_of_attributes() const
{
	return attributes_.size();
}

} // namespace stream_process
