/*
 * bench.cpp
 *
 *  Created on: 23.08.2011
 *      Author: daniel
 */

#include <zix/z_index.hpp>
#include <zix/z_order.hpp>
#include <zix/z_order_int.hpp>

#include <boost/random.hpp>
#include <cstdlib>

using namespace boost::qvm;

int main(int argc, char* argv[])
{
	boost::qvm::vec<float, 3> a[100];
	boost::qvm::vec<float, 3> b[100];

	boost::mt19937 rng;
	boost::uniform_real<float> u(-1, 1);
	boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > gen(rng, u);

	// randomize
	for (int i = 0; i < 100; ++i)
	{
		a[i] % X = gen();
		a[i] % Y = gen();
		a[i] % Z = gen();

		b[i] % X = gen();
		b[i] % Y = gen();
		b[i] % Z = gen();
	}

}
