#ifndef __STREAM_PROCESS__ATTRIBUTE_ACCESSOR__HPP__
#define __STREAM_PROCESS__ATTRIBUTE_ACCESSOR__HPP__

#include <cassert>
#include "stream_data.hpp"

namespace stream_process
{

template<typename T>
class attribute_accessor
{
public:
	attribute_accessor(std::size_t offset = 0) :
		offset_(offset)
	{
	}

	T& operator()(stream_data* stream_data_) const
	{
		assert(stream_data_);
		return *reinterpret_cast<T*> (stream_data_ + offset_);
	}

	const T& operator()(const stream_data* stream_data_) const
	{
		assert(stream_data_);
		return *reinterpret_cast<const T*> (stream_data_ + offset_);
	}

private:
	std::size_t offset_;
};

} // namespace stream_process

#endif
