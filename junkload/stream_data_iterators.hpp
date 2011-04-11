#ifndef __STREAM_PROCESS__STREAM_DATA_ITERATORS__HPP__
#define __STREAM_PROCESS__STREAM_DATA_ITERATORS__HPP__

#include <iterator>

namespace stream_process
{

class stream_data_iterator: public std::iterator<
		std::bidirectional_iterator_tag, char*>
{
public:
	stream_data_iterator(char* data, const size_t size_in_bytes) :
		_data(data), _size_in_bytes(size_in_bytes)
	{
	}

	char* operator*() const
	{
		return _data;
	}

	stream_data_iterator& operator++()
	{
		_data += _size_in_bytes;
		return *this;
	}

	stream_data_iterator operator++(int)
	{
		stream_data_iterator tmp(*this);
		++(*this);
		return tmp;
	}

	stream_data_iterator& operator--()
	{
		_data -= _size_in_bytes;
		return *this;
	}

	stream_data_iterator operator--(int)
	{
		stream_data_iterator tmp(*this);
		--(*this);
		return tmp;
	}

	void operator+=(size_t increment)
	{
		_data += increment * _size_in_bytes;
	}

	stream_data_iterator operator+(size_t increment)
	{
		stream_data_iterator it(*this);
		it += increment;
		return it;
	}

	bool operator==(const stream_data_iterator& other_) const
	{
		return _data == other_._data;
	}

	bool operator!=(const stream_data_iterator& other_) const
	{
		return _data != other_._data;
	}

private:
	char* _data;
	size_t _size_in_bytes;

	stream_data_iterator() :
		_size_in_bytes(0)
	{
	}
};

class stream_data_const_iterator: public std::iterator<
		std::bidirectional_iterator_tag, const char*>
{
public:
	stream_data_const_iterator(const char* data, const size_t size_in_bytes) :
		_data(data), _size_in_bytes(size_in_bytes)
	{
	}

	const char* operator*() const
	{
		return _data;
	}

	stream_data_const_iterator& operator++()
	{
		_data += _size_in_bytes;
		return *this;
	}

	stream_data_const_iterator operator++(int)
	{
		stream_data_const_iterator tmp(*this);
		++(*this);
		return tmp;
	}

	stream_data_const_iterator& operator--()
	{
		_data -= _size_in_bytes;
		return *this;
	}

	stream_data_const_iterator operator--(int)
	{
		stream_data_const_iterator tmp(*this);
		--(*this);
		return tmp;
	}

	void operator+=(size_t increment)
	{
		_data += increment * _size_in_bytes;
	}

	stream_data_const_iterator operator+(size_t increment)
	{
		stream_data_const_iterator it(*this);
		it += increment;
		return it;
	}

	bool operator==(const stream_data_const_iterator& other_) const
	{
		return _data == other_._data;
	}

	bool operator!=(const stream_data_const_iterator& other_) const
	{
		return _data != other_._data;
	}

private:
	const char* _data;
	size_t _size_in_bytes;

	stream_data_const_iterator() :
		_size_in_bytes(0)
	{
	}
};

} // namespace stream_process

#endif
