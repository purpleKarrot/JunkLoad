#ifndef __STREAM_PROCESS__DATA_ELEMENT__HPP__
#define __STREAM_PROCESS__DATA_ELEMENT__HPP__

#include <stream_process/stream_structure.hpp>

#include <boost/lexical_cast.hpp>
#include <string>

namespace stream_process
{

class mapped_data_element;

class data_element
{
public:
	data_element(const std::string& name);
	~data_element();

	inline stream_structure& get_structure();
	inline const stream_structure& get_structure() const;

	void update();

	inline bool empty() const;
	inline size_t size() const;

	inline size_t get_size() const;
	inline void set_size(size_t size_);

	inline std::string get_name() const;

	// returns the offset (e.g. a binary header before the data blob)
	inline size_t get_offset() const;
	inline void set_offset(size_t offset_);

	// returns the size of a  point/face/...
	inline size_t get_size_in_bytes() const;

	// returns the size of the whole data (sub)set, without offset
	inline size_t get_data_size_in_bytes() const;

	// returns the size of the data set, plus offset
	inline size_t get_file_size_in_bytes() const;

	std::string to_string() const;
	std::string to_header_string() const;

	template<typename container_t>
	bool set_from_strings(const container_t& strings_);

	std::string get_filename(const std::string& base_filename) const;

	void merge_input(const data_element& input_element);

protected:
	std::string _name;

	stream_structure _structure;

	size_t _size;
	size_t _offset;

	size_t _size_in_bytes; // one point
	size_t _data_size_in_bytes; // _size * point

	mapped_data_element* _mapped_file;
};

inline stream_structure&
data_element::get_structure()
{
	return _structure;
}

inline const stream_structure&
data_element::get_structure() const
{
	return _structure;
}

inline bool data_element::empty() const
{
	return _size == 0;
}

inline size_t data_element::size() const
{
	return _size;
}

inline size_t data_element::get_size() const
{
	return _size;
}

inline void data_element::set_size(size_t size_)
{
	_size = size_;
	update();
}

inline std::string data_element::get_name() const
{
	return _name;
}

inline size_t data_element::get_offset() const
{
	return _offset;
}

inline void data_element::set_offset(size_t offset_)
{
	_offset = offset_;
}

inline size_t data_element::get_size_in_bytes() const
{
	return _size_in_bytes;
}

inline size_t data_element::get_data_size_in_bytes() const
{
	return _data_size_in_bytes;
}

inline size_t data_element::get_file_size_in_bytes() const
{
	return _data_size_in_bytes + _offset;
}

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
