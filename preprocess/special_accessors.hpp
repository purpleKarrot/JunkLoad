#ifndef __STREAM_PROCESS__SPECIAL_ACCESSORS__HPP__
#define __STREAM_PROCESS__SPECIAL_ACCESSORS__HPP__

#include <junk/VMMLibIncludes.h>
#include <junk/attribute_accessor.hpp>

namespace junk
{

template<std::size_t M, typename T>
class smallest_component_accessor
{
public:
	smallest_component_accessor(std::size_t offset) :
		get_attribute(offset)
	{
	}

	inline const T& operator()(const char* d) const
	{
		const vec_type& v = get_attribute(d);
		return v.find_min();
	}

private:
	typedef vmml::vector<M, T> vec_type;
	attribute_accessor<vec_type> get_attribute;
};

} // namespace junk

#endif
