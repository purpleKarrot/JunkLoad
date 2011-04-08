//Copyright (c) 2008-2010 Emil Dotchevski and Reverge Studios, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_7E7AB138196311E0907B246CDFD72085
#define UUID_7E7AB138196311E0907B246CDFD72085

#include <boost/qvm/deduce_s.hpp>
#include <boost/qvm/v_traits.hpp>
#include <boost/qvm/static_assert.hpp>

namespace
boost
	{
	namespace
	qvm
		{
		template <class T,int D>
		struct vec;

		namespace
		qvm_detail
			{
			template <class V,int D,
				int VD=v_traits<V>::dim>
			struct
			deduce_v_default
				{
				typedef vec<typename v_traits<V>::scalar_type,D> type;
				};

			template <class V,int D>
			struct
			deduce_v_default<V,D,D>
				{
				typedef V type;
				};
			}

		template <class V,int Dim=v_traits<V>::dim>
		struct
		deduce_v
			{
			BOOST_QVM_STATIC_ASSERT(is_v<V>::value);
			typedef typename qvm_detail::deduce_v_default<V,Dim>::type type;
			};

		namespace
		qvm_detail
			{
			template <class A,class B,int D,
				bool VA=is_v<A>::value,
				bool VB=is_v<B>::value,
				int AD=v_traits<A>::dim,
				int BD=v_traits<B>::dim>
			struct
			deduce_v2_default
				{
				typedef vec<
					typename deduce_s<
						typename scalar<A>::type,
						typename scalar<B>::type>::type,
					D> type;
				};

			template <class V,int D>
			struct
			deduce_v2_default<V,V,D,true,true,D,D>
				{
				typedef V type;
				};
			};

		template <class A,class B,int D>
		struct
		deduce_v2
			{
			BOOST_QVM_STATIC_ASSERT(is_v<A>::value || is_v<B>::value);
			typedef typename qvm_detail::deduce_v2_default<A,B,D>::type type;
			};
		}
	}

#endif