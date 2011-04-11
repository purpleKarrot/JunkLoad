// Copyright (c) 2010 Daniel Pfeifer

#ifndef __VMML_VECTOR_TRAITS_HPP__
#define __VMML_VECTOR_TRAITS_HPP__

#include <vmmlib/vector.hpp>
#include <boost/qvm/v_traits.hpp>
#include <boost/static_assert.hpp>
#include <boost/assert.hpp>

namespace boost
{
namespace qvm
{

template<size_t M, typename T>
struct v_traits<vmml::vector<M, T> >
{
	typedef T scalar_type;
	typedef vmml::vector<M, T> vector_type;
	static const int dim = M;

	template<int I>
	static scalar_type r(const vector_type& v)
	{
		BOOST_STATIC_ASSERT(I >= 0);
		BOOST_STATIC_ASSERT(I < dim);
		return v.array[I];
	}

	template<int I>
	static scalar_type& w(vector_type& v)
	{
		BOOST_STATIC_ASSERT(I >= 0);
		BOOST_STATIC_ASSERT(I < dim);
		return v.array[I];
	}

	static scalar_type ir(int i, const vector_type& v)
	{
		BOOST_ASSERT(i >= 0);
		BOOST_ASSERT(i < dim);
		return v.array[i];
	}

	static scalar_type& iw(int i, vector_type& v)
	{
		BOOST_ASSERT(i >= 0);
		BOOST_ASSERT(i < dim);
		return v.array[i];
	}
};

} // namespace qvm
} // namespace boost

#endif /* VMML_VECTOR_TRAITS_HPP */
