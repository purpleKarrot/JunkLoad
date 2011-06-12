/*
 * types.hpp
 *
 *  Created on: Apr 10, 2011
 *      Author: daniel
 */

#include <junk/types.hpp>
#include <GL/gl.h>

namespace junk
{

std::size_t size_in_bytes(typid type)
{
	static std::size_t type_sizes[] =
	{
		sizeof(int8_t),
		sizeof(int16_t),
		sizeof(int32_t),
		sizeof(int64_t),
		sizeof(uint8_t),
		sizeof(uint16_t),
		sizeof(uint32_t),
		sizeof(uint64_t),
		sizeof(float),
		sizeof(double)
	};

	return type_sizes[type];
}

unsigned int gl_type(typid type)
{
	static unsigned int types[] =
	{
		GL_BYTE,
		GL_SHORT,
		GL_INT,
		0, // GL_INT64,
		GL_UNSIGNED_BYTE,
		GL_UNSIGNED_SHORT,
		GL_UNSIGNED_INT,
		0, // GL_UNSIGNED_INT64,
		GL_FLOAT,
		GL_DOUBLE
	};

	return types[type];
}

} // namespace junk
