#include "stream_structure.hpp"

namespace stream_process
{

bool stream_structure::has_attribute(const std::string& name) const
{
	return _by_name.find(name) != _by_name.end();
}

bool stream_structure::has_attribute(const std::string& name, data_type_id id_,
		size_t array_size) const
{
	attribute* attr_ptr = find(name);

	if (!attr_ptr)
		return false;

	if (attr_ptr->get_data_type_id() == id_
			&& attr_ptr->get_number_of_elements() == array_size)
	{
		return true;
	}
	return false;
}

attribute& stream_structure::get_attribute(const std::string& name)
{
	named_iterator it = _by_name.find(name);
	if (it == _by_name.end())
	{
		throw std::runtime_error(
				std::string("could not find attribute with name ") + name + ".");
	}

	return *(it->second);
}

const attribute& stream_structure::get_attribute(const std::string& name) const
{
	const_named_iterator it = _by_name.find(name);
	if (it == _by_name.end())
	{
		throw std::runtime_error(
				std::string("could not find attribute with name ") + name + ".");
	}

	return *(it->second);
}

// creates an empty attribute with the specified name
attribute& stream_structure::create_attribute(const std::string& name,
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

// creates an empty attribute with the specified name
attribute& stream_structure::create_custom_attribute(const std::string& name,
		size_t element_size_in_bytes, size_t number_of_elements)
{
	attribute* new_attr = new attribute(name, element_size_in_bytes,
			number_of_elements, true);

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

void stream_structure::create_attribute(const attribute& attr)
{
	if (attr.get_name() == "uninitialized")
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

void stream_structure::_add_attribute(attribute& attr)
{
	const std::string& name = attr.get_name();

	if (has_attribute(name))
	{
		throw std::runtime_error(
				std::string("could not create attribute with name ") + name
						+ " - name is already taken.");
	}

	named_container::value_type new_element(name, &attr);
	std::pair<named_iterator, bool> result = _by_name.insert(new_element);

	if (result.second != true)
	{
		throw std::runtime_error(
				std::string("could not create attribute with name ") + name
						+ ".");
	}

	attributes.push_back(attr);
}

attribute* stream_structure::find(const std::string& name) const
{
	const_named_iterator it = _by_name.find(name);
	if (it != _by_name.end())
		return it->second;
	else
		return 0;
}

size_t stream_structure::compute_size_in_bytes() const
{
	size_t size_in_bytes = 0;
	super::const_iterator it = attributes.begin(), it_end = attributes.end();
	for (; it != it_end; ++it)
	{
		size_in_bytes += (it)->get_size_in_bytes();
	}

	return size_in_bytes;
}

size_t stream_structure::compute_out_size_in_bytes() const
{
	size_t size_in_bytes = 0;
	super::const_iterator it = attributes.begin(), it_end = attributes.end();
	for (; it != it_end; ++it)
	{
		const attribute& attr = *it;
		if (attr.is_output())
			size_in_bytes += (it)->get_size_in_bytes();
	}

	return size_in_bytes;
}

void stream_structure::compute_offsets()
{
	size_t offset = 0;
	for (super::iterator it = attributes.begin(), it_end = attributes.end(); it != it_end; ++it)
	{
		attribute& attr = *it;
		attr.set_offset(offset);
		offset += attr.get_size_in_bytes();
	}
}

std::string stream_structure::to_string() const
{
	assert(_by_name.size() == attributes.size());

	std::string structure_string;
	for (super::const_iterator it = attributes.begin(), it_end = attributes.end(); it != it_end; ++it)
	{
		const attribute& attr = *it;
		structure_string += attr.to_string();
	}
	return structure_string;
}

std::string stream_structure::to_header_string() const
{
	assert(_by_name.size() == attributes.size());

	std::string structure_string = "";
	structure_string += "# element, attribute-identifier, name, type, ";
	structure_string += " array_size, size_in_bytes, flags\n";

	for (super::const_iterator it = attributes.begin(), it_end = attributes.end(); it != it_end; ++it)
	{
		const attribute& attr = *it;
		if (attr.is_output())
			structure_string += attr.to_header_string("");
	}
	return structure_string;
}

void stream_structure::print(std::ostream& os) const
{
	os << to_string() << std::endl;
}

std::string stream_structure::get_name() const
{
	return std::string();
}

size_t stream_structure::get_number_of_attributes() const
{
	return attributes.size();
}

} // namespace stream_process
