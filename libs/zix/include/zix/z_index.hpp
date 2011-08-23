/*
 * z_index.hpp
 *
 *  Created on: 23.08.2011
 *      Author: daniel
 */

#ifndef ZIX_Z_INDEX_HPP
#define ZIX_Z_INDEX_HPP

#include <cassert>

namespace zix
{

//! calculate a 10bit number from a number between -1 and +1
inline int discretize(float val)
{
	assert(-1.0 <= val && val <= +1.0);
	return (val + 1.0) * 512;
}

//! take a 10bit number and create a 30bit number with 00 between each bit
inline int stretch(int x)
{
	x = (x | (x << 0x10)) & 0x030000FF;
	x = (x | (x << 0x08)) & 0x0300F00F;
	x = (x | (x << 0x04)) & 0x030C30C3;
	x = (x | (x << 0x02)) & 0x09249249;

	return x;
}

inline int z_index(float x, float y, float z)
{
	int a = stretch(discretize(a));
	int b = stretch(discretize(b));
	int c = stretch(discretize(c));

	return a | (b << 1) | (c << 2);
}

} // namespace zix

#endif /* ZIX_Z_INDEX_HPP */
