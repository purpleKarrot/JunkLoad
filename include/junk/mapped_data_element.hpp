#ifndef __STREAM_PROCESS__MAPPED_DATA_ELEMENT__HPP__
#define __STREAM_PROCESS__MAPPED_DATA_ELEMENT__HPP__

#include "types.hpp"
#include "stream_iterator.hpp"

#include <boost/iostreams/device/mapped_file.hpp>

namespace junk
{

class mapped_data_element
{
public:
	mapped_data_element() :
			num_elements(0), element_size(0)
	{
	}

	~mapped_data_element()
	{
	}

	void open(const junk::element& element, const std::string& filename, bool create = false);

	void close()
	{
		num_elements = num_elements = 0;
		mapped_file.close();
	}

	bool is_open() const
	{
		return mapped_file.is_open();
	}

	std::size_t size() const
	{
		return num_elements;
	}

	const junk::element& get_element() const
	{
		return *element;
	}

	char* data()
	{
		return mapped_file.data();
	}

	const char* data() const
	{
		return mapped_file.data();
	}

	std::size_t data_size() const
	{
		return mapped_file.size();
	}

public:
	char* operator[](std::size_t index)
	{
		assert(index < num_elements);
		assert(mapped_file.is_open());
		return mapped_file.data() + index * element_size;
	}

	const char* operator[](std::size_t index) const
	{
		assert(index < num_elements);
		assert(mapped_file.is_open());
		return mapped_file.data() + index * element_size;
	}

	stream_iterator begin()
	{
		assert(mapped_file.is_open());
		return stream_iterator(mapped_file.data(), element_size);
	}

	stream_iterator end()
	{
		assert(mapped_file.is_open());
		return stream_iterator(mapped_file.data() + mapped_file.size(), element_size);
	}

	const_stream_iterator begin() const
	{
		assert(mapped_file.is_open());
		return const_stream_iterator(mapped_file.data(), element_size);
	}

	const_stream_iterator end() const
	{
		assert(mapped_file.is_open());
		return const_stream_iterator(mapped_file.data() + mapped_file.size(), element_size);
	}

private:
	std::size_t num_elements;
	std::size_t element_size;
	const junk::element* element;
	boost::iostreams::mapped_file mapped_file;
};

} // namespace junk

#endif
