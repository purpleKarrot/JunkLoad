//Copyright (c) 2008-2010 Emil Dotchevski and Reverge Studios, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_E6519754D19211DFB8405F74DFD72085
#define UUID_E6519754D19211DFB8405F74DFD72085

#include <boost/qvm/inline.hpp>
#include <boost/qvm/enable_if.hpp>
#include <boost/qvm/deduce_q.hpp>
#include <boost/qvm/m_traits.hpp>
#include <boost/qvm/math.hpp>
#include <boost/qvm/assert.hpp>
#include <boost/qvm/error.hpp>
#include <boost/throw_exception.hpp>

namespace
boost
	{
	namespace
	qvm
		{
		template <class A,class B>
		BOOST_QVM_INLINE_OPERATIONS
		typename enable_if_c<
			is_q<A>::value && is_q<B>::value,
			A &>::type
		assign( A & a, B const & b )
			{
			q_traits<A>::template w<0>(a) = q_traits<B>::template r<0>(b);
			q_traits<A>::template w<1>(a) = q_traits<B>::template r<1>(b);
			q_traits<A>::template w<2>(a) = q_traits<B>::template r<2>(b);
			q_traits<A>::template w<3>(a) = q_traits<B>::template r<3>(b);
			return a;
			}

		template <class A,class B,class Cmp>
		BOOST_QVM_INLINE_OPERATIONS
		typename enable_if_c<
			is_q<A>::value && is_q<B>::value,
			bool>::type
		cmp( A const & a, B const & b, Cmp f )
			{
			typedef typename deduce_s<
				typename q_traits<A>::scalar_type,
				typename q_traits<B>::scalar_type>::type T;
			T q1[4] =
				{
				q_traits<A>::template r<0>(a),
				q_traits<A>::template r<1>(a),
				q_traits<A>::template r<2>(a),
				q_traits<A>::template r<3>(a)
				};
			T q2[4] =
				{
				q_traits<B>::template r<0>(b),
				q_traits<B>::template r<1>(b),
				q_traits<B>::template r<2>(b),
				q_traits<B>::template r<3>(b)
				};
			int i;
			for( i=0; i!=4; ++i )
				if( !f(q1[i],q2[i]) )
					break;
			if( i==4 )
				return true;
			for( i=0; i!=4; ++i )
				if( !f(q1[i],-q2[i]) )
					return false;
			return true;
			}

		////////////////////////////////////////////////

		template <class R,class A>
		BOOST_QVM_INLINE_TRIVIAL
		typename enable_if_c<
			is_q<R>::value && is_q<A>::value,
			R>::type
		make( A const & a )
			{
			R r;
			q_traits<R>::template w<0>(r) = q_traits<A>::template r<0>(a);
			q_traits<R>::template w<1>(r) = q_traits<A>::template r<1>(a);
			q_traits<R>::template w<2>(r) = q_traits<A>::template r<2>(a);
			q_traits<R>::template w<3>(r) = q_traits<A>::template r<3>(a);
			return r;
			}

		template <class R,class A>
		BOOST_QVM_INLINE_OPERATIONS
		typename enable_if_c<
			is_q<R>::value && is_m<A>::value &&
			m_traits<A>::rows==3 && m_traits<A>::cols==3,
			R>::type
		make( A const & a )
			{
			typedef typename m_traits<A>::scalar_type T;
			T const mat[3][3] =
				{
					{ m_traits<A>::template r<0,0>(a), m_traits<A>::template r<0,1>(a), m_traits<A>::template r<0,2>(a) },
					{ m_traits<A>::template r<1,0>(a), m_traits<A>::template r<1,1>(a), m_traits<A>::template r<1,2>(a) },
					{ m_traits<A>::template r<2,0>(a), m_traits<A>::template r<2,1>(a), m_traits<A>::template r<2,2>(a) }
				};
			R r;
			if( mat[0][0]+mat[1][1]+mat[2][2] > s_traits<T>::value(0) )
				{
				T t = mat[0][0] + mat[1][1] + mat[2][2] + s_traits<T>::value(1);
				T s = (s_traits<T>::value(1)/sqrt<T>(t))/2;
				q_traits<R>::template w<0>(r)=s*t;
				q_traits<R>::template w<1>(r)=(mat[2][1]-mat[1][2])*s;
				q_traits<R>::template w<2>(r)=(mat[0][2]-mat[2][0])*s;
				q_traits<R>::template w<3>(r)=(mat[1][0]-mat[0][1])*s;
				}
			else if( mat[0][0]>mat[1][1] && mat[0][0]>mat[2][2] )
				{
				T t = mat[0][0] - mat[1][1] - mat[2][2] + s_traits<T>::value(1);
				T s = (s_traits<T>::value(1)/sqrt<T>(t))/2;
				q_traits<R>::template w<0>(r)=(mat[2][1]-mat[1][2])*s;
				q_traits<R>::template w<1>(r)=s*t;
				q_traits<R>::template w<2>(r)=(mat[1][0]+mat[0][1])*s;
				q_traits<R>::template w<3>(r)=(mat[0][2]+mat[2][0])*s;
				}
			else if( mat[1][1]>mat[2][2] )
				{
				T t = - mat[0][0] + mat[1][1] - mat[2][2] + s_traits<T>::value(1);
				T s = (s_traits<T>::value(1)/sqrt<T>(t))/2;
				q_traits<R>::template w<0>(r)=(mat[0][2]-mat[2][0])*s;
				q_traits<R>::template w<1>(r)=(mat[1][0]+mat[0][1])*s;
				q_traits<R>::template w<2>(r)=s*t;
				q_traits<R>::template w<3>(r)=(mat[2][1]+mat[1][2])*s;
				}
			else
				{
				T t = - mat[0][0] - mat[1][1] + mat[2][2] + s_traits<T>::value(1);
				T s = (s_traits<T>::value(1)/sqrt<T>(t))/2;
				q_traits<R>::template w<0>(r)=(mat[1][0]-mat[0][1])*s;
				q_traits<R>::template w<1>(r)=(mat[0][2]+mat[2][0])*s;
				q_traits<R>::template w<2>(r)=(mat[2][1]+mat[1][2])*s;
				q_traits<R>::template w<3>(r)=s*t;
				}
			return r;
			}

		////////////////////////////////////////////////

		template <class A>
		BOOST_QVM_INLINE_OPERATIONS
		typename lazy_enable_if_c<
			is_q<A>::value,
			deduce_q<A> >::type
		conjugate( A const & a )
			{
			typedef typename deduce_q<A>::type R;
			R r;
			q_traits<R>::template w<0>(r)=q_traits<A>::template r<0>(a);
			q_traits<R>::template w<1>(r)=-q_traits<A>::template r<1>(a);
			q_traits<R>::template w<2>(r)=-q_traits<A>::template r<2>(a);
			q_traits<R>::template w<3>(r)=-q_traits<A>::template r<3>(a);
			return r;
			}

		////////////////////////////////////////////////

		namespace
		qvm_detail
			{
			template <class T>
			class
			identity_quat_
				{
				identity_quat_( identity_quat_ const & );
				identity_quat_ & operator=( identity_quat_ const & );
				~identity_quat_();

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

		template <class T>
		struct
		q_traits< qvm_detail::identity_quat_<T> >
			{
			typedef qvm_detail::identity_quat_<T> this_quaternion;
			typedef T scalar_type;

			template <int I>
			static
			BOOST_QVM_INLINE_CRITICAL
			scalar_type
			r( this_quaternion const & x )
				{
				BOOST_QVM_ASSERT(&x==0);
				BOOST_QVM_STATIC_ASSERT(I>=0);
				BOOST_QVM_STATIC_ASSERT(I<4);
				return s_traits<T>::value(I==0);
				}

			static
			BOOST_QVM_INLINE_CRITICAL
			scalar_type
			ir( int i, this_quaternion const & x )
				{
				BOOST_QVM_ASSERT(&x==0);
				BOOST_QVM_ASSERT(i>=0);
				BOOST_QVM_ASSERT(i<4);
				return s_traits<T>::value(i==0);
				}
			};

		template <class T>
		BOOST_QVM_INLINE_TRIVIAL
		qvm_detail::identity_quat_<T> const &
		identity_q()
			{
			return *(qvm_detail::identity_quat_<T> const *)0;
			}

		template <class A>
		BOOST_QVM_INLINE_OPERATIONS
		typename enable_if_c<
			is_q<A>::value,
			void>::type
		set_identity( A & a )
			{
			typedef typename q_traits<A>::scalar_type T;
			T const zero=s_traits<T>::value(0);
			T const one=s_traits<T>::value(1);
			q_traits<A>::template w<0>(a) = one;
			q_traits<A>::template w<1>(a) = zero;
			q_traits<A>::template w<2>(a) = zero;
			q_traits<A>::template w<3>(a) = zero;
			}

		////////////////////////////////////////////////

		namespace
		qvm_detail
			{
			template <class OriginalType,class Scalar>
			class
			quaternion_scalar_cast_
				{
				quaternion_scalar_cast_( quaternion_scalar_cast_ const & );
				quaternion_scalar_cast_ & operator=( quaternion_scalar_cast_ const & );
				~quaternion_scalar_cast_();

				public:

				template <class T>
				BOOST_QVM_INLINE_TRIVIAL
				quaternion_scalar_cast_ &
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

			template <bool> struct scalar_cast_quaternion_filter { };
			template <> struct scalar_cast_quaternion_filter<true> { typedef int type; };
			}

		template <class OriginalType,class Scalar>
		struct
		q_traits< qvm_detail::quaternion_scalar_cast_<OriginalType,Scalar> >
			{
			typedef Scalar scalar_type;
			typedef qvm_detail::quaternion_scalar_cast_<OriginalType,Scalar> this_quaternion;

			template <int I>
			static
			BOOST_QVM_INLINE_CRITICAL
			scalar_type
			r( this_quaternion const & x )
				{
				BOOST_QVM_STATIC_ASSERT(I>=0);
				BOOST_QVM_STATIC_ASSERT(I<4);
				return scalar_type(q_traits<OriginalType>::template r<I>(reinterpret_cast<OriginalType const &>(x)));
				}

			static
			BOOST_QVM_INLINE_CRITICAL
			scalar_type
			ir( int i, this_quaternion const & x )
				{
				BOOST_QVM_ASSERT(i>=0);
				BOOST_QVM_ASSERT(i<4);
				return scalar_type(q_traits<OriginalType>::ir(i,reinterpret_cast<OriginalType const &>(x)));
				}
			};

		template <class Scalar,class T>
		BOOST_QVM_INLINE_TRIVIAL
		qvm_detail::quaternion_scalar_cast_<T,Scalar> const &
		scalar_cast( T const & x, typename qvm_detail::scalar_cast_quaternion_filter<is_q<T>::value>::type=0 )
			{
			return reinterpret_cast<qvm_detail::quaternion_scalar_cast_<T,Scalar> const &>(x);
			}

		////////////////////////////////////////////////

		template <class A>
		BOOST_QVM_INLINE_OPERATIONS
		typename enable_if_c<
			is_q<A>::value,
			A &>::type
		operator/=( A & a, typename q_traits<A>::scalar_type b )
			{
			q_traits<A>::template w<0>(a)/=b;
			q_traits<A>::template w<1>(a)/=b;
			q_traits<A>::template w<2>(a)/=b;
			q_traits<A>::template w<3>(a)/=b;
			return a;
			}

		template <class A>
		BOOST_QVM_INLINE_OPERATIONS
		typename lazy_enable_if_c<
			is_q<A>::value,
			deduce_q<A> >::type
		operator/( A const & a, typename q_traits<A>::scalar_type b )
			{
			typedef typename deduce_q<A>::type R;
			R r;
			q_traits<R>::template w<0>(r) = q_traits<A>::template r<0>(a)/b;
			q_traits<R>::template w<1>(r) = q_traits<A>::template r<1>(a)/b;
			q_traits<R>::template w<2>(r) = q_traits<A>::template r<2>(a)/b;
			q_traits<R>::template w<3>(r) = q_traits<A>::template r<3>(a)/b;
			return r;
			}

		template <class A,class B>
		BOOST_QVM_INLINE_OPERATIONS
		typename lazy_enable_if_c<
			is_q<A>::value && is_q<B>::value,
			deduce_s<typename q_traits<A>::scalar_type,typename q_traits<B>::scalar_type> >::type
		dot( A const & a, B const & b )
			{
			typedef typename q_traits<A>::scalar_type Ta;
			typedef typename q_traits<B>::scalar_type Tb;
			typedef typename deduce_s<Ta,Tb>::type Tr;
			Ta const a0=q_traits<A>::template r<0>(a);
			Ta const a1=q_traits<A>::template r<1>(a);
			Ta const a2=q_traits<A>::template r<2>(a);
			Ta const a3=q_traits<A>::template r<3>(a);
			Tb const b0=q_traits<B>::template r<0>(b);
			Tb const b1=q_traits<B>::template r<1>(b);
			Tb const b2=q_traits<B>::template r<2>(b);
			Tb const b3=q_traits<B>::template r<3>(b);
			Tr const dp=a0*b0+a1*b1+a2*b2+a3*b3;
			return dp;
			}

		template <class A,class B>
		BOOST_QVM_INLINE_OPERATIONS
		typename enable_if_c<
			is_q<A>::value && is_q<B>::value,
			bool>::type
		operator==( A const & a, B const & b )
			{
			return
				q_traits<A>::template r<0>(a)==q_traits<B>::template r<0>(b) &&
				q_traits<A>::template r<1>(a)==q_traits<B>::template r<1>(b) &&
				q_traits<A>::template r<2>(a)==q_traits<B>::template r<2>(b) &&
				q_traits<A>::template r<3>(a)==q_traits<B>::template r<3>(b);
			}

		template <class A>
		BOOST_QVM_INLINE_OPERATIONS
		typename lazy_enable_if_c<
			is_q<A>::value,
			deduce_q<A> >::type
		inverse( A const & a )
			{
			typedef typename deduce_q<A>::type R;
			typedef typename q_traits<A>::scalar_type TA;
			TA aa = q_traits<A>::template r<0>(a);
			TA ab = q_traits<A>::template r<1>(a);
			TA ac = q_traits<A>::template r<2>(a);
			TA ad = q_traits<A>::template r<3>(a);
			TA mag2 = ab*ab + ac*ac + ad*ad + aa*aa;
			if( mag2==s_traits<TA>::value(0) )
				BOOST_THROW_EXCEPTION(zero_magnitude_error());
			TA rm=s_traits<TA>::value(1)/mag2;
			R r;
			q_traits<R>::template w<0>(r) = aa*rm;
			q_traits<R>::template w<1>(r) = -ab*rm;
			q_traits<R>::template w<2>(r) = -ac*rm;
			q_traits<R>::template w<3>(r) = -ad*rm;
			return r;
			}

		template <class A>
		BOOST_QVM_INLINE_OPERATIONS
		typename enable_if_c<
			is_q<A>::value,
			typename q_traits<A>::scalar_type>::type
		mag2( A const & a )
			{
			typedef typename q_traits<A>::scalar_type T;
			T x=q_traits<A>::template r<0>(a);
			T y=q_traits<A>::template r<1>(a);
			T z=q_traits<A>::template r<2>(a);
			T w=q_traits<A>::template r<3>(a);
			return x*x+y*y+z*z+w*w;
			}

		template <class A>
		BOOST_QVM_INLINE_OPERATIONS
		typename enable_if_c<
			is_q<A>::value,
			typename q_traits<A>::scalar_type>::type
		mag( A const & a )
			{
			typedef typename q_traits<A>::scalar_type T;
			T x=q_traits<A>::template r<0>(a);
			T y=q_traits<A>::template r<1>(a);
			T z=q_traits<A>::template r<2>(a);
			T w=q_traits<A>::template r<3>(a);
			return sqrt<T>(x*x+y*y+z*z+w*w);
			}

		template <class A,class B>
		BOOST_QVM_INLINE_OPERATIONS
		typename enable_if_c<
			is_q<A>::value && is_q<B>::value,
			A &>::type
		operator-=( A & a, B const & b )
			{
			q_traits<A>::template w<0>(a)-=q_traits<B>::template r<0>(b);
			q_traits<A>::template w<1>(a)-=q_traits<B>::template r<1>(b);
			q_traits<A>::template w<2>(a)-=q_traits<B>::template r<2>(b);
			q_traits<A>::template w<3>(a)-=q_traits<B>::template r<3>(b);
			return a;
			}

		template <class A,class B>
		BOOST_QVM_INLINE_OPERATIONS
		typename lazy_enable_if_c<
			is_q<A>::value && is_q<B>::value,
			deduce_q2<A,B> >::type
		operator-( A const & a, B const & b )
			{
			typedef typename deduce_q2<A,B>::type R;
			R r;
			q_traits<R>::template w<0>(r)=q_traits<A>::template r<0>(a)-q_traits<B>::template r<0>(b);
			q_traits<R>::template w<1>(r)=q_traits<A>::template r<1>(a)-q_traits<B>::template r<1>(b);
			q_traits<R>::template w<2>(r)=q_traits<A>::template r<2>(a)-q_traits<B>::template r<2>(b);
			q_traits<R>::template w<3>(r)=q_traits<A>::template r<3>(a)-q_traits<B>::template r<3>(b);
			return r;
			}

		template <class A>
		BOOST_QVM_INLINE_OPERATIONS
		typename lazy_enable_if_c<
			is_q<A>::value,
			deduce_q<A> >::type
		operator-( A const & a )
			{
			typedef typename deduce_q<A>::type R;
			R r;
			q_traits<R>::template w<0>(r)=-q_traits<A>::template r<0>(a);
			q_traits<R>::template w<1>(r)=-q_traits<A>::template r<1>(a);
			q_traits<R>::template w<2>(r)=-q_traits<A>::template r<2>(a);
			q_traits<R>::template w<3>(r)=-q_traits<A>::template r<3>(a);
			return r;
			}

		template <class A,class B>
		BOOST_QVM_INLINE_OPERATIONS
		typename enable_if_c<
			is_q<A>::value && is_q<B>::value,
			A &>::type
		operator*=( A & a, B const & b )
			{
			typedef typename q_traits<A>::scalar_type TA;
			typedef typename q_traits<B>::scalar_type TB;
			TA const aa=q_traits<A>::template r<0>(a);
			TA const ab=q_traits<A>::template r<1>(a);
			TA const ac=q_traits<A>::template r<2>(a);
			TA const ad=q_traits<A>::template r<3>(a);
			TB const ba=q_traits<B>::template r<0>(b);
			TB const bb=q_traits<B>::template r<1>(b);
			TB const bc=q_traits<B>::template r<2>(b);
			TB const bd=q_traits<B>::template r<3>(b);
			q_traits<A>::template w<0>(a) = aa*ba - ab*bb - ac*bc - ad*bd;
			q_traits<A>::template w<1>(a) = aa*bb + ab*ba + ac*bd - ad*bc;
			q_traits<A>::template w<2>(a) = aa*bc + ac*ba + ad*bb - ab*bd;
			q_traits<A>::template w<3>(a) = aa*bd + ad*ba + ab*bc - ac*bb;
			return a;
			}

		template <class A>
		BOOST_QVM_INLINE_OPERATIONS
		typename enable_if_c<
			is_q<A>::value,
			A &>::type
		operator*=( A & a, typename q_traits<A>::scalar_type b )
			{
			q_traits<A>::template w<0>(a)*=b;
			q_traits<A>::template w<1>(a)*=b;
			q_traits<A>::template w<2>(a)*=b;
			q_traits<A>::template w<3>(a)*=b;
			return a;
			}

		template <class A,class B>
		BOOST_QVM_INLINE_OPERATIONS
		typename lazy_enable_if_c<
			is_q<A>::value && is_q<B>::value,
			deduce_q2<A,B> >::type
		operator*( A const & a, B const & b )
			{
			typedef typename deduce_q2<A,B>::type R;
			typedef typename q_traits<A>::scalar_type TA;
			typedef typename q_traits<B>::scalar_type TB;
			TA const aa=q_traits<A>::template r<0>(a);
			TA const ab=q_traits<A>::template r<1>(a);
			TA const ac=q_traits<A>::template r<2>(a);
			TA const ad=q_traits<A>::template r<3>(a);
			TB const ba=q_traits<B>::template r<0>(b);
			TB const bb=q_traits<B>::template r<1>(b);
			TB const bc=q_traits<B>::template r<2>(b);
			TB const bd=q_traits<B>::template r<3>(b);
			R r;
			q_traits<R>::template w<0>(r) = aa*ba - ab*bb - ac*bc - ad*bd;
			q_traits<R>::template w<1>(r) = aa*bb + ab*ba + ac*bd - ad*bc;
			q_traits<R>::template w<2>(r) = aa*bc + ac*ba + ad*bb - ab*bd;
			q_traits<R>::template w<3>(r) = aa*bd + ad*ba + ab*bc - ac*bb;
			return r;
			}

		template <class A>
		BOOST_QVM_INLINE_OPERATIONS
		typename lazy_enable_if_c<
			is_q<A>::value,
			deduce_q<A> >::type
		operator*( A const & a, typename q_traits<A>::scalar_type b )
			{
			typedef typename deduce_q<A>::type R;
			R r;
			q_traits<R>::template w<0>(r)=q_traits<A>::template r<0>(a)*b;
			q_traits<R>::template w<1>(r)=q_traits<A>::template r<1>(a)*b;
			q_traits<R>::template w<2>(r)=q_traits<A>::template r<2>(a)*b;
			q_traits<R>::template w<3>(r)=q_traits<A>::template r<3>(a)*b;
			return r;
			}

		template <class A,class B>
		BOOST_QVM_INLINE_OPERATIONS
		typename enable_if_c<
			is_q<A>::value && is_q<B>::value,
			bool>::type
		operator!=( A const & a, B const & b )
			{
			return
				q_traits<A>::template r<0>(a)!=q_traits<B>::template r<0>(b) ||
				q_traits<A>::template r<1>(a)!=q_traits<B>::template r<1>(b) ||
				q_traits<A>::template r<2>(a)!=q_traits<B>::template r<2>(b) ||
				q_traits<A>::template r<3>(a)!=q_traits<B>::template r<3>(b);
			}

		template <class A>
		BOOST_QVM_INLINE_OPERATIONS
		typename lazy_enable_if_c<
			is_q<A>::value,
			deduce_q<A> >::type
		normalized( A const & a )
			{
			typedef typename q_traits<A>::scalar_type T;
			T const a0=q_traits<A>::template r<0>(a);
			T const a1=q_traits<A>::template r<1>(a);
			T const a2=q_traits<A>::template r<2>(a);
			T const a3=q_traits<A>::template r<3>(a);
			T const mag2=a0*a0+a1*a1+a2*a2+a3*a3;
			if( mag2==s_traits<typename q_traits<A>::scalar_type>::value(0) )
				BOOST_THROW_EXCEPTION(zero_magnitude_error());
			T const rm=s_traits<T>::value(1)/sqrt<T>(mag2);
			typedef typename deduce_q<A>::type R;
			R r;
			q_traits<R>::template w<0>(r)=a0*rm;
			q_traits<R>::template w<1>(r)=a1*rm;
			q_traits<R>::template w<2>(r)=a2*rm;
			q_traits<R>::template w<3>(r)=a3*rm;
			return r;
			}

		template <class A>
		BOOST_QVM_INLINE_OPERATIONS
		typename enable_if_c<
			is_q<A>::value,
			void>::type
		normalize( A & a )
			{
			typedef typename q_traits<A>::scalar_type T;
			T const a0=q_traits<A>::template r<0>(a);
			T const a1=q_traits<A>::template r<1>(a);
			T const a2=q_traits<A>::template r<2>(a);
			T const a3=q_traits<A>::template r<3>(a);
			T const mag2=a0*a0+a1*a1+a2*a2+a3*a3;
			if( mag2==s_traits<typename q_traits<A>::scalar_type>::value(0) )
				BOOST_THROW_EXCEPTION(zero_magnitude_error());
			T const rm=s_traits<T>::value(1)/sqrt<T>(mag2);
			q_traits<A>::template w<0>(a)*=rm;
			q_traits<A>::template w<1>(a)*=rm;
			q_traits<A>::template w<2>(a)*=rm;
			q_traits<A>::template w<3>(a)*=rm;
			}

		template <class A,class B>
		BOOST_QVM_INLINE_OPERATIONS
		typename enable_if_c<
			is_q<A>::value && is_q<B>::value,
			A &>::type
		operator+=( A & a, B const & b )
			{
			q_traits<A>::template w<0>(a)+=q_traits<B>::template r<0>(b);
			q_traits<A>::template w<1>(a)+=q_traits<B>::template r<1>(b);
			q_traits<A>::template w<2>(a)+=q_traits<B>::template r<2>(b);
			q_traits<A>::template w<3>(a)+=q_traits<B>::template r<3>(b);
			return a;
			}

		template <class A,class B>
		BOOST_QVM_INLINE_OPERATIONS
		typename lazy_enable_if_c<
			is_q<A>::value && is_q<B>::value,
			deduce_q2<A,B> >::type
		operator+( A const & a, B const & b )
			{
			typedef typename deduce_q2<A,B>::type R;
			R r;
			q_traits<R>::template w<0>(r)=q_traits<A>::template r<0>(a)+q_traits<B>::template r<0>(b);
			q_traits<R>::template w<1>(r)=q_traits<A>::template r<1>(a)+q_traits<B>::template r<1>(b);
			q_traits<R>::template w<2>(r)=q_traits<A>::template r<2>(a)+q_traits<B>::template r<2>(b);
			q_traits<R>::template w<3>(r)=q_traits<A>::template r<3>(a)+q_traits<B>::template r<3>(b);
			return r;
			}

		template <class A,class B>
		BOOST_QVM_INLINE_OPERATIONS
		typename lazy_enable_if_c<
			is_q<A>::value && is_q<B>::value,
			deduce_q2<A,B> >::type
		slerp( A const & a, B const & b, typename q_traits<typename deduce_q2<A,B>::type>::scalar_type t )
			{
			typedef typename deduce_q2<A,B>::type R;
			typedef typename q_traits<R>::scalar_type TR;
			typedef typename q_traits<A>::scalar_type TA;
			typedef typename q_traits<B>::scalar_type TB;
			TR const one=s_traits<TR>::value(1);
			TR dp=dot(a,b);
			if( dp>one )
				dp=one;
			R const n=b-a*dp;
			TR const nm=mag(n);
			if( nm>s_traits<TR>::value(0) )
				{
				TR const theta_0 = qvm::acos<TR>(dp);
				TR const theta = theta_0*t;
				return a*qvm::cos<TR>(theta) + (n/nm)*qvm::sin<TR>(theta);
				}
			else
				return normalized(a+(b-a)*t);
			}

		////////////////////////////////////////////////

		namespace
		qvm_detail
			{
			template <class T>
			class
			qref_
				{
				qref_( qref_ const & );
				qref_ & operator=( qref_ const & );
				~qref_();

				public:

				template <class R>
				BOOST_QVM_INLINE_TRIVIAL
				qref_ &
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

		template <class Q>
		struct q_traits;

		template <class Q>
		struct
		q_traits< qvm_detail::qref_<Q> >
			{
			typedef typename q_traits<Q>::scalar_type scalar_type;
			typedef qvm_detail::qref_<Q> this_quaternion;

			template <int I>
			static
			BOOST_QVM_INLINE_CRITICAL
			scalar_type
			r( this_quaternion const & x )
				{
				BOOST_QVM_STATIC_ASSERT(I>=0);
				BOOST_QVM_STATIC_ASSERT(I<4);
				return q_traits<Q>::template r<I>(reinterpret_cast<Q const &>(x));
				}

			template <int I>
			static
			BOOST_QVM_INLINE_CRITICAL
			scalar_type &
			w( this_quaternion & x )
				{
				BOOST_QVM_STATIC_ASSERT(I>=0);
				BOOST_QVM_STATIC_ASSERT(I<4);
				return q_traits<Q>::template w<I>(reinterpret_cast<Q &>(x));
				}
			};

		template <class Q>
		struct
		deduce_q< qvm_detail::qref_<Q> >
			{
			typedef quat<typename q_traits<Q>::scalar_type> type;
			};

		template <class Q>
		BOOST_QVM_INLINE_TRIVIAL
		typename enable_if_c<
			is_q<Q>::value,
			qvm_detail::qref_<Q> const &>::type
		qref( Q const & a )
			{
			return reinterpret_cast<qvm_detail::qref_<Q> const &>(a);
			}

		template <class Q>
		BOOST_QVM_INLINE_TRIVIAL
		typename enable_if_c<
			is_q<Q>::value,
			qvm_detail::qref_<Q> &>::type
		qref( Q & a )
			{
			return reinterpret_cast<qvm_detail::qref_<Q> &>(a);
			}

		////////////////////////////////////////////////

		namespace
		qvm_detail
			{
			template <class T>
			class
			zero_quat_
				{
				zero_quat_( zero_quat_ const & );
				zero_quat_ & operator=( zero_quat_ const & );
				~zero_quat_();

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

		template <class T>
		struct
		q_traits< qvm_detail::zero_quat_<T> >
			{
			typedef qvm_detail::zero_quat_<T> this_quaternion;
			typedef T scalar_type;

			template <int I>
			static
			BOOST_QVM_INLINE_CRITICAL
			scalar_type
			r( this_quaternion const & x )
				{
				BOOST_QVM_ASSERT(&x==0);
				BOOST_QVM_STATIC_ASSERT(I>=0);
				BOOST_QVM_STATIC_ASSERT(I<4);
				return s_traits<scalar_type>::value(0);
				}

			static
			BOOST_QVM_INLINE_CRITICAL
			scalar_type
			ir( int i, this_quaternion const & x )
				{
				BOOST_QVM_ASSERT(&x==0);
				BOOST_QVM_ASSERT(i>=0);
				BOOST_QVM_ASSERT(i<4);
				return s_traits<scalar_type>::value(0);
				}
			};

		template <class T>
		BOOST_QVM_INLINE_TRIVIAL
		qvm_detail::zero_quat_<T> const &
		zero_q()
			{
			return *(qvm_detail::zero_quat_<T> const *)0;
			}

		template <class A>
		BOOST_QVM_INLINE_OPERATIONS
		typename enable_if_c<
			is_q<A>::value,
			void>::type
		set_zero( A & a )
			{
			typedef typename q_traits<A>::scalar_type T;
			T const zero=s_traits<T>::value(0);
			q_traits<A>::template w<0>(a) = zero;
			q_traits<A>::template w<1>(a) = zero;
			q_traits<A>::template w<2>(a) = zero;
			q_traits<A>::template w<3>(a) = zero;
			}

		////////////////////////////////////////////////

		namespace
		qvm_detail
			{
			template <class V>
			struct
			rot_quat_
				{
				typedef typename v_traits<V>::scalar_type scalar_type;
				scalar_type a[4];

				BOOST_QVM_INLINE
				rot_quat_( V const & axis, typename v_traits<V>::scalar_type angle )
					{
					scalar_type const x=v_traits<V>::template r<0>(axis);
					scalar_type const y=v_traits<V>::template r<1>(axis);
					scalar_type const z=v_traits<V>::template r<2>(axis);
					scalar_type const m2=x*x+y*y+z*z;
					if( m2==s_traits<scalar_type>::value(0) )
						BOOST_THROW_EXCEPTION(zero_magnitude_error());
					scalar_type const rm=s_traits<scalar_type>::value(1)/sqrt<scalar_type>(m2);
					angle/=2;
					scalar_type const s=sin<scalar_type>(angle);
					a[0] = cos<scalar_type>(angle);
					a[1] = rm*x*s;
					a[2] = rm*y*s;
					a[3] = rm*z*s;
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
		q_traits< qvm_detail::rot_quat_<V> >
			{
			typedef qvm_detail::rot_quat_<V> this_quaternion;
			typedef typename this_quaternion::scalar_type scalar_type;

			template <int I>
			static
			BOOST_QVM_INLINE_CRITICAL
			scalar_type
			r( this_quaternion const & x )
				{
				BOOST_QVM_STATIC_ASSERT(I>=0);
				BOOST_QVM_STATIC_ASSERT(I<4);
				return x.a[I];
				}
			};

		template <class A>
		BOOST_QVM_INLINE
		typename enable_if_c<
			is_v<A>::value && v_traits<A>::dim==3,
			qvm_detail::rot_quat_<A> >::type
		rot_quat( A const & axis, typename v_traits<A>::scalar_type angle )
			{
			return qvm_detail::rot_quat_<A>(axis,angle);
			}

		template <class A,class B>
		BOOST_QVM_INLINE_OPERATIONS
		typename enable_if_c<
			is_q<A>::value &&
			is_v<B>::value && v_traits<B>::dim==3,
			void>::type
		set_rot( A & a, B const & axis, typename v_traits<B>::scalar_type angle )
			{
			assign(a,rot_quat(axis,angle));
			}

		template <class A,class B>
		BOOST_QVM_INLINE_OPERATIONS
		typename enable_if_c<
			is_q<A>::value &&
			is_v<B>::value && v_traits<B>::dim==3,
			void>::type
		rotate( A & a, B const & axis, typename v_traits<B>::scalar_type angle )
			{
			a *= rot_quat(axis,angle);
			}

		////////////////////////////////////////////////

		namespace
		qvm_detail
			{
			template <class T>
			struct
			rotx_quat_
				{
				BOOST_QVM_INLINE_TRIVIAL
				rotx_quat_()
					{
					}

				template <class R>
				BOOST_QVM_INLINE_TRIVIAL
				operator R() const
					{
					R r;
					assign(r,*this);
					return r;
					}

				private:

				rotx_quat_( rotx_quat_ const & );
				rotx_quat_ & operator=( rotx_quat_ const & );
				~rotx_quat_();
				};

			template <int I>
			struct
			rotx_quat_get
				{
				template <class T>
				static
				BOOST_QVM_INLINE_CRITICAL
				T
				get( T const & )
					{
					return s_traits<T>::value(0);
					};
				};

			template <>
			struct
			rotx_quat_get<1>
				{
				template <class T>
				static
				BOOST_QVM_INLINE_CRITICAL
				T
				get( T const & angle )
					{
					return sin<T>(angle/2);
					};
				};

			template <>
			struct
			rotx_quat_get<0>
				{
				template <class T>
				static
				BOOST_QVM_INLINE_CRITICAL
				T
				get( T const & angle )
					{
					return cos<T>(angle/2);
					};
				};
			}

		template <class Angle>
		struct
		q_traits< qvm_detail::rotx_quat_<Angle> >
			{
			typedef qvm_detail::rotx_quat_<Angle> this_quaternion;
			typedef Angle scalar_type;

			template <int I>
			static
			BOOST_QVM_INLINE_CRITICAL
			scalar_type
			r( this_quaternion const & x )
				{
				BOOST_QVM_STATIC_ASSERT(I>=0);
				BOOST_QVM_STATIC_ASSERT(I<4);
				return qvm_detail::rotx_quat_get<I>::get(reinterpret_cast<Angle const &>(x));
				}
			};

		template <class Angle>
		struct
		deduce_q< qvm_detail::rotx_quat_<Angle> >
			{
			typedef quat<Angle> type;
			};

		template <class Angle>
		struct
		deduce_q2< qvm_detail::rotx_quat_<Angle>, qvm_detail::rotx_quat_<Angle> >
			{
			typedef quat<Angle> type;
			};

		template <class Angle>
		BOOST_QVM_INLINE_TRIVIAL
		qvm_detail::rotx_quat_<Angle> const &
		rotx_quat( Angle const & angle )
			{
			return reinterpret_cast<qvm_detail::rotx_quat_<Angle> const &>(angle);
			}

		template <class A>
		BOOST_QVM_INLINE_OPERATIONS
		typename enable_if_c<
			is_q<A>::value,
			void>::type
		set_rotx( A & a, typename q_traits<A>::scalar_type angle )
			{
			assign(a,rotx_quat(angle));
			}

		template <class A>
		BOOST_QVM_INLINE_OPERATIONS
		typename enable_if_c<
			is_q<A>::value,
			void>::type
		rotate_x( A & a, typename q_traits<A>::scalar_type angle )
			{
			a *= rotx_quat(angle);
			}

		////////////////////////////////////////////////

		namespace
		qvm_detail
			{
			template <class T>
			struct
			roty_quat_
				{
				BOOST_QVM_INLINE_TRIVIAL
				roty_quat_()
					{
					}

				template <class R>
				BOOST_QVM_INLINE_TRIVIAL
				operator R() const
					{
					R r;
					assign(r,*this);
					return r;
					}

				private:

				roty_quat_( roty_quat_ const & );
				roty_quat_ & operator=( roty_quat_ const & );
				~roty_quat_();
				};

			template <int I>
			struct
			roty_quat_get
				{
				template <class T>
				static
				BOOST_QVM_INLINE_CRITICAL
				T
				get( T const & )
					{
					return s_traits<T>::value(0);
					};
				};

			template <>
			struct
			roty_quat_get<2>
				{
				template <class T>
				static
				BOOST_QVM_INLINE_CRITICAL
				T
				get( T const & angle )
					{
					return sin<T>(angle/2);
					};
				};

			template <>
			struct
			roty_quat_get<0>
				{
				template <class T>
				static
				BOOST_QVM_INLINE_CRITICAL
				T
				get( T const & angle )
					{
					return cos<T>(angle/2);
					};
				};
			}

		template <class Angle>
		struct
		q_traits< qvm_detail::roty_quat_<Angle> >
			{
			typedef qvm_detail::roty_quat_<Angle> this_quaternion;
			typedef Angle scalar_type;

			template <int I>
			static
			BOOST_QVM_INLINE_CRITICAL
			scalar_type
			r( this_quaternion const & x )
				{
				BOOST_QVM_STATIC_ASSERT(I>=0);
				BOOST_QVM_STATIC_ASSERT(I<4);
				return qvm_detail::roty_quat_get<I>::get(reinterpret_cast<Angle const &>(x));
				}
			};

		template <class Angle>
		struct
		deduce_q< qvm_detail::roty_quat_<Angle> >
			{
			typedef quat<Angle> type;
			};

		template <class Angle>
		struct
		deduce_q2< qvm_detail::roty_quat_<Angle>, qvm_detail::roty_quat_<Angle> >
			{
			typedef quat<Angle> type;
			};

		template <class Angle>
		BOOST_QVM_INLINE_TRIVIAL
		qvm_detail::roty_quat_<Angle> const &
		roty_quat( Angle const & angle )
			{
			return reinterpret_cast<qvm_detail::roty_quat_<Angle> const &>(angle);
			}

		template <class A>
		BOOST_QVM_INLINE_OPERATIONS
		typename enable_if_c<
			is_q<A>::value,
			void>::type
		set_roty( A & a, typename q_traits<A>::scalar_type angle )
			{
			assign(a,roty_quat(angle));
			}

		template <class A>
		BOOST_QVM_INLINE_OPERATIONS
		typename enable_if_c<
			is_q<A>::value,
			void>::type
		rotate_y( A & a, typename q_traits<A>::scalar_type angle )
			{
			a *= roty_quat(angle);
			}

		////////////////////////////////////////////////

		namespace
		qvm_detail
			{
			template <class T>
			struct
			rotz_quat_
				{
				BOOST_QVM_INLINE_TRIVIAL
				rotz_quat_()
					{
					}

				template <class R>
				BOOST_QVM_INLINE_TRIVIAL
				operator R() const
					{
					R r;
					assign(r,*this);
					return r;
					}

				private:

				rotz_quat_( rotz_quat_ const & );
				rotz_quat_ & operator=( rotz_quat_ const & );
				~rotz_quat_();
				};

			template <int I>
			struct
			rotz_quat_get
				{
				template <class T>
				static
				BOOST_QVM_INLINE_CRITICAL
				T
				get( T const & )
					{
					return s_traits<T>::value(0);
					};
				};

			template <>
			struct
			rotz_quat_get<3>
				{
				template <class T>
				static
				BOOST_QVM_INLINE_CRITICAL
				T
				get( T const & angle )
					{
					return sin<T>(angle/2);
					};
				};

			template <>
			struct
			rotz_quat_get<0>
				{
				template <class T>
				static
				BOOST_QVM_INLINE_CRITICAL
				T
				get( T const & angle )
					{
					return cos<T>(angle/2);
					};
				};
			}

		template <class Angle>
		struct
		q_traits< qvm_detail::rotz_quat_<Angle> >
			{
			typedef qvm_detail::rotz_quat_<Angle> this_quaternion;
			typedef Angle scalar_type;

			template <int I>
			static
			BOOST_QVM_INLINE_CRITICAL
			scalar_type
			r( this_quaternion const & x )
				{
				BOOST_QVM_STATIC_ASSERT(I>=0);
				BOOST_QVM_STATIC_ASSERT(I<4);
				return qvm_detail::rotz_quat_get<I>::get(reinterpret_cast<Angle const &>(x));
				}
			};

		template <class Angle>
		struct
		deduce_q< qvm_detail::rotz_quat_<Angle> >
			{
			typedef quat<Angle> type;
			};

		template <class Angle>
		struct
		deduce_q2< qvm_detail::rotz_quat_<Angle>, qvm_detail::rotz_quat_<Angle> >
			{
			typedef quat<Angle> type;
			};

		template <class Angle>
		BOOST_QVM_INLINE_TRIVIAL
		qvm_detail::rotz_quat_<Angle> const &
		rotz_quat( Angle const & angle )
			{
			return reinterpret_cast<qvm_detail::rotz_quat_<Angle> const &>(angle);
			}

		template <class A>
		BOOST_QVM_INLINE_OPERATIONS
		typename enable_if_c<
			is_q<A>::value,
			void>::type
		set_rotz( A & a, typename q_traits<A>::scalar_type angle )
			{
			assign(a,rotz_quat(angle));
			}

		template <class A>
		BOOST_QVM_INLINE_OPERATIONS
		typename enable_if_c<
			is_q<A>::value,
			void>::type
		rotate_z( A & a, typename q_traits<A>::scalar_type angle )
			{
			a *= rotz_quat(angle);
			}

		////////////////////////////////////////////////

		namespace
		sfinae
			{
			using ::boost::qvm::assign;
			using ::boost::qvm::cmp;
			using ::boost::qvm::make;
			using ::boost::qvm::conjugate;
			using ::boost::qvm::set_identity;
			using ::boost::qvm::set_zero;
			using ::boost::qvm::scalar_cast;
			using ::boost::qvm::operator/=;
			using ::boost::qvm::operator/;
			using ::boost::qvm::dot;
			using ::boost::qvm::operator==;
			using ::boost::qvm::inverse;
			using ::boost::qvm::mag2;
			using ::boost::qvm::mag;
			using ::boost::qvm::slerp;
			using ::boost::qvm::operator-=;
			using ::boost::qvm::operator-;
			using ::boost::qvm::operator*=;
			using ::boost::qvm::operator*;
			using ::boost::qvm::operator!=;
			using ::boost::qvm::normalized;
			using ::boost::qvm::normalize;
			using ::boost::qvm::operator+=;
			using ::boost::qvm::operator+;
			using ::boost::qvm::qref;
			using ::boost::qvm::rot_quat;
			using ::boost::qvm::set_rot;
			using ::boost::qvm::rotate;
			using ::boost::qvm::rotx_quat;
			using ::boost::qvm::set_rotx;
			using ::boost::qvm::rotate_x;
			using ::boost::qvm::roty_quat;
			using ::boost::qvm::set_roty;
			using ::boost::qvm::rotate_y;
			using ::boost::qvm::rotz_quat;
			using ::boost::qvm::set_rotz;
			using ::boost::qvm::rotate_z;
			}

		////////////////////////////////////////////////
		}
	}

#endif
