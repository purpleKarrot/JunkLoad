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

unsigned int gl_type(junk::type type);

std::size_t size_in_bytes(junk::type type);

std::size_t size_in_bytes(const attribute& a);

// returns the size of a  point/face/...
std::size_t size_in_bytes(const element& e);

// returns the size of the whole data set
std::size_t file_size_in_bytes(const element& e);

} // namespace junk

#endif /* JUNKLOAD_TYPES_HPP */
