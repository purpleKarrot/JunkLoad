//Copyright (c) 2008-2009 Emil Dotchevski and Reverge Studios, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_4D9687F6B17A11DE85A4B05556D89593
#define UUID_4D9687F6B17A11DE85A4B05556D89593

#include <boost/la/custom/vector_eq_vector.hpp>

namespace
boost
	{
	namespace
	la
		{
		namespace
		la_detail
			{
			template <int D>
			struct
			vector_eq_vector_defined
				{
				static bool const value=false;
				};
			}

		template <class A,class B>
		BOOST_LA_INLINE_OPERATIONS
		typename enable_if_c<
			is_vector<A>::value && is_vector<B>::value &&
			vector_traits<A>::dim==vector_traits<B>::dim &&
			!la_detail::vector_eq_vector_defined<vector_traits<A>::dim>::value,
			bool>::type
		operator==( A const & a, B const & b )
			{
			for( int i=0; i!=vector_traits<A>::dim; ++i )
				if( vector_traits<A>::ir(i,a)!=vector_traits<B>::ir(i,b) )
					return false;
			return true;
			}
		}
	}

#endif
