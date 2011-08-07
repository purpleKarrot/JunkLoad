#ifndef PPK_DATA_SET_HPP
#define PPK_DATA_SET_HPP

#include <string>
#include <junk/types.hpp>
#include <junk/stream_range.hpp>

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

	junk::header& header();
	const junk::header& header() const;

	junk::raw_data raw_data(std::size_t index);
	junk::const_raw_data raw_data(std::size_t index) const;

	junk::stream_range stream_range(std::size_t index);
	junk::const_stream_range stream_range(std::size_t index) const;
	//junk::element_range element_range(const std::string& name) const;
};

} // namespace junk

#endif /* PPK_DATA_SET_HPP */
