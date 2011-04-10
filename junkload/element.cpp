#include "element.hpp"

#include "file_suffix_helper.hpp"

#include <boost/lexical_cast.hpp>

namespace stream_process
{

void element::update()
{
	compute_offsets();
}

std::string element::to_string() const
{
	std::string result = "element ";
	result += _name;
	result += "\n";

	result += _name;
	result += " count ";
	result += boost::lexical_cast<std::string>(_size);
	result += " \n";

	result += "\n";

	result += to_string_();

	return result;
}

std::string element::to_header_string() const
{
	std::string result = "element ";
	result += _name;
	result += "\n";

	result += _name;
	result += " count ";
	result += boost::lexical_cast<std::string>(_size);
	result += " \n";

	result += "\n";

	result += to_header_string_();

	return result;
}

std::string element::get_filename(const std::string& base_filename) const
{
	return base_filename + file_suffix_helper::get_suffix(_name);
}

} // namespace stream_process
