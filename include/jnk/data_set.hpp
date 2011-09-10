#ifndef PPK_DATA_SET_HPP
#define PPK_DATA_SET_HPP

#include <string>
#include <jnk/types.hpp>
#include <jnk/traits.hpp>
#include <jnk/stream_range.hpp>
#include <jnk/accessor.hpp>

#include <boost/pimpl.h>

namespace junk
{
namespace detail
{

template<typename Value>
struct raw_data
{
	Value data;
	std::size_t size;
};

} // namespace detail

typedef detail::raw_data<char*> raw_data;
typedef detail::raw_data<const char*> const_raw_data;

struct data_set: pimpl<data_set>::pointer_semantics
{
	data_set(const std::string& filename, bool new_file = false);

	void load(bool new_file);

	void add_element(const char* name, const char* plural = 0);

	template<typename T>
	void add_attribute(const char* element, const char* attribute)
	{
		add_attribute(element, attribute,
				traits::type<T>::value, traits::size<T>::value);
	}

	std::size_t get_size(const char* element) const;
	void set_size(const char* element, std::size_t size);

	junk::raw_data raw_data(std::size_t index);
	junk::const_raw_data raw_data(std::size_t index) const;

	junk::stream_range stream_range(std::size_t index);
	junk::const_stream_range stream_range(std::size_t index) const;

	template<typename T>
	accessor<T> get_accessor(const char* element, const char* attribute) const
	{
		const junk::attribute& attr = get_attribute(element, attribute);

		assert(attr.type == traits::type<T>::value);
		assert(attr.size == traits::size<T>::value);

		return accessor<T>(attr.offset);
	}

//private:
	const element& get_element(const char* name) const;
	const attribute& get_attribute(const char* element, const char* attribute) const;

private:
	element& get_element(const char* name);

	void add_attribute(const char* element, const char* attribute,
			junk::type type, std::size_t size = 1);
};

} // namespace junk

#endif /* PPK_DATA_SET_HPP */
