#ifndef __STREAM_PROCESS__ATTRIBUTE_ACCESSOR__HPP__
#define __STREAM_PROCESS__ATTRIBUTE_ACCESSOR__HPP__

#include <stream_process/attribute.hpp>
#include <stream_process/attribute_type.hpp>
#include <stream_process/stream_structure.hpp>
#include <stream_process/exception.hpp>
#include <stream_process/VMMLibIncludes.h>
#include <stream_process/stream_data.hpp>

#include <string>
#include <cassert>
#include <limits>

namespace stream_process
{

template<typename T>
class attribute_accessor
{
public:
	attribute_accessor();

	inline T& operator()(stream_data* stream_data_);
	inline const T& operator()(const stream_data* stream_data_) const;

	size_t get_offset() const;

	attribute& setup_attribute(stream_structure& ps, const std::string& name,
			bool is_output_, size_t array_size = 0);

	attribute& setup_attribute(const stream_structure& ps,
			const std::string& name);

	void setup();

	void set_offset(size_t offset_); // dangerous

	static const T& get_with_offset(size_t offset, const stream_data* data_);

protected:
	const attribute* _attribute_ptr;
	size_t _offset;
};

template<typename T>
attribute_accessor<T>::attribute_accessor() :
	_attribute_ptr(0), _offset(std::numeric_limits<size_t>::max())
{
}

template<typename T>
inline T& attribute_accessor<T>::operator()(stream_data* stream_data_)
{
	assert(_offset != std::numeric_limits<size_t>::max());
	assert(stream_data_);

	return *reinterpret_cast<T*> (stream_data_ + _offset);
}

template<typename T>
inline const T& attribute_accessor<T>::operator()(
		const stream_data* stream_data_) const
{
	assert(_offset != std::numeric_limits<size_t>::max());
	assert(stream_data_);

	return *reinterpret_cast<const T*> (stream_data_ + _offset);
}

template<typename T>
inline size_t attribute_accessor<T>::get_offset() const
{
	return _offset;
}

template<typename T>
inline attribute& attribute_accessor<T>::setup_attribute(stream_structure& ps,
		const std::string& name, bool is_output_, size_t array_size)
{
	attribute* attr = 0;
	if (!ps.has_attribute(name))
	{
		attr = &ps.create_attribute<T> (name, array_size);
		attr->set_is_output(is_output_);
	}
	else
	{
		attr = &ps.get_attribute(name);
		attribute_type<T> test_type;

		if (!test_type(*attr))
		{
			throw exception(std::string("attempt to initialize accessor ")
					+ " with attribute " + attr->get_name()
					+ ": type mismatch.", SPROCESS_HERE);
		}

		if (!attr->is_output() && is_output_)
			attr->set_is_output(true);
	}

	_attribute_ptr = attr;
	return *attr;
}

template<typename T>
inline attribute& attribute_accessor<T>::setup_attribute(
		const stream_structure& ps, const std::string& name)
{
	attribute* attr = 0;
	if (!ps.has_attribute(name))
	{
		throw exception(std::string("attempt to initialize accessor ")
				+ " with invalid attribute '" + name + "'", SPROCESS_HERE);
	}

	// FIXME
	attr = const_cast<attribute*> (&ps.get_attribute(name));
	attribute_type<T> test_type;
	if (!test_type(*attr))
	{
		// if it's a non-built-in-type, make sure the attribute_type
		// template is implemented for the requested type
		// in file: attribute_type.hpp
		throw exception(std::string("attempt to initialize accessor")
				+ " with attribute " + attr->get_name() + ": type mismatch.",
				SPROCESS_HERE);
	}

	_attribute_ptr = attr;
	return *attr;
}

template<typename T>
inline void attribute_accessor<T>::setup()
{
	assert(_attribute_ptr);
	_offset = _attribute_ptr->get_offset();
}

template<typename T>
inline void attribute_accessor<T>::set_offset(size_t offset_)
{
	_offset = offset_;
}

template<typename T>
const T& attribute_accessor<T>::get_with_offset(size_t offset_,
		const stream_data* data_)
{
	return *reinterpret_cast<const T*> (data_ + offset_);
}

} // namespace stream_process

#endif
