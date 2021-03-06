#ifndef JUNK_ATTRIBUTE_ACCESSOR_HPP
#define JUNK_ATTRIBUTE_ACCESSOR_HPP

#include <cstddef>

namespace junk
{

template<typename T>
class accessor
{
public:
	accessor(std::ptrdiff_t offset = 0) :
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
	std::ptrdiff_t offset;
};

} // namespace junk

#endif /* JUNK_ATTRIBUTE_ACCESSOR_HPP */
