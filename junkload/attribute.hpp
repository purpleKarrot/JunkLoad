#ifndef __STREAM_PROCESS__ATTRIBUTE__HPP__
#define __STREAM_PROCESS__ATTRIBUTE__HPP__

#include "data_types.hpp"
#include "bit_array.hpp"

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
	enum
	{
		ATTR_IS_HIGH_PRECISION, ATTR_IS_TEMPORARY
	};

	attribute();

	attribute(const std::string& name_, data_type_id data_type_id_,
			size_t number_of_elements = 1);

	attribute(const std::string& name, size_t element_size_in_bytes,
			size_t number_of_elements, bool is_custom_type);

	const std::string& get_name() const
	{
		return _name;
	}

	const data_type_id& get_data_type_id() const
	{
		return _data_type_id;
	}

	void set_data_type_id(data_type_id id_)
	{
		_data_type_id = id_;
	}

	size_t get_element_size_in_bytes() const
	{
		return _element_size_in_bytes;
	}

	size_t get_number_of_elements() const
	{
		return _number_of_elements;
	}

	size_t get_size_in_bytes() const
	{
		return _size_in_bytes;
	}

	size_t get_offset() const
	{
		return _offset;
	}

	void set_offset(size_t offset_)
	{
		_offset = offset_;
	}

	bool is_array() const
	{
		return _number_of_elements > 1;
	}

	void set_number_of_elements(size_t number_of_elements_)
	{
		_number_of_elements = number_of_elements_;
	}

	void set_is_high_precision(bool is_hp)
	{
		_flags.set_bit(ATTR_IS_HIGH_PRECISION, is_hp);
	}

	bool is_high_precision() const
	{
		return _flags.get_bit(ATTR_IS_HIGH_PRECISION);
	}

	void set_is_output(bool is_output)
	{
		_flags.set_bit(ATTR_IS_TEMPORARY, !is_output);
	}

	bool is_output() const
	{
		return !_flags.get_bit(ATTR_IS_TEMPORARY);
	}

	bool from_header_string(const std::string& attr_string);

	bool from_header_string_vector(const std::vector<std::string>& attr_tokens);

	template<typename container_t>
	bool from_header_strings(const container_t& attr_tokens);

	std::string to_string() const;
	std::string to_header_string() const;
	std::string to_header_string(const std::string& identifier) const;

private:
	void _update();

public:
	std::string _name;

	data_type_id _data_type_id;

	size_t _number_of_elements;

	size_t _element_size_in_bytes;

	size_t _size_in_bytes;

	size_t _offset;

	bit_array _flags;
};

struct attribute_ptr_offset_less
{
	inline bool operator()(const attribute* a, const attribute* b)
	{
		return std::less<size_t>()(a->get_offset(), b->get_offset());
	}
};

struct attribute_ptr_outputs_first
{
	inline bool operator()(const attribute* a, const attribute* b)
	{
		const bool a_is_output = a->is_output();
		const bool b_is_output = b->is_output();

		if (!a_is_output && b_is_output)
			return false;

		if (a_is_output && !b_is_output)
			return true;

		return std::less<size_t>()(a->get_offset(), b->get_offset());
	}
};

template<typename container_t>
bool attribute::from_header_strings(const container_t& tokens)
{
	if (tokens.size() < 3)
		return false;

	if (tokens[0] != "attribute")
		return false;

	std::string name = tokens[1];
	data_type_id type_ = SP_UNKNOWN_DATA_TYPE;
	size_t number_of_elements = 1;
	size_t flags = 0;

	size_t size_in_bytes = 0;

	const data_type_helper& dth = data_type_helper::get_singleton();
	type_ = dth.get_data_type_id(tokens[2]);

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

	_name = name;
	_data_type_id = type_;
	_number_of_elements = number_of_elements;
	_flags = flags;

	if (_data_type_id != SP_UNKNOWN_DATA_TYPE)
	{
		_update();
	}
	else
	{
		_element_size_in_bytes = size_in_bytes / number_of_elements;
		_size_in_bytes = size_in_bytes;
	}

	return true;
}

} // namespace stream_process

#endif
