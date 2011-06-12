//Copyright (c) 2008-2009 Emil Dotchevski and Reverge Studios, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_5265FC7CA1C011DE9EBDFFA956D89593
#define UUID_5265FC7CA1C011DE9EBDFFA956D89593

#include <boost/qvm/inline.hpp>
#include <boost/qvm/m_traits.hpp>
#include <boost/qvm/deduce_v.hpp>
#include <boost/qvm/assert.hpp>
#include <boost/qvm/enable_if.hpp>

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
            template <int Col,class OriginalMatrix>
            class
            col_
                {
                col_( col_ const & );
                col_ & operator=( col_ const & );
                ~col_();

                public:

                template <class T>
                BOOST_QVM_INLINE_TRIVIAL
                col_ &
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
            }

        template <int Col,class OriginalMatrix>
        struct
        v_traits< qvm_detail::col_<Col,OriginalMatrix> >
            {
            typedef qvm_detail::col_<Col,OriginalMatrix> this_vector;
            typedef typename m_traits<OriginalMatrix>::scalar_type scalar_type;
            static int const dim=m_traits<OriginalMatrix>::rows;
            BOOST_QVM_STATIC_ASSERT(Col>=0);
            BOOST_QVM_STATIC_ASSERT(Col<m_traits<OriginalMatrix>::cols);

            template <int I>
            static
            BOOST_QVM_INLINE_CRITICAL
            scalar_type
            r( this_vector const & x )
                {
                BOOST_QVM_STATIC_ASSERT(I>=0);
                BOOST_QVM_STATIC_ASSERT(I<dim);
                return m_traits<OriginalMatrix>::template r<I,Col>(reinterpret_cast<OriginalMatrix const &>(x));
                }

            template <int I>
            static
            BOOST_QVM_INLINE_CRITICAL
            scalar_type &
            w( this_vector & x )
                {
                BOOST_QVM_STATIC_ASSERT(I>=0);
                BOOST_QVM_STATIC_ASSERT(I<dim);
                return m_traits<OriginalMatrix>::template w<I,Col>(reinterpret_cast<OriginalMatrix &>(x));
                }

            static
            BOOST_QVM_INLINE_CRITICAL
            scalar_type
            ir( int i, this_vector const & x )
                {
                BOOST_QVM_ASSERT(i>=0);
                BOOST_QVM_ASSERT(i<dim);
                return m_traits<OriginalMatrix>::ir(i,Col,reinterpret_cast<OriginalMatrix const &>(x));
                }

            static
            BOOST_QVM_INLINE_CRITICAL
            scalar_type &
            iw( int i, this_vector & x )
                {
                BOOST_QVM_ASSERT(i>=0);
                BOOST_QVM_ASSERT(i<dim);
                return m_traits<OriginalMatrix>::iw(i,Col,reinterpret_cast<OriginalMatrix &>(x));
                }
            };

        template <int Col,class OriginalMatrix,int D>
        struct
        deduce_v<qvm_detail::col_<Col,OriginalMatrix>,D>
            {
            typedef vec<typename m_traits<OriginalMatrix>::scalar_type,D> type;
            };

        template <int Col,class OriginalMatrix,int D>
        struct
        deduce_v2<qvm_detail::col_<Col,OriginalMatrix>,qvm_detail::col_<Col,OriginalMatrix>,D>
            {
            typedef vec<typename m_traits<OriginalMatrix>::scalar_type,D> type;
            };

        template <int Col,class A>
        typename boost::enable_if_c<
            is_m<A>::value,
            qvm_detail::col_<Col,A> const &>::type
        BOOST_QVM_INLINE_TRIVIAL
        col( A const & a )
            {
            return reinterpret_cast<typename qvm_detail::col_<Col,A> const &>(a);
            }

        template <int Col,class A>
        typename boost::enable_if_c<
            is_m<A>::value,
            qvm_detail::col_<Col,A> &>::type
        BOOST_QVM_INLINE_TRIVIAL
        col( A & a )
            {
            return reinterpret_cast<typename qvm_detail::col_<Col,A> &>(a);
            }

        ////////////////////////////////////////////////

        namespace
        qvm_detail
            {
            template <int Row,class OriginalMatrix>
            class
            row_
                {
                row_( row_ const & );
                row_ & operator=( row_ const & );
                ~row_();

                public:

                template <class T>
                BOOST_QVM_INLINE_TRIVIAL
                row_ &
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
            }

        template <int Row,class OriginalMatrix>
        struct
        v_traits< qvm_detail::row_<Row,OriginalMatrix> >
            {
            typedef qvm_detail::row_<Row,OriginalMatrix> this_vector;
            typedef typename m_traits<OriginalMatrix>::scalar_type scalar_type;
            static int const dim=m_traits<OriginalMatrix>::cols;
            BOOST_QVM_STATIC_ASSERT(Row>=0);
            BOOST_QVM_STATIC_ASSERT(Row<m_traits<OriginalMatrix>::rows);

            template <int I>
            static
            BOOST_QVM_INLINE_CRITICAL
            scalar_type
            r( this_vector const & x )
                {
                BOOST_QVM_STATIC_ASSERT(I>=0);
                BOOST_QVM_STATIC_ASSERT(I<dim);
                return m_traits<OriginalMatrix>::template r<Row,I>(reinterpret_cast<OriginalMatrix const &>(x));
                }

            template <int I>
            static
            BOOST_QVM_INLINE_CRITICAL
            scalar_type &
            w( this_vector & x )
                {
                BOOST_QVM_STATIC_ASSERT(I>=0);
                BOOST_QVM_STATIC_ASSERT(I<dim);
                return m_traits<OriginalMatrix>::template w<Row,I>(reinterpret_cast<OriginalMatrix &>(x));
                }

            static
            BOOST_QVM_INLINE_CRITICAL
            scalar_type
            ir( int i, this_vector const & x )
                {
                BOOST_QVM_ASSERT(i>=0);
                BOOST_QVM_ASSERT(i<dim);
                return m_traits<OriginalMatrix>::ir(Row,i,reinterpret_cast<OriginalMatrix const &>(x));
                }

            static
            BOOST_QVM_INLINE_CRITICAL
            scalar_type &
            iw( int i, this_vector & x )
                {
                BOOST_QVM_ASSERT(i>=0);
                BOOST_QVM_ASSERT(i<dim);
                return m_traits<OriginalMatrix>::iw(Row,i,reinterpret_cast<OriginalMatrix &>(x));
                }
            };

        template <int Row,class OriginalMatrix,int D>
        struct
        deduce_v<qvm_detail::row_<Row,OriginalMatrix>,D>
            {
            typedef vec<typename m_traits<OriginalMatrix>::scalar_type,D> type;
            };

        template <int Row,class OriginalMatrix,int D>
        struct
        deduce_v2<qvm_detail::row_<Row,OriginalMatrix>,qvm_detail::row_<Row,OriginalMatrix>,D>
            {
            typedef vec<typename m_traits<OriginalMatrix>::scalar_type,D> type;
            };

        template <int Row,class A>
        typename boost::enable_if_c<
            is_m<A>::value,
            qvm_detail::row_<Row,A> const &>::type
        BOOST_QVM_INLINE_TRIVIAL
        row( A const & a )
            {
            return reinterpret_cast<typename qvm_detail::row_<Row,A> const &>(a);
            }

        template <int Row,class A>
        typename boost::enable_if_c<
            is_m<A>::value,
            qvm_detail::row_<Row,A> &>::type
        BOOST_QVM_INLINE_TRIVIAL
        row( A & a )
            {
            return reinterpret_cast<typename qvm_detail::row_<Row,A> &>(a);
            }

        ////////////////////////////////////////////////

        namespace
        qvm_detail
            {
            template <class OriginalMatrix>
            class
            diag_
                {
                diag_( diag_ const & );
                diag_ & operator=( diag_ const & );
                ~diag_();

                public:

                template <class T>
                BOOST_QVM_INLINE_TRIVIAL
                diag_ &
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

            template <int X,int Y,bool Which>
            struct diag_bool_dispatch;

            template <int X,int Y>
            struct
            diag_bool_dispatch<X,Y,true>
                {
                static int const value=X;
                };

            template <int X,int Y>
            struct
            diag_bool_dispatch<X,Y,false>
                {
                static int const value=Y;
                };
            }

        template <class OriginalMatrix>
        struct
        v_traits< qvm_detail::diag_<OriginalMatrix> >
            {
            typedef qvm_detail::diag_<OriginalMatrix> this_vector;
            typedef typename m_traits<OriginalMatrix>::scalar_type scalar_type;
            static int const dim=qvm_detail::diag_bool_dispatch<
                    m_traits<OriginalMatrix>::rows,
                    m_traits<OriginalMatrix>::cols,
                    m_traits<OriginalMatrix>::rows<=m_traits<OriginalMatrix>::cols>::value;

            template <int I>
            static
            BOOST_QVM_INLINE_CRITICAL
            scalar_type
            r( this_vector const & x )
                {
                BOOST_QVM_STATIC_ASSERT(I>=0);
                BOOST_QVM_STATIC_ASSERT(I<dim);
                return m_traits<OriginalMatrix>::template r<I,I>(reinterpret_cast<OriginalMatrix const &>(x));
                }

            template <int I>
            static
            BOOST_QVM_INLINE_CRITICAL
            scalar_type &
            w( this_vector & x )
                {
                BOOST_QVM_STATIC_ASSERT(I>=0);
                BOOST_QVM_STATIC_ASSERT(I<dim);
                return m_traits<OriginalMatrix>::template w<I,I>(reinterpret_cast<OriginalMatrix &>(x));
                }

            static
            BOOST_QVM_INLINE_CRITICAL
            scalar_type
            ir( int i, this_vector const & x )
                {
                BOOST_QVM_ASSERT(i>=0);
                BOOST_QVM_ASSERT(i<dim);
                return m_traits<OriginalMatrix>::ir(i,i,reinterpret_cast<OriginalMatrix const &>(x));
                }

            static
            BOOST_QVM_INLINE_CRITICAL
            scalar_type &
            iw( int i, this_vector & x )
                {
                BOOST_QVM_ASSERT(i>=0);
                BOOST_QVM_ASSERT(i<dim);
                return m_traits<OriginalMatrix>::iw(i,i,reinterpret_cast<OriginalMatrix &>(x));
                }
            };

        template <class OriginalMatrix,int D>
        struct
        deduce_v<qvm_detail::diag_<OriginalMatrix>,D>
            {
            typedef vec<typename m_traits<OriginalMatrix>::scalar_type,D> type;
            };

        template <class OriginalMatrix,int D>
        struct
        deduce_v2<qvm_detail::diag_<OriginalMatrix>,qvm_detail::diag_<OriginalMatrix>,D>
            {
            typedef vec<typename m_traits<OriginalMatrix>::scalar_type,D> type;
            };

        template <class A>
        typename boost::enable_if_c<
            is_m<A>::value,
            qvm_detail::diag_<A> const &>::type
        BOOST_QVM_INLINE_TRIVIAL
        diag( A const & a )
            {
            return reinterpret_cast<typename qvm_detail::diag_<A> const &>(a);
            }

        template <class A>
        typename boost::enable_if_c<
            is_m<A>::value,
            qvm_detail::diag_<A> &>::type
        BOOST_QVM_INLINE_TRIVIAL
        diag( A & a )
            {
            return reinterpret_cast<typename qvm_detail::diag_<A> &>(a);
            }

        ////////////////////////////////////////////////

        namespace
        qvm_detail
            {
            template <class OriginalMatrix>
            class
            trans_
                {
                trans_( trans_ const & );
                trans_ & operator=( trans_ const & );
                ~trans_();

                public:

                template <class T>
                BOOST_QVM_INLINE_TRIVIAL
                trans_ &
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
            }

        template <class OriginalMatrix>
        struct
        v_traits< qvm_detail::trans_<OriginalMatrix> >
            {
            typedef qvm_detail::trans_<OriginalMatrix> this_vector;
            typedef typename m_traits<OriginalMatrix>::scalar_type scalar_type;
            static int const dim=m_traits<OriginalMatrix>::rows-1;
            BOOST_QVM_STATIC_ASSERT(m_traits<OriginalMatrix>::rows==m_traits<OriginalMatrix>::cols);
            BOOST_QVM_STATIC_ASSERT(m_traits<OriginalMatrix>::rows>=3);

            template <int I>
            static
            BOOST_QVM_INLINE_CRITICAL
            scalar_type
            r( this_vector const & x )
                {
                BOOST_QVM_STATIC_ASSERT(I>=0);
                BOOST_QVM_STATIC_ASSERT(I<dim);
                return m_traits<OriginalMatrix>::template r<I,dim>(reinterpret_cast<OriginalMatrix const &>(x));
                }

            template <int I>
            static
            BOOST_QVM_INLINE_CRITICAL
            scalar_type &
            w( this_vector & x )
                {
                BOOST_QVM_STATIC_ASSERT(I>=0);
                BOOST_QVM_STATIC_ASSERT(I<dim);
                return m_traits<OriginalMatrix>::template w<I,dim>(reinterpret_cast<OriginalMatrix &>(x));
                }

            static
            BOOST_QVM_INLINE_CRITICAL
            scalar_type
            ir( int i, this_vector const & x )
                {
                BOOST_QVM_ASSERT(i>=0);
                BOOST_QVM_ASSERT(i<dim);
                return m_traits<OriginalMatrix>::ir(i,dim,reinterpret_cast<OriginalMatrix const &>(x));
                }

            static
            BOOST_QVM_INLINE_CRITICAL
            scalar_type &
            iw( int i, this_vector & x )
                {
                BOOST_QVM_ASSERT(i>=0);
                BOOST_QVM_ASSERT(i<dim);
                return m_traits<OriginalMatrix>::iw(i,dim,reinterpret_cast<OriginalMatrix &>(x));
                }
            };

        template <class OriginalMatrix,int D>
        struct
        deduce_v<qvm_detail::trans_<OriginalMatrix>,D>
            {
            typedef vec<typename m_traits<OriginalMatrix>::scalar_type,D> type;
            };

        template <class OriginalMatrix,int D>
        struct
        deduce_v2<qvm_detail::trans_<OriginalMatrix>,qvm_detail::trans_<OriginalMatrix>,D>
            {
            typedef vec<typename m_traits<OriginalMatrix>::scalar_type,D> type;
            };

        template <class A>
        typename boost::enable_if_c<
            is_m<A>::value && m_traits<A>::rows==m_traits<A>::cols && m_traits<A>::rows>=3,
            qvm_detail::trans_<A> const &>::type
        BOOST_QVM_INLINE_TRIVIAL
        trans( A const & a )
            {
            return reinterpret_cast<typename qvm_detail::trans_<A> const &>(a);
            }

        template <class A>
        typename boost::enable_if_c<
            is_m<A>::value && m_traits<A>::rows==m_traits<A>::cols && m_traits<A>::rows>=3,
            qvm_detail::trans_<A> &>::type
        BOOST_QVM_INLINE_TRIVIAL
        trans( A & a )
            {
            return reinterpret_cast<typename qvm_detail::trans_<A> &>(a);
            }

        ////////////////////////////////////////////////
        }
    }

#endif
