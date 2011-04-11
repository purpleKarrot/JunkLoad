/*
 * types.hpp
 *
 *  Created on: Apr 10, 2011
 *      Author: daniel
 */

#include "types.hpp"

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

} // namespace junk
