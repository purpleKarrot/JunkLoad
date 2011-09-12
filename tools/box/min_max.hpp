/*
 * min_max.hpp
 *
 *  Created on: Sep 10, 2011
 *      Author: dan
 */

#ifndef MIN_MAX_HPP
#define MIN_MAX_HPP

#include <boost/qvm/all.hpp>

template<typename T>
inline void minimize(T& a, const T& b)
{
	if (a > b)
		a = b;
}

template<typename T>
inline void maximize(T& a, const T& b)
{
	if (a < b)
		a = b;
}

template<>
inline void minimize<position>(position& a, const position& b)
{
	using namespace boost::qvm;

	minimize(a % X, b % X);
	minimize(a % Y, b % Y);
	minimize(a % Z, b % Z);
}

template<>
inline void maximize<position>(position& a, const position& b)
{
	using namespace boost::qvm;

	maximize(a % X, b % X);
	maximize(a % Y, b % Y);
	maximize(a % Z, b % Z);
}

template<typename T>
inline void minimize(T& a, const T& b, const T& c)
{
	minimize(a, b);
	minimize(a, c);
}

template<typename T>
inline void maximize(T& a, const T& b, const T& c)
{
	maximize(a, b);
	maximize(a, c);
}

template<typename T>
inline void minimize(T& a, const T& b, const T& c, const T& d)
{
	minimize(a, b);
	minimize(a, c);
	minimize(a, d);
}

template<typename T>
inline void maximize(T& a, const T& b, const T& c, const T& d)
{
	maximize(a, b);
	maximize(a, c);
	maximize(a, d);
}

#endif /* MIN_MAX_HPP */
