#include "data_element.hpp"

#include "mapped_data_element.hpp"

#include "file_suffix_helper.hpp"

#include <boost/lexical_cast.hpp>

namespace stream_process
{

data_element::data_element(const std::string& name) :
	_name(name), _structure(name + " attribute"), _size(0), _offset(0),
			_size_in_bytes(0), _data_size_in_bytes(0), _mapped_file(0)
{
}

data_element::~data_element()
{
}

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

void data_element::merge_input(const data_element& input_element_)
{
	assert(_name == input_element_._name);

	_size = input_element_._size;
	_offset = input_element_._offset;

	_structure.merge_input(input_element_.get_structure());

}

} // namespace stream_process
