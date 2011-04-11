#ifndef __STREAM_PROCESS__ATTRIBUTE__HPP__
#define __STREAM_PROCESS__ATTRIBUTE__HPP__

#include "data_types.hpp"

#include <boost/lexical_cast.hpp>

#include <string>
#include <vector>
#include <functional>

namespace stream_process
{

/**
 *	this class contains the description / meta data of an attribute of a stream_point.
 *	attributes are for example the point position or the normal.
 *
 *	for an array of attributes, use a data_type_id with the array flag set.
 *
 */
class attribute
{
public:
	attribute();

	attribute(const std::string& name_, data_type_id data_type_id_,
			size_t number_of_elements = 1);

	bool is_array() const
	{
		return size > 1;
	}

	bool is_high_precision() const
	{
		return false;
	}

	bool is_output() const
	{
		return true;
	}

public:
	std::string name;
	data_type_id type;
	std::size_t size;
	std::size_t offset;
};

inline std::size_t size_in_bytes(const attribute& a)
{
	const data_type_helper& dth = data_type_helper::get_singleton();
	std::size_t element_size = dth.get_size_in_bytes(a.type);
	return a.size * element_size;
}

} // namespace stream_process

#endif
