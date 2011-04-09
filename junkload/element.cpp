#include "element.hpp"

#include "file_suffix_helper.hpp"

#include <boost/lexical_cast.hpp>

namespace stream_process
{

void data_element::update()
{
	_structure.compute_offsets();

	_size_in_bytes = _structure.compute_size_in_bytes();
	_data_size_in_bytes = _size * _size_in_bytes;
}

std::string data_element::to_string() const
{
	std::string result = "element ";
	result += _name;
	result += "\n";

	result += _name;
	result += " count ";
	result += boost::lexical_cast<std::string>(_size);
	result += " \n";

	if (_offset)
	{
		result += _name;
		result += " offset ";
		result += boost::lexical_cast<std::string>(_offset);
		result += " \n";
	}
	result += "\n";

	result += _structure.to_string();

	return result;
}

std::string data_element::to_header_string() const
{
	std::string result = "element ";
	result += _name;
	result += "\n";

	result += _name;
	result += " count ";
	result += boost::lexical_cast<std::string>(_size);
	result += " \n";

	if (_offset)
	{
		result += _name;
		result += " offset ";
		result += boost::lexical_cast<std::string>(_offset);
		result += " \n";
	}
	result += "\n";

	result += _structure.to_header_string();

	return result;
}

std::string data_element::get_filename(const std::string& base_filename) const
{
	return base_filename + file_suffix_helper::get_suffix(_name);
}

} // namespace stream_process
