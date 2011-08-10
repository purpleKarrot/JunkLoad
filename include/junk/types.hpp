/*
 * types.hpp
 *
 *  Created on: Apr 10, 2011
 *      Author: daniel
 */

#ifndef JUNKLOAD_TYPES_HPP
#define JUNKLOAD_TYPES_HPP

#include <string>
#include <vector>

namespace junk
{

enum type
{
	s_int_08,
	s_int_16,
	s_int_32,
	u_int_08,
	u_int_16,
	u_int_32,
	float_32,
	float_64,
};

struct attribute
{
	attribute()
	{
	}

	attribute(junk::type type, const std::string& name, std::size_t size, std::size_t offset) :
			type(type), name(name), size(size), offset(offset)
	{
	}

	junk::type type;
	std::string name;
	std::size_t size;
	std::size_t offset;
};

typedef std::vector<attribute> attrib_list;

struct element
{
	attrib_list attributes;
	std::string name_sg;
	std::string name_pl;
	std::size_t size;
};

typedef std::vector<element> element_list;

} // namespace junk


#include <iostream>
#include <junk/traits.hpp>
#include <boost/foreach.hpp>

namespace junk
{

std::size_t size_in_bytes(junk::type type);

unsigned int gl_type(junk::type type);

inline std::size_t size_in_bytes(const attribute& a)
{
	return a.size * size_in_bytes(a.type);
}

// returns the size of a  point/face/...
inline std::size_t size_in_bytes(const element& e)
{
	std::size_t size = 0;

	BOOST_FOREACH(const attribute& attr, e.attributes)
	{
		size += size_in_bytes(attr);
	}

	return size;
}

// returns the size of the whole data set
inline std::size_t file_size_in_bytes(const element& e)
{
	return e.size * size_in_bytes(e);
}

} // namespace junk

#endif /* JUNKLOAD_TYPES_HPP */
