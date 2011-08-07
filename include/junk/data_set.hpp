#ifndef PPK_DATA_SET_HPP
#define PPK_DATA_SET_HPP

#include <string>
#include <junk/types.hpp>
#include <junk/mapped_data_element.hpp>

#include <boost/pimpl.h>

namespace junk
{

struct data_set: pimpl<data_set>::pointer_semantics
{
	data_set(const std::string& filename, bool new_file = false);

	void load(bool new_file);

	junk::header& header();
	const junk::header& header() const;

	mapped_data_element& vertex_map();
	const mapped_data_element& vertex_map() const;

	mapped_data_element& face_map();
	const mapped_data_element& face_map() const;
};

} // namespace junk

#endif /* PPK_DATA_SET_HPP */
