#ifndef __STREAM_PROCESS__SPECIAL_ACCESSORS__HPP__
#define __STREAM_PROCESS__SPECIAL_ACCESSORS__HPP__

#include <stream_process/VMMLibIncludes.h>
#include <stream_process/point_set.hpp>

namespace stream_process
{

template<size_t M, typename T>
struct smallest_component_accessor
{
	typedef vmml::vector<M, T> vec_type;

	void setup(const point_set& ps, const std::string& attr_name)
	{
		get_attribute = ps.get_accessor<vec_type> (attr_name);
	}

	void setup(size_t offset)
	{
		get_attribute.set_offset(offset);
	}

	inline const T& operator()(const stream_data* d) const
	{
		const vec_type& v = get_attribute(d);
		return v.find_min();
	}

	attribute_accessor<vec_type> get_attribute;
};

// a stream_data accessor that converts an attribute of type T to type U
template<typename T, typename U>
struct conversion_accessor
{
	conversion_accessor(size_t offset)
	{
		get_attribute.set_offset(offset);
	}
	conversion_accessor(const attribute_accessor<T>& acc) :
		get_attribute(acc)
	{
	}

	inline U operator()(const stream_data* d) const
	{
		return static_cast<U> (get_attribute(d));
	}

	attribute_accessor<T> get_attribute;

}; // struct conversion_accessor


} // namespace stream_process

#endif

