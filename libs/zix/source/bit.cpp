/*
 * bit.cpp
 *
 *  Created on: 20.08.2011
 *      Author: daniel
 */

//! calculate a 10bit number from a number between -1 and +1
int discretize(double val)
{
	assert(-1.0 <= val && val <= +1.0);
	return (val + 1.0) * 512;
}

//! take a 10bit number and create a 30bit number with 00 between each bit
int stretch(int x)
{
	x = (x | (x << 0x10)) & 0x030000FF;
	x = (x | (x << 0x08)) & 0x0300F00F;
	x = (x | (x << 0x04)) & 0x030C30C3;
	x = (x | (x << 0x02)) & 0x09249249;

	return x;
}

int morton(double x, double y, double z)
{
	int a = stretch(discretize(x));
	int b = stretch(discretize(y));
	int c = stretch(discretize(z));

	return a | (b << 1) | (c << 2);
}

void box(int& min, int& max)
{
	int mask = min ^ max;

	mask |= maks >> 1;
	mask |= maks >> 2;
	mask |= maks >> 4;
	mask |= maks >> 8;
	mask |= maks >> 16;

	min &= ~mask;
	max |= mask;
}
