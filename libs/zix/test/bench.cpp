/*
 * bench.cpp
 *
 *  Created on: 23.08.2011
 *      Author: daniel
 */

#include <zix/z_index.hpp>
#include <zix/z_order.hpp>
#include <zix/z_order_int.hpp>

#include <boost/timer.hpp>
#include <boost/random.hpp>
#include <cstdlib>
#include <iostream>

using namespace boost::qvm;

static const int num = 0xFFFFFF;

// not enough space to put all this on the stack..
struct data
{
	boost::qvm::vec<float, 3> a[num];
	boost::qvm::vec<float, 3> b[num];
	bool c[num];
}*d;

int main(int argc, char* argv[])
{
	d = new data;

	boost::mt19937 rng;
	boost::uniform_real<float> u(-1.f, 1.f);
	boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > gen(rng, u);

	// randomize
	for (int i = 0; i < num; ++i)
	{
		d->a[i] % X = gen();
		d->a[i] % Y = gen();
		d->a[i] % Z = gen();

		d->b[i] % X = gen();
		d->b[i] % Y = gen();
		d->b[i] % Z = gen();
	}

	boost::timer t;

	// calc as float
	for (int i = 0; i < num; ++i)
	{
		d->c[i] = zorder_less(d->a[i], d->b[i]);
	}

	std::cout << t.elapsed() << std::endl;
	t.restart();

	// convert to int first
	for (int i = 0; i < num; ++i)
	{
		d->c[i] = zix::zorder_less_int(d->a[i], d->b[i]);
	}

	std::cout << t.elapsed() << std::endl;
	t.restart();

	// compare index
	for (int i = 0; i < num; ++i)
	{
		d->c[i] = zix::z_index(d->a[i]) < zix::z_index(d->b[i]);
	}

	std::cout << t.elapsed() << std::endl;

	return 0;
}
