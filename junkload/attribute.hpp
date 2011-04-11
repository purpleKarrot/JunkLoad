#ifndef __STREAM_PROCESS__ATTRIBUTE__HPP__
#define __STREAM_PROCESS__ATTRIBUTE__HPP__

#include "data_types.hpp"

#include <boost/lexical_cast.hpp>

#include <string>
#include <vector>
#include <functional>

namespace stream_process
{

/**
 *	this class contains the description / meta data of an attribute of a stream_point.
 *	attributes are for example the point position or the normal.
 *
 *	for an array of attributes, use a data_type_id with the array flag set.
 *
 */
class attribute
{
public:
	attribute();

	attribute(const std::string& name_, data_type_id data_type_id_,
			size_t number_of_elements = 1);

	bool is_array() const
	{
		return size > 1;
	}

	bool is_high_precision() const
	{
		return false;
	}

	bool is_output() const
	{
		return true;
	}

	template<typename container_t>
	bool from_header_strings(const container_t& attr_tokens);
	bool from_header_string(const std::string& attr_string);
	bool from_header_string_vector(const std::vector<std::string>& attr_tokens);

	std::string to_string() const;
	std::string to_header_string() const;
	std::string to_header_string(const std::string& identifier) const;

public:
	std::string name;
	data_type_id type;
	std::size_t size;
	std::size_t offset;
};

inline std::size_t size_in_bytes(const attribute& a)
{
	const data_type_helper& dth = data_type_helper::get_singleton();
	std::size_t element_size = dth.get_size_in_bytes(a.type);
	return a.size * element_size;
}

template<typename container_t>
bool attribute::from_header_strings(const container_t& tokens)
{
	if (tokens.size() < 3)
		return false;

	if (tokens[0] != "attribute")
		return false;

	std::string name = tokens[1];
	data_type_id type = SP_INT_8;
	size_t number_of_elements = 1;
	size_t flags = 0;

	size_t size_in_bytes = 0;

	const data_type_helper& dth = data_type_helper::get_singleton();
	type = dth.get_data_type_id(tokens[2]);

	if (tokens.size() > 3)
	{
		number_of_elements = boost::lexical_cast<size_t>(tokens[3]);
	}

	if (tokens.size() > 4)
	{
		size_in_bytes = boost::lexical_cast<size_t>(tokens[4]);
	}

	if (tokens.size() > 5)
	{
		flags = boost::lexical_cast<size_t>(tokens[5]);
	}

	this->name = name;
	this->type = type;
	this->size = number_of_elements;

	return true;
}

} // namespace stream_process

#endif
