#include "attribute.hpp"

#include <sstream>
#include <iomanip>
#include <limits>

#include <boost/algorithm/string.hpp>

namespace stream_process
{

attribute::attribute() :
	_name("uninitialized"),
	_data_type_id(SP_INT_8),
	_number_of_elements(0),
	_element_size_in_bytes(0),
	_size_in_bytes(0),
	_offset(std::numeric_limits<size_t>::max()),
	_flags()
{
}

attribute::attribute(const std::string& name_, data_type_id data_type_id_,
		size_t number_of_elements_) :
	_name(name_),
	_data_type_id(data_type_id_),
	_number_of_elements(number_of_elements_),
	_element_size_in_bytes(0),
	_size_in_bytes(0),
	_offset(std::numeric_limits<size_t>::max()),
	_flags()
{
	_update();
}

attribute::attribute(const std::string& name_, size_t element_size_in_bytes_,
		size_t number_of_elements_, bool is_custom_type) :
	_name(name_),
	_data_type_id(SP_INT_8),
	_number_of_elements(number_of_elements_),
	_element_size_in_bytes(element_size_in_bytes_),
	_size_in_bytes(_element_size_in_bytes * _number_of_elements),
	_offset(std::numeric_limits<size_t>::max()),
	_flags()
{
}

void attribute::_update()
{
	const data_type_helper& dth = data_type_helper::get_singleton();

	_element_size_in_bytes = dth.get_size_in_bytes(_data_type_id);
	_size_in_bytes = _number_of_elements * _element_size_in_bytes;
}

bool attribute::from_header_string(const std::string& attr_string)
{
	using namespace boost;
	std::vector < std::string > tokens;
	split(tokens, attr_string, is_any_of(" "));
	return from_header_string_vector(tokens);
}

bool attribute::from_header_string_vector(
		const std::vector<std::string>& tokens)
{
	if (tokens.size() < 3)
		return false;

	if (tokens[0] == "vertex_attribute" || tokens[0] == "face_attribute"
			|| tokens[0] == "attribute" || tokens[0] == "data")
	{
	}
	else
		return false;

	std::string name = tokens[1];
	data_type_id type_ = SP_INT_8;
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

	_update();

	return true;
}

std::string attribute::to_string() const
{
	const data_type_helper& dth = data_type_helper::get_singleton();

	std::string result;

	result = _name;
	while (result.size() < 16)
		result += " ";

	result += dth.get_default_name(_data_type_id);
	while (result.size() < 32)
		result += " ";

	result += boost::lexical_cast<std::string>(_number_of_elements);
	while (result.size() < 40)
		result += " ";

	result += boost::lexical_cast<std::string>(_offset);
	while (result.size() < 48)
		result += " ";

	if (is_output())
		result += "  out";

	result += "\n";

	return result;
}

std::string attribute::to_header_string() const
{
	return to_header_string("data");
}

std::string attribute::to_header_string(const std::string& identifier) const
{
	const data_type_helper& dth = data_type_helper::get_singleton();

	std::string result;

	result = _name;
	while (result.size() < 16)
		result += " ";

	result += dth.get_default_name(_data_type_id);
	while (result.size() < 32)
		result += " ";

	result += boost::lexical_cast<std::string>(_number_of_elements);
	while (result.size() < 40)
		result += " ";

	result += boost::lexical_cast<std::string>(_size_in_bytes);
	while (result.size() < 48)
		result += " ";

	result += boost::lexical_cast<std::string>(_flags.array);
	while (result.size() < 56)
		result += " ";

	result += "\n";

	std::string id_ = identifier;
	while (id_.size() < 20)
		id_ += " ";

	return id_ + result;
}

} // namespace stream_process
