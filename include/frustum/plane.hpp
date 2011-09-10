/*
 * vertex.hpp
 *
 *  Created on: 10.09.2011
 *      Author: daniel
 */

#ifndef FRUSTUM_PLANE_HPP
#define FRUSTUM_PLANE_HPP

#include <boost/qvm/v_traits.hpp>
#include <boost/static_assert.hpp>
#include <boost/assert.hpp>

namespace frustum
{

template<typename T>
class plane
{
public:
	T a, b, c, d;

private:
	friend struct boost::qvm::v_traits<plane>;
	static T plane<T>::* const mem_array[3];
};

template<typename T>
T plane<T>::* const plane<T>::mem_array[3] =
{
	&Plane<T>::a,
	&Plane<T>::b,
	&Plane<T>::c,
	&Plane<T>::d
};

typedef plane<float> Plane;

} // namespace frustum

namespace boost
{
namespace qvm
{

template<typename T>
struct v_traits<frustum::plane<T> >
{
	typedef T scalar_type;
	typedef frustum::plane<T> vector_type;
	static const int dim = 4;

	template<int I>
	static scalar_type r(const vector_type& v)
	{
		BOOST_STATIC_ASSERT(I >= 0);
		BOOST_STATIC_ASSERT(I < dim);
		return v.*vector_type::mem_array[I];
	}

	template<int I>
	static scalar_type& w(vector_type& v)
	{
		BOOST_STATIC_ASSERT(I >= 0);
		BOOST_STATIC_ASSERT(I < dim);
		return v.*vector_type::mem_array[I];
	}

	static scalar_type ir(int i, const vector_type& v)
	{
		BOOST_ASSERT(i >= 0);
		BOOST_ASSERT(i < dim);
		return v.*vector_type::mem_array[i];
	}

	static scalar_type& iw(int i, vector_type& v)
	{
		BOOST_ASSERT(i >= 0);
		BOOST_ASSERT(i < dim);
		return v.*vector_type::mem_array[i];
	}
};

} // namespace qvm
} // namespace boost

#endif /* FRUSTUM_PLANE_HPP */
