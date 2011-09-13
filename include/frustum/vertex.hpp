/*
 * vertex.hpp
 *
 *  Created on: 10.09.2011
 *      Author: daniel
 */

#ifndef FRUSTUM_VERTEX_HPP
#define FRUSTUM_VERTEX_HPP

#include <boost/qvm/v_traits.hpp>
#include <boost/static_assert.hpp>
#include <boost/assert.hpp>

namespace frustum
{

template<typename T>
class vertex
{
public:
	vertex()
	{
	}

	vertex(T x, T y, T z) :
			x(x), y(y), z(z)
	{
	}

	T x, y, z;

private:
	friend struct boost::qvm::v_traits<vertex>;
	static T vertex<T>::* const mem_array[3];
};

template<typename T>
T vertex<T>::* const vertex<T>::mem_array[3] =
{
	&vertex<T>::x,
	&vertex<T>::y,
	&vertex<T>::z
};

typedef vertex<float> Vertex;

} // namespace frustum

namespace boost
{
namespace qvm
{

template<typename T>
struct v_traits<frustum::vertex<T> >
{
	typedef T scalar_type;
	typedef frustum::vertex<T> vector_type;
	static const int dim = 3;

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

#endif /* FRUSTUM_VERTEX_HPP */
