//Copyright (c) 2008-2010 Emil Dotchevski and Reverge Studios, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_384AFF3AD23A11DFA80B754FE0D72085
#define UUID_384AFF3AD23A11DFA80B754FE0D72085

#include <boost/qvm/v2.hpp>
#include <boost/qvm/v3.hpp>
#include <boost/qvm/v4.hpp>
#include <boost/qvm/assert.hpp>

namespace
boost
    {
    namespace
    qvm
        {
        ////////////////////////////////////////////////

        namespace
        qvm_detail
            {
            template <int D>
            struct
            assign_vv_defined
                {
                static bool const value=false;
                };

            template <int I,int N>
            struct
            copy_vector_elements
                {
                template <class A,class B>
                static
                void
                f( A & a, B const & b )
                    {
                    v_traits<A>::template w<I>(a)=v_traits<B>::template r<I>(b);
                    copy_vector_elements<I+1,N>::f(a,b);
                    }
                };

            template <int N>
            struct
            copy_vector_elements<N,N>
                {
                template <class A,class B>
                static
                void
                f( A & a, B const & b )
                    {
                    }
                };
            }

        template <class A,class B>
        inline
        typename boost::enable_if_c<
            is_v<A>::value && is_v<B>::value &&
            v_traits<A>::dim==v_traits<B>::dim &&
            !qvm_detail::assign_vv_defined<v_traits<A>::dim>::value,
            A &>::type
        assign( A & a, B const & b )
            {
            qvm_detail::copy_vector_elements<0,v_traits<A>::dim>::f(a,b);
            return a;
            }

        ////////////////////////////////////////////////

        namespace
        qvm_detail
            {
            template <int D>
            struct
            make_v_defined
                {
                static bool const value=false;
                };
            }

        template <class R,class A>
        BOOST_QVM_INLINE_TRIVIAL
        typename enable_if_c<
            is_v<R>::value && is_v<A>::value &&
            v_traits<R>::dim==v_traits<A>::dim &&
            !qvm_detail::make_v_defined<v_traits<R>::dim>::value,
            R>::type
        make( A const & a )
            {
            R r; assign(r,a);
            return r;
            }

        ////////////////////////////////////////////////

        template <class A,class B>
        BOOST_QVM_INLINE_OPERATIONS
        typename lazy_enable_if_c<
            is_v<A>::value && is_v<B>::value &&
            v_traits<A>::dim==3 && v_traits<B>::dim==3,
            deduce_v2<A,B,3> >::type
        cross( A const & a, B const & b )
            {
            typedef typename deduce_v2<A,B,3>::type R;
            R r;
            v_traits<R>::template w<0>(r)=
                v_traits<A>::template r<1>(a)*v_traits<B>::template r<2>(b)-
                v_traits<A>::template r<2>(a)*v_traits<B>::template r<1>(b);
            v_traits<R>::template w<1>(r)=
                v_traits<A>::template r<2>(a)*v_traits<B>::template r<0>(b)-
                v_traits<A>::template r<0>(a)*v_traits<B>::template r<2>(b);
            v_traits<R>::template w<2>(r)=
                v_traits<A>::template r<0>(a)*v_traits<B>::template r<1>(b)-
                v_traits<A>::template r<1>(a)*v_traits<B>::template r<0>(b);
            return r;
            }

        ////////////////////////////////////////////////

        template <class A,class B,class Cmp>
        BOOST_QVM_INLINE_OPERATIONS
        typename enable_if_c<
            is_v<A>::value && is_v<B>::value &&
            v_traits<A>::dim==v_traits<B>::dim,
            bool>::type
        cmp( A const & a, B const & b, Cmp f )
            {
            typedef typename deduce_s<
                typename v_traits<A>::scalar_type,
                typename v_traits<B>::scalar_type>::type T;
            int const dim=v_traits<A>::dim;
            T v1[dim]; assign(v1,a);
            T v2[dim]; assign(v2,b);
            for( int i=0; i!=dim; ++i )
                if( !f(v1[i],v2[i]) )
                    return false;
            return true;
            }

        ////////////////////////////////////////////////

        namespace
        qvm_detail
            {
            template <class T,int Dim>
            class
            zero_v_
                {
                zero_v_( zero_v_ const & );
                zero_v_ & operator=( zero_v_ const & );
                ~zero_v_();

                public:

                template <class R>
                BOOST_QVM_INLINE_TRIVIAL
                operator R() const
                    {
                    R r;
                    assign(r,*this);
                    return r;
                    }
                };
            }

        template <class V>
        struct v_traits;

        template <class T,int Dim>
        struct
        v_traits< qvm_detail::zero_v_<T,Dim> >
            {
            typedef qvm_detail::zero_v_<T,Dim> this_vector;
            typedef T scalar_type;
            static int const dim=Dim;

            template <int I>
            static
            BOOST_QVM_INLINE_CRITICAL
            scalar_type
            r( this_vector const & x )
                {
                BOOST_QVM_ASSERT(&x==0);
                BOOST_QVM_STATIC_ASSERT(I>=0);
                BOOST_QVM_STATIC_ASSERT(I<Dim);
                return s_traits<scalar_type>::value(0);
                }

            static
            BOOST_QVM_INLINE_CRITICAL
            scalar_type
            ir( int i, this_vector const & x )
                {
                BOOST_QVM_ASSERT(&x==0);
                BOOST_QVM_ASSERT(i>=0);
                BOOST_QVM_ASSERT(i<Dim);
                return s_traits<scalar_type>::value(0);
                }
            };

        template <class T,int Dim,int D>
        struct
        deduce_v<qvm_detail::zero_v_<T,Dim>,D>
            {
            typedef vec<T,D> type;
            };

        template <class T,int Dim>
        BOOST_QVM_INLINE_TRIVIAL
        qvm_detail::zero_v_<T,Dim> const &
        zero_v()
            {
            return *(qvm_detail::zero_v_<T,Dim> const *)0;
            }

        template <class A>
        BOOST_QVM_INLINE_OPERATIONS
        typename enable_if_c<
            is_v<A>::value,
            void>::type
        set_zero( A & a )
            {
            assign(a,zero_v<typename v_traits<A>::scalar_type,v_traits<A>::dim>());
            }

        ////////////////////////////////////////////////

        namespace
        qvm_detail
            {
            template <class OriginalType,class Scalar>
            class
            vector_scalar_cast_
                {
                vector_scalar_cast_( vector_scalar_cast_ const & );
                vector_scalar_cast_ & operator=( vector_scalar_cast_ const & );
                ~vector_scalar_cast_();

                public:

                template <class T>
                BOOST_QVM_INLINE_TRIVIAL
                vector_scalar_cast_ &
                operator=( T const & x )
                    {
                    assign(*this,x);
                    return *this;
                    }

                template <class R>
                BOOST_QVM_INLINE_TRIVIAL
                operator R() const
                    {
                    R r;
                    assign(r,*this);
                    return r;
                    }
                };

            template <bool> struct scalar_cast_vector_filter { };
            template <> struct scalar_cast_vector_filter<true> { typedef int type; };
            }

        template <class OriginalType,class Scalar>
        struct
        v_traits< qvm_detail::vector_scalar_cast_<OriginalType,Scalar> >
            {
            typedef Scalar scalar_type;
            typedef qvm_detail::vector_scalar_cast_<OriginalType,Scalar> this_vector;
            static int const dim=v_traits<OriginalType>::dim;

            template <int I>
            static
            BOOST_QVM_INLINE_CRITICAL
            scalar_type
            r( this_vector const & x )
                {
                BOOST_QVM_STATIC_ASSERT(I>=0);
                BOOST_QVM_STATIC_ASSERT(I<dim);
                return scalar_type(v_traits<OriginalType>::template r<I>(reinterpret_cast<OriginalType const &>(x)));
                }

            static
            BOOST_QVM_INLINE_CRITICAL
            scalar_type
            ir( int i, this_vector const & x )
                {
                BOOST_QVM_ASSERT(i>=0);
                BOOST_QVM_ASSERT(i<dim);
                return scalar_type(v_traits<OriginalType>::ir(i,reinterpret_cast<OriginalType const &>(x)));
                }
            };

        template <class OriginalType,class Scalar,int D>
        struct
        deduce_v<qvm_detail::vector_scalar_cast_<OriginalType,Scalar>,D>
            {
            typedef vec<Scalar,D> type;
            };

        template <class Scalar,class T>
        BOOST_QVM_INLINE_TRIVIAL
        qvm_detail::vector_scalar_cast_<T,Scalar> const &
        scalar_cast( T const & x, typename qvm_detail::scalar_cast_vector_filter<is_v<T>::value>::type=0 )
            {
            return reinterpret_cast<qvm_detail::vector_scalar_cast_<T,Scalar> const &>(x);
            }

        ////////////////////////////////////////////////

        namespace
        qvm_detail
            {
            template <int D>
            struct
            div_eq_vs_defined
                {
                static bool const value=false;
                };
            }

        template <class A>
        BOOST_QVM_INLINE_OPERATIONS
        typename enable_if_c<
            is_v<A>::value &&
            !qvm_detail::div_eq_vs_defined<v_traits<A>::dim>::value,
            A &>::type
        operator/=( A & a, typename v_traits<A>::scalar_type b )
            {
            for( int i=0; i!=v_traits<A>::dim; ++i )
                v_traits<A>::iw(i,a)/=b;
            return a;
            }

        ////////////////////////////////////////////////

        namespace
        qvm_detail
            {
            template <int D>
            struct
            div_vs_defined
                {
                static bool const value=false;
                };
            }

        template <class A>
        BOOST_QVM_INLINE_OPERATIONS
        typename lazy_enable_if_c<
            is_v<A>::value &&
            !qvm_detail::div_vs_defined<v_traits<A>::dim>::value,
            deduce_v<A> >::type
        operator/( A const & a, typename v_traits<A>::scalar_type b )
            {
            typedef typename deduce_v<A>::type R;
            R r;
            for( int i=0; i!=v_traits<A>::dim; ++i )
                v_traits<R>::iw(i,r)=v_traits<A>::ir(i,a)/b;
            return r;
            }

        ////////////////////////////////////////////////

        namespace
        qvm_detail
            {
            template <int D>
            struct
            dot_vv_defined
                {
                static bool const value=false;
                };
            }

        template <class A,class B>
        BOOST_QVM_INLINE_OPERATIONS
        typename lazy_enable_if_c<
            is_v<A>::value && is_v<B>::value &&
            v_traits<A>::dim==v_traits<B>::dim &&
            !qvm_detail::dot_vv_defined<v_traits<A>::dim>::value,
            deduce_s<typename v_traits<A>::scalar_type,typename v_traits<B>::scalar_type> >::type
        dot( A const & a, B const & b )
            {
            typedef typename deduce_s<typename v_traits<A>::scalar_type,typename v_traits<B>::scalar_type>::type T;
            T m(s_traits<T>::value(0));
            for( int i=0; i!=v_traits<A>::dim; ++i )
                m+=v_traits<A>::ir(i,a)*v_traits<B>::ir(i,b);
            return m;
            }

        ////////////////////////////////////////////////

        namespace
        qvm_detail
            {
            template <int D>
            struct
            eq_vv_defined
                {
                static bool const value=false;
                };
            }

        template <class A,class B>
        BOOST_QVM_INLINE_OPERATIONS
        typename enable_if_c<
            is_v<A>::value && is_v<B>::value &&
            v_traits<A>::dim==v_traits<B>::dim &&
            !qvm_detail::eq_vv_defined<v_traits<A>::dim>::value,
            bool>::type
        operator==( A const & a, B const & b )
            {
            for( int i=0; i!=v_traits<A>::dim; ++i )
                if( v_traits<A>::ir(i,a)!=v_traits<B>::ir(i,b) )
                    return false;
            return true;
            }

        ////////////////////////////////////////////////

        namespace
        qvm_detail
            {
            template <int D>
            struct
            mag2_v_defined
                {
                static bool const value=false;
                };
            }

        template <class A>
        BOOST_QVM_INLINE_OPERATIONS
        typename enable_if_c<
            is_v<A>::value &&
            !qvm_detail::mag2_v_defined<v_traits<A>::dim>::value,
            typename v_traits<A>::scalar_type>::type
        mag2( A const & a )
            {
            typedef typename v_traits<A>::scalar_type T;
            T m(s_traits<T>::value(0));
            for( int i=0; i!=v_traits<A>::dim; ++i )
                {
                T x=v_traits<A>::ir(i,a);
                m+=x*x;
                }
            return m;
            }

        ////////////////////////////////////////////////

        namespace
        qvm_detail
            {
            template <int D>
            struct
            mag_v_defined
                {
                static bool const value=false;
                };
            }

        template <class A>
        BOOST_QVM_INLINE_OPERATIONS
        typename enable_if_c<
            is_v<A>::value &&
            !qvm_detail::mag_v_defined<v_traits<A>::dim>::value,
            typename v_traits<A>::scalar_type>::type
        mag( A const & a )
            {
            typedef typename v_traits<A>::scalar_type T;
            T m(s_traits<T>::value(0));
            for( int i=0; i!=v_traits<A>::dim; ++i )
                {
                T x=v_traits<A>::ir(i,a);
                m+=x*x;
                }
            return sqrt<T>(m);
            }

        ////////////////////////////////////////////////

        namespace
        qvm_detail
            {
            template <int D>
            struct
            minus_eq_vv_defined
                {
                static bool const value=false;
                };
            }

        template <class A,class B>
        BOOST_QVM_INLINE_OPERATIONS
        typename enable_if_c<
            is_v<A>::value && is_v<B>::value &&
            v_traits<A>::dim==v_traits<B>::dim &&
            !qvm_detail::minus_eq_vv_defined<v_traits<A>::dim>::value,
            A &>::type
        operator-=( A & a, B const & b )
            {
            for( int i=0; i!=v_traits<A>::dim; ++i )
                v_traits<A>::iw(i,a)-=v_traits<B>::ir(i,b);
            return a;
            }

        ////////////////////////////////////////////////

        namespace
        qvm_detail
            {
            template <int D>
            struct
            minus_v_defined
                {
                static bool const value=false;
                };
            }

        template <class A>
        BOOST_QVM_INLINE_OPERATIONS
        typename lazy_enable_if_c<
            is_v<A>::value &&
            !qvm_detail::minus_v_defined<v_traits<A>::dim>::value,
            deduce_v<A> >::type
        operator-( A const & a )
            {
            typedef typename deduce_v<A>::type R;
            R r;
            for( int i=0; i!=v_traits<A>::dim; ++i )
                v_traits<R>::iw(i,r)=-v_traits<A>::ir(i,a);
            return r;
            }

        ////////////////////////////////////////////////

        namespace
        qvm_detail
            {
            template <int D>
            struct
            minus_vv_defined
                {
                static bool const value=false;
                };
            }

        template <class A,class B>
        BOOST_QVM_INLINE_OPERATIONS
        typename lazy_enable_if_c<
            is_v<A>::value && is_v<B>::value &&
            v_traits<A>::dim==v_traits<B>::dim &&
            !qvm_detail::minus_vv_defined<v_traits<A>::dim>::value,
            deduce_v2<A,B,v_traits<A>::dim> >::type
        operator-( A const & a, B const & b )
            {
            typedef typename deduce_v2<A,B,v_traits<A>::dim>::type R;
            R r;
            for( int i=0; i!=v_traits<A>::dim; ++i )
                v_traits<R>::iw(i,r)=v_traits<A>::ir(i,a)-v_traits<B>::ir(i,b);
            return r;
            }

        ////////////////////////////////////////////////

        namespace
        qvm_detail
            {
            template <int D>
            struct
            mul_eq_vs_defined
                {
                static bool const value=false;
                };
            }

        template <class A>
        BOOST_QVM_INLINE_OPERATIONS
        typename enable_if_c<
            is_v<A>::value &&
            !qvm_detail::mul_eq_vs_defined<v_traits<A>::dim>::value,
            A &>::type
        operator*=( A & a, typename v_traits<A>::scalar_type b )
            {
            for( int i=0; i!=v_traits<A>::dim; ++i )
                v_traits<A>::iw(i,a)*=b;
            return a;
            }

        ////////////////////////////////////////////////

        namespace
        qvm_detail
            {
            template <int D>
            struct
            mul_vs_defined
                {
                static bool const value=false;
                };
            }

        template <class A>
        BOOST_QVM_INLINE_OPERATIONS
        typename lazy_enable_if_c<
            is_v<A>::value &&
            !qvm_detail::mul_vs_defined<v_traits<A>::dim>::value,
            deduce_v<A> >::type
        operator*( A const & a, typename v_traits<A>::scalar_type b )
            {
            typedef typename deduce_v<A>::type R;
            R r;
            for( int i=0; i!=v_traits<A>::dim; ++i )
                v_traits<R>::iw(i,r)=v_traits<A>::ir(i,a)*b;
            return r;
            }

        ////////////////////////////////////////////////

        namespace
        qvm_detail
            {
            template <int D>
            struct
            neq_vv_defined
                {
                static bool const value=false;
                };
            }

        template <class A,class B>
        BOOST_QVM_INLINE_OPERATIONS
        typename enable_if_c<
            is_v<A>::value && is_v<B>::value &&
            v_traits<A>::dim==v_traits<B>::dim &&
            !qvm_detail::neq_vv_defined<v_traits<A>::dim>::value,
            bool>::type
        operator!=( A const & a, B const & b )
            {
            for( int i=0; i!=v_traits<A>::dim; ++i )
                if( v_traits<A>::ir(i,a)!=v_traits<B>::ir(i,b) )
                    return true;
            return false;
            }

        ////////////////////////////////////////////////

        namespace
        qvm_detail
            {
            template <int D>
            struct
            normalize_v_defined
                {
                static bool const value=false;
                };
            }

        template <class A>
        BOOST_QVM_INLINE_OPERATIONS
        typename lazy_enable_if_c<
            is_v<A>::value &&
            !qvm_detail::normalize_v_defined<v_traits<A>::dim>::value,
            deduce_v<A> >::type
        normalized( A const & a )
            {
            typedef typename v_traits<A>::scalar_type T;
            T m(s_traits<T>::value(0));
            for( int i=0; i!=v_traits<A>::dim; ++i )
                {
                T x=v_traits<A>::ir(i,a);
                m+=x*x;
                }
            if( m==s_traits<T>::value(0) )
                BOOST_THROW_EXCEPTION(zero_magnitude_error());
            T rm=s_traits<T>::value(1)/sqrt<T>(m);
            typedef typename deduce_v<A>::type R;
            R r;
            for( int i=0; i!=v_traits<A>::dim; ++i )
                v_traits<R>::iw(i,r)=v_traits<A>::ir(i,a)*rm;
            return r;
            }

        template <class A>
        BOOST_QVM_INLINE_OPERATIONS
        typename enable_if_c<
            is_v<A>::value &&
            !qvm_detail::normalize_v_defined<v_traits<A>::dim>::value,
            void>::type
        normalize( A & a )
            {
            typedef typename v_traits<A>::scalar_type T;
            T m(s_traits<T>::value(0));
            for( int i=0; i!=v_traits<A>::dim; ++i )
                {
                T x=v_traits<A>::ir(i,a);
                m+=x*x;
                }
            if( m==s_traits<T>::value(0) )
                BOOST_THROW_EXCEPTION(zero_magnitude_error());
            T rm=s_traits<T>::value(1)/sqrt<T>(m);
            for( int i=0; i!=v_traits<A>::dim; ++i )
                v_traits<A>::iw(i,a)*=rm;
            }

        ////////////////////////////////////////////////

        namespace
        qvm_detail
            {
            template <int D>
            struct
            plus_eq_vv_defined
                {
                static bool const value=false;
                };
            }

        template <class A,class B>
        BOOST_QVM_INLINE_OPERATIONS
        typename enable_if_c<
            is_v<A>::value && is_v<B>::value &&
            v_traits<A>::dim==v_traits<B>::dim &&
            !qvm_detail::plus_eq_vv_defined<v_traits<A>::dim>::value,
            A &>::type
        operator+=( A & a, B const & b )
            {
            for( int i=0; i!=v_traits<A>::dim; ++i )
                v_traits<A>::iw(i,a)+=v_traits<B>::ir(i,b);
            return a;
            }

        ////////////////////////////////////////////////

        namespace
        qvm_detail
            {
            template <int D>
            struct
            plus_vv_defined
                {
                static bool const value=false;
                };
            }

        template <class A,class B>
        BOOST_QVM_INLINE_OPERATIONS
        typename lazy_enable_if_c<
            is_v<A>::value && is_v<B>::value &&
            v_traits<A>::dim==v_traits<B>::dim &&
            !qvm_detail::plus_vv_defined<v_traits<A>::dim>::value,
            deduce_v2<A,B,v_traits<A>::dim> >::type
        operator+( A const & a, B const & b )
            {
            typedef typename deduce_v2<A,B,v_traits<A>::dim>::type R;
            R r;
            for( int i=0; i!=v_traits<A>::dim; ++i )
                v_traits<R>::iw(i,r)=v_traits<A>::ir(i,a)+v_traits<B>::ir(i,b);
            return r;
            }

        ////////////////////////////////////////////////

        namespace
        qvm_detail
            {
            template <class T>
            class
            vref_
                {
                vref_( vref_ const & );
                vref_ & operator=( vref_ const & );
                ~vref_();

                public:

                template <class R>
                BOOST_QVM_INLINE_TRIVIAL
                vref_ &
                operator=( R const & x )
                    {
                    assign(*this,x);
                    return *this;
                    }

                template <class R>
                BOOST_QVM_INLINE_TRIVIAL
                operator R() const
                    {
                    R r;
                    assign(r,*this);
                    return r;
                    }
                };
            }

        template <class V>
        struct
        v_traits< qvm_detail::vref_<V> >
            {
            typedef typename v_traits<V>::scalar_type scalar_type;
            typedef qvm_detail::vref_<V> this_vector;
            static int const dim=v_traits<V>::dim;

            template <int I>
            static
            BOOST_QVM_INLINE_CRITICAL
            scalar_type
            r( this_vector const & x )
                {
                BOOST_QVM_STATIC_ASSERT(I>=0);
                BOOST_QVM_STATIC_ASSERT(I<dim);
                return v_traits<V>::template r<I>(reinterpret_cast<V const &>(x));
                }

            template <int I>
            static
            BOOST_QVM_INLINE_CRITICAL
            scalar_type &
            w( this_vector & x )
                {
                BOOST_QVM_STATIC_ASSERT(I>=0);
                BOOST_QVM_STATIC_ASSERT(I<dim);
                return v_traits<V>::template w<I>(reinterpret_cast<V &>(x));
                }

            static
            BOOST_QVM_INLINE_CRITICAL
            scalar_type
            ir( int i, this_vector const & x )
                {
                BOOST_QVM_ASSERT(i>=0);
                BOOST_QVM_ASSERT(i<dim);
                return v_traits<V>::ir(i,reinterpret_cast<V const &>(x));
                }

            static
            BOOST_QVM_INLINE_CRITICAL
            scalar_type &
            iw( int i, this_vector & x )
                {
                BOOST_QVM_ASSERT(i>=0);
                BOOST_QVM_ASSERT(i<dim);
                return v_traits<V>::iw(i,reinterpret_cast<V &>(x));
                }
            };

        template <class V,int D>
        struct
        deduce_v<qvm_detail::vref_<V>,D>
            {
            typedef vec<typename v_traits<V>::scalar_type,D> type;
            };

        template <class V>
        BOOST_QVM_INLINE_TRIVIAL
        typename enable_if_c<
            is_v<V>::value,
            qvm_detail::vref_<V> const &>::type
        vref( V const & a )
            {
            return reinterpret_cast<qvm_detail::vref_<V> const &>(a);
            }

        template <class V>
        BOOST_QVM_INLINE_TRIVIAL
        typename enable_if_c<
            is_v<V>::value,
            qvm_detail::vref_<V> &>::type
        vref( V & a )
            {
            return reinterpret_cast<qvm_detail::vref_<V> &>(a);
            }

        ////////////////////////////////////////////////

        namespace
        sfinae
            {
            using ::boost::qvm::assign;
            using ::boost::qvm::make;
            using ::boost::qvm::cross;
            using ::boost::qvm::cmp;
            using ::boost::qvm::set_zero;
            using ::boost::qvm::scalar_cast;
            using ::boost::qvm::operator/=;
            using ::boost::qvm::operator/;
            using ::boost::qvm::dot;
            using ::boost::qvm::operator==;
            using ::boost::qvm::mag2;
            using ::boost::qvm::mag;
            using ::boost::qvm::operator-=;
            using ::boost::qvm::operator-;
            using ::boost::qvm::operator*=;
            using ::boost::qvm::operator*;
            using ::boost::qvm::operator!=;
            using ::boost::qvm::normalized;
            using ::boost::qvm::normalize;
            using ::boost::qvm::operator+=;
            using ::boost::qvm::operator+;
            using ::boost::qvm::vref;
            }

        ////////////////////////////////////////////////
        }
    }

#endif
