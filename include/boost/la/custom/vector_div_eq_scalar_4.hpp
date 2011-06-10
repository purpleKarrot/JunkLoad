//Copyright (c) 2009 Emil Dotchevski and Reverge Studios, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_LA_CUSTOM_VECTOR_DIV_EQ_SCALAR_4_HPP_INCLUDED
#define BOOST_LA_CUSTOM_VECTOR_DIV_EQ_SCALAR_4_HPP_INCLUDED

//This file was generated by a program. Do not edit manually.

#include <boost/la/config.hpp>
#include <boost/la/vector_traits.hpp>
#include <boost/utility/enable_if.hpp>

namespace
boost
    {
    namespace
    la
        {
        template <class A>
        BOOST_LA_INLINE_OPERATIONS
        typename enable_if_c<
            vector_traits<A>::dim==4,
            A &>::type
        operator/=( A & a, typename vector_traits<A>::scalar_type b )
            {
            vector_traits<A>::template w<0>(a)/=b;
            vector_traits<A>::template w<1>(a)/=b;
            vector_traits<A>::template w<2>(a)/=b;
            vector_traits<A>::template w<3>(a)/=b;
            return a;
            }

        namespace
        la_detail
            {
            template <int D>
            struct vector_div_eq_scalar_defined;

            template <>
            struct
            vector_div_eq_scalar_defined<4>
                {
                static bool const value=true;
                };
            }
        }
    }

#endif
