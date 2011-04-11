#ifndef __STREAM_PROCESS__ATTRIBUTE_ACCESSOR__HPP__
#define __STREAM_PROCESS__ATTRIBUTE_ACCESSOR__HPP__

namespace stream_process
{

template<typename T>
class attribute_accessor
{
public:
	attribute_accessor(std::size_t offset = 0) :
		offset(offset)
	{
	}

	T& operator()(char* data) const
	{
		return *reinterpret_cast<T*> (data + offset);
	}

	const T& operator()(const char* data) const
	{
		return *reinterpret_cast<const T*> (data + offset);
	}

private:
	std::size_t offset;
};

} // namespace stream_process

#endif
