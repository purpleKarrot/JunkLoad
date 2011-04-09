#ifndef __STREAM_PROCESS__DATA_ELEMENT__HPP__
#define __STREAM_PROCESS__DATA_ELEMENT__HPP__

#include "stream_structure.hpp"

#include <boost/lexical_cast.hpp>
#include <string>

namespace stream_process
{

class mapped_data_element;

class data_element
{
public:
	data_element(const std::string& name) :
		_name(name),
		_structure(name + " attribute"),
		_size(0),
		_offset(0),
		_size_in_bytes(0),
		_data_size_in_bytes(0)
	{
	}

	~data_element()
	{
	}

	stream_structure& get_structure()
	{
		return _structure;
	}

	const stream_structure& get_structure() const
	{
		return _structure;
	}

	void update();

	bool empty() const
	{
		return _size == 0;
	}

	std::size_t size() const
	{
		return _size;
	}

	std::size_t get_size() const
	{
		return _size;
	}

	void set_size(std::size_t size_)
	{
		_size = size_;
		update();
	}

	std::string get_name() const
	{
		return _name;
	}

	// returns the offset (e.g. a binary header before the data blob)
	std::size_t get_offset() const
	{
		return _offset;
	}

	void set_offset(std::size_t offset_)
	{
		_offset = offset_;
	}

	// returns the size of a  point/face/...
	std::size_t get_size_in_bytes() const
	{
		return _size_in_bytes;
	}

	// returns the size of the whole data (sub)set, without offset
	std::size_t get_data_size_in_bytes() const
	{
		return _data_size_in_bytes;
	}

	// returns the size of the data set, plus offset
	std::size_t get_file_size_in_bytes() const
	{
		return _data_size_in_bytes + _offset;
	}

	std::string to_string() const;
	std::string to_header_string() const;

	template<typename container_t>
	bool set_from_strings(const container_t& strings_);

	std::string get_filename(const std::string& base_filename) const;

private:
	std::string _name;

	stream_structure _structure;

	std::size_t _size;
	std::size_t _offset;

	std::size_t _size_in_bytes; // one point
	std::size_t _data_size_in_bytes; // _size * point
};

template<typename container_t>
bool data_element::set_from_strings(const container_t& tokens)
{
	if (tokens.size() < 2)
		return false;

	const std::string& keyword = tokens.front();

	if (keyword == "attribute")
	{
		attribute attrib;
		if (!attrib.from_header_strings(tokens))
			return false;

		_structure.create_attribute(attrib);
		return true;
	}

	if (keyword == "count" || keyword == "size")
	{
		_size = boost::lexical_cast<size_t>(tokens[1]);
		return true;
	}

	if (keyword == "offset")
	{
		_offset = boost::lexical_cast<size_t>(tokens[1]);
		return true;
	}

	return false;
}

} // namespace stream_process

#endif
