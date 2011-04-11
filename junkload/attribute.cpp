#include "attribute.hpp"

#include <sstream>
#include <iomanip>
#include <limits>

#include <boost/algorithm/string.hpp>

namespace stream_process
{

attribute::attribute() :
	type(SP_INT_8), size(1), offset(0)
{
}

attribute::attribute(const std::string& name, data_type_id type, size_t size) :
	name(name), type(type), size(size), offset(0)
{
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

std::string attribute::to_string() const
{
	const data_type_helper& dth = data_type_helper::get_singleton();

	std::string result;

	result = name;
	while (result.size() < 16)
		result += " ";

	result += dth.get_default_name(type);
	while (result.size() < 32)
		result += " ";

	result += boost::lexical_cast<std::string>(size);
	while (result.size() < 40)
		result += " ";

	result += boost::lexical_cast<std::string>(offset);
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

	result = name;
	while (result.size() < 16)
		result += " ";

	result += dth.get_default_name(type);
	while (result.size() < 32)
		result += " ";

	result += boost::lexical_cast<std::string>(size);
	while (result.size() < 40)
		result += " ";

	result += "\n";

	std::string id_ = identifier;
	while (id_.size() < 20)
		id_ += " ";

	return id_ + result;
}

} // namespace stream_process
