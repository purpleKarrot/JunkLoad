/*
 * types.hpp
 *
 *  Created on: Apr 10, 2011
 *      Author: daniel
 */

#include <boost/foreach.hpp>
#include <junk/types.hpp>
#include <GL/gl.h>

namespace junk
{

std::size_t size_in_bytes(junk::type type)
{
	static std::size_t type_sizes[] =
	{
		sizeof(int8_t),
		sizeof(int16_t),
		sizeof(int32_t),
		sizeof(uint8_t),
		sizeof(uint16_t),
		sizeof(uint32_t),
		sizeof(float),
		sizeof(double)
	};

	return type_sizes[type];
}

unsigned int gl_type(junk::type type)
{
	static unsigned int types[] =
	{
		GL_BYTE,
		GL_SHORT,
		GL_INT,
		GL_UNSIGNED_BYTE,
		GL_UNSIGNED_SHORT,
		GL_UNSIGNED_INT,
		GL_FLOAT,
		GL_DOUBLE
	};

	return types[type];
}

std::size_t size_in_bytes(const attribute& a)
{
	return a.size * size_in_bytes(a.type);
}

// returns the size of a  point/face/...
std::size_t size_in_bytes(const element& e)
{
	std::size_t size = 0;

	BOOST_FOREACH(const attribute& attr, e.attributes)
	{
		size += size_in_bytes(attr);
	}

	return size;
}

// returns the size of the whole data set
std::size_t file_size_in_bytes(const element& e)
{
	return e.size * size_in_bytes(e);
}

} // namespace junk
