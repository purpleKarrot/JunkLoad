//Copyright (c) 2008-2010 Emil Dotchevski and Reverge Studios, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_20D98340A3EB11DEB2180CD156D89593
#define UUID_20D98340A3EB11DEB2180CD156D89593

#include <boost/qvm/inline.hpp>
#include <boost/qvm/deduce_m.hpp>
#include <boost/qvm/assert.hpp>
#include <boost/qvm/enable_if.hpp>
#include <boost/qvm/detail/transp_impl.hpp>

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
            template <int Row,class OriginalMatrix>
            class
            delr_
                {
                delr_( delr_ const & );
                delr_ & operator=( delr_ const & );
                ~delr_();

                public:

                template <class T>
                BOOST_QVM_INLINE_TRIVIAL
                delr_ &
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

        template <int I,class OriginalMatrix>
        struct
        m_traits< qvm_detail::delr_<I,OriginalMatrix> >
            {
            typedef qvm_detail::delr_<I,OriginalMatrix> this_matrix;
            typedef typename m_traits<OriginalMatrix>::scalar_type scalar_type;
            static int const rows=m_traits<OriginalMatrix>::rows-1;
            static int const cols=m_traits<OriginalMatrix>::cols;

            template <int Row,int Col>
            static
            BOOST_QVM_INLINE_CRITICAL
            scalar_type
            r( this_matrix const & x )
                {
                BOOST_QVM_STATIC_ASSERT(Row>=0);
                BOOST_QVM_STATIC_ASSERT(Row<rows);
                BOOST_QVM_STATIC_ASSERT(Col>=0);
                BOOST_QVM_STATIC_ASSERT(Col<cols);
                return m_traits<OriginalMatrix>::template r<Row+(Row>=I),Col>(reinterpret_cast<OriginalMatrix const &>(x));
                }

            template <int Row,int Col>
            static
            BOOST_QVM_INLINE_CRITICAL
            scalar_type &
            w( this_matrix & x )
                {
                BOOST_QVM_STATIC_ASSERT(Row>=0);
                BOOST_QVM_STATIC_ASSERT(Row<rows);
                BOOST_QVM_STATIC_ASSERT(Col>=0);
                BOOST_QVM_STATIC_ASSERT(Col<cols);
                return m_traits<OriginalMatrix>::template w<Row+(Row>=I),Col>(reinterpret_cast<OriginalMatrix &>(x));
                }

            static
            BOOST_QVM_INLINE_CRITICAL
            scalar_type
            ir( int row, int col, this_matrix const & x )
                {
                BOOST_QVM_ASSERT(row>=0);
                BOOST_QVM_ASSERT(row<rows);
                BOOST_QVM_ASSERT(col>=0);
                BOOST_QVM_ASSERT(col<cols);
                return m_traits<OriginalMatrix>::ir(row+(row>=I),col,reinterpret_cast<OriginalMatrix const &>(x));
                }

            static
            BOOST_QVM_INLINE_CRITICAL
            scalar_type &
            iw( int row, int col, this_matrix & x )
                {
                BOOST_QVM_ASSERT(row>=0);
                BOOST_QVM_ASSERT(row<rows);
                BOOST_QVM_ASSERT(col>=0);
                BOOST_QVM_ASSERT(col<cols);
                return m_traits<OriginalMatrix>::iw(row+(row>=I),col,reinterpret_cast<OriginalMatrix &>(x));
                }
            };

        template <int J,class OriginalMatrix,int R,int C>
        struct
        deduce_m<qvm_detail::delr_<J,OriginalMatrix>,R,C>
            {
            typedef mat<typename m_traits<OriginalMatrix>::scalar_type,R,C> type;
            };

        template <int J,class OriginalMatrix,int R,int C>
        struct
        deduce_m2<qvm_detail::delr_<J,OriginalMatrix>,qvm_detail::delr_<J,OriginalMatrix>,R,C>
            {
            typedef mat<typename m_traits<OriginalMatrix>::scalar_type,R,C> type;
            };

        template <int Row,class A>
        typename boost::enable_if_c<
            is_m<A>::value,
            qvm_detail::delr_<Row,A> const &>::type
        BOOST_QVM_INLINE_TRIVIAL
        delr( A const & a )
            {
            return reinterpret_cast<typename qvm_detail::delr_<Row,A> const &>(a);
            }

        template <int Row,class A>
        typename boost::enable_if_c<
            is_m<A>::value,
            qvm_detail::delr_<Row,A> &>::type
        BOOST_QVM_INLINE_TRIVIAL
        delr( A & a )
            {
            return reinterpret_cast<typename qvm_detail::delr_<Row,A> &>(a);
            }

        ////////////////////////////////////////////////

        namespace
        qvm_detail
            {
            template <int Col,class OriginalMatrix>
            class
            delc_
                {
                delc_( delc_ const & );
                delc_ & operator=( delc_ const & );
                ~delc_();

                public:

                template <class T>
                BOOST_QVM_INLINE_TRIVIAL
                delc_ &
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

        template <int J,class OriginalMatrix>
        struct
        m_traits< qvm_detail::delc_<J,OriginalMatrix> >
            {
            typedef qvm_detail::delc_<J,OriginalMatrix> this_matrix;
            typedef typename m_traits<OriginalMatrix>::scalar_type scalar_type;
            static int const rows=m_traits<OriginalMatrix>::rows;
            static int const cols=m_traits<OriginalMatrix>::cols-1;

            template <int Row,int Col>
            static
            BOOST_QVM_INLINE_CRITICAL
            scalar_type
            r( this_matrix const & x )
                {
                BOOST_QVM_STATIC_ASSERT(Row>=0);
                BOOST_QVM_STATIC_ASSERT(Row<rows);
                BOOST_QVM_STATIC_ASSERT(Col>=0);
                BOOST_QVM_STATIC_ASSERT(Col<cols);
                return m_traits<OriginalMatrix>::template r<Row,Col+(Col>=J)>(reinterpret_cast<OriginalMatrix const &>(x));
                }

            template <int Row,int Col>
            static
            BOOST_QVM_INLINE_CRITICAL
            scalar_type &
            w( this_matrix & x )
                {
                BOOST_QVM_STATIC_ASSERT(Row>=0);
                BOOST_QVM_STATIC_ASSERT(Row<rows);
                BOOST_QVM_STATIC_ASSERT(Col>=0);
                BOOST_QVM_STATIC_ASSERT(Col<cols);
                return m_traits<OriginalMatrix>::template w<Row,Col+(Col>=J)>(reinterpret_cast<OriginalMatrix &>(x));
                }

            static
            BOOST_QVM_INLINE_CRITICAL
            scalar_type
            ir( int row, int col, this_matrix const & x )
                {
                BOOST_QVM_ASSERT(row>=0);
                BOOST_QVM_ASSERT(row<rows);
                BOOST_QVM_ASSERT(col>=0);
                BOOST_QVM_ASSERT(col<cols);
                return m_traits<OriginalMatrix>::ir(row,col+(col>=J),reinterpret_cast<OriginalMatrix const &>(x));
                }

            static
            BOOST_QVM_INLINE_CRITICAL
            scalar_type &
            iw( int row, int col, this_matrix & x )
                {
                BOOST_QVM_ASSERT(row>=0);
                BOOST_QVM_ASSERT(row<rows);
                BOOST_QVM_ASSERT(col>=0);
                BOOST_QVM_ASSERT(col<cols);
                return m_traits<OriginalMatrix>::iw(row,col+(col>=J),reinterpret_cast<OriginalMatrix &>(x));
                }
            };

        template <int J,class OriginalMatrix,int R,int C>
        struct
        deduce_m<qvm_detail::delc_<J,OriginalMatrix>,R,C>
            {
            typedef mat<typename m_traits<OriginalMatrix>::scalar_type,R,C> type;
            };

        template <int J,class OriginalMatrix,int R,int C>
        struct
        deduce_m2<qvm_detail::delc_<J,OriginalMatrix>,qvm_detail::delc_<J,OriginalMatrix>,R,C>
            {
            typedef mat<typename m_traits<OriginalMatrix>::scalar_type,R,C> type;
            };

        template <int Col,class A>
        typename boost::enable_if_c<
            is_m<A>::value,
            qvm_detail::delc_<Col,A> const &>::type
        BOOST_QVM_INLINE_TRIVIAL
        delc( A const & a )
            {
            return reinterpret_cast<typename qvm_detail::delc_<Col,A> const &>(a);
            }

        template <int Col,class A>
        typename boost::enable_if_c<
            is_m<A>::value,
            qvm_detail::delc_<Col,A> &>::type
        BOOST_QVM_INLINE_TRIVIAL
        delc( A & a )
            {
            return reinterpret_cast<typename qvm_detail::delc_<Col,A> &>(a);
            }

        ////////////////////////////////////////////////

        namespace
        qvm_detail
            {
            template <int Row,int Col,class OriginalMatrix>
            class
            delrc_
                {
                delrc_( delrc_ const & );
                delrc_ & operator=( delrc_ const & );
                ~delrc_();

                public:

                template <class T>
                BOOST_QVM_INLINE_TRIVIAL
                delrc_ &
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

        template <int I,int J,class OriginalMatrix>
        struct
        m_traits< qvm_detail::delrc_<I,J,OriginalMatrix> >
            {
            typedef qvm_detail::delrc_<I,J,OriginalMatrix> this_matrix;
            typedef typename m_traits<OriginalMatrix>::scalar_type scalar_type;
            static int const rows=m_traits<OriginalMatrix>::rows-1;
            static int const cols=m_traits<OriginalMatrix>::cols-1;

            template <int Row,int Col>
            static
            BOOST_QVM_INLINE_CRITICAL
            scalar_type
            r( this_matrix const & x )
                {
                BOOST_QVM_STATIC_ASSERT(Row>=0);
                BOOST_QVM_STATIC_ASSERT(Row<rows);
                BOOST_QVM_STATIC_ASSERT(Col>=0);
                BOOST_QVM_STATIC_ASSERT(Col<cols);
                return m_traits<OriginalMatrix>::template r<Row+(Row>=I),Col+(Col>=J)>(reinterpret_cast<OriginalMatrix const &>(x));
                }

            template <int Row,int Col>
            static
            BOOST_QVM_INLINE_CRITICAL
            scalar_type &
            w( this_matrix & x )
                {
                BOOST_QVM_STATIC_ASSERT(Row>=0);
                BOOST_QVM_STATIC_ASSERT(Row<rows);
                BOOST_QVM_STATIC_ASSERT(Col>=0);
                BOOST_QVM_STATIC_ASSERT(Col<cols);
                return m_traits<OriginalMatrix>::template w<Row+(Row>=I),Col+(Col>=J)>(reinterpret_cast<OriginalMatrix &>(x));
                }

            static
            BOOST_QVM_INLINE_CRITICAL
            scalar_type
            ir( int row, int col, this_matrix const & x )
                {
                BOOST_QVM_ASSERT(row>=0);
                BOOST_QVM_ASSERT(row<rows);
                BOOST_QVM_ASSERT(col>=0);
                BOOST_QVM_ASSERT(col<cols);
                return m_traits<OriginalMatrix>::ir(row+(row>=I),col+(col>=J),reinterpret_cast<OriginalMatrix const &>(x));
                }

            static
            BOOST_QVM_INLINE_CRITICAL
            scalar_type &
            iw( int row, int col, this_matrix & x )
                {
                BOOST_QVM_ASSERT(row>=0);
                BOOST_QVM_ASSERT(row<rows);
                BOOST_QVM_ASSERT(col>=0);
                BOOST_QVM_ASSERT(col<cols);
                return m_traits<OriginalMatrix>::iw(row+(row>=I),col+(col>=J),reinterpret_cast<OriginalMatrix &>(x));
                }
            };

        template <int I,int J,class OriginalMatrix,int R,int C>
        struct
        deduce_m<qvm_detail::delrc_<I,J,OriginalMatrix>,R,C>
            {
            typedef mat<typename m_traits<OriginalMatrix>::scalar_type,R,C> type;
            };

        template <int I,int J,class OriginalMatrix,int R,int C>
        struct
        deduce_m2<qvm_detail::delrc_<I,J,OriginalMatrix>,qvm_detail::delrc_<I,J,OriginalMatrix>,R,C>
            {
            typedef mat<typename m_traits<OriginalMatrix>::scalar_type,R,C> type;
            };

        template <int Row,int Col,class A>
        typename boost::enable_if_c<
            is_m<A>::value,
            qvm_detail::delrc_<Row,Col,A> const &>::type
        BOOST_QVM_INLINE_TRIVIAL
        delrc( A const & a )
            {
            return reinterpret_cast<typename qvm_detail::delrc_<Row,Col,A> const &>(a);
            }

        template <int Row,int Col,class A>
        typename boost::enable_if_c<
            is_m<A>::value,
            qvm_detail::delrc_<Row,Col,A> &>::type
        BOOST_QVM_INLINE_TRIVIAL
        delrc( A & a )
            {
            return reinterpret_cast<typename qvm_detail::delrc_<Row,Col,A> &>(a);
            }

        ////////////////////////////////////////////////

        namespace
        qvm_detail
            {
            template <int Row,class OriginalMatrix>
            class
            negr_
                {
                negr_( negr_ const & );
                negr_ & operator=( negr_ const & );
                ~negr_();

                public:

                template <class T>
                BOOST_QVM_INLINE_TRIVIAL
                negr_ &
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

        template <int I,class OriginalMatrix>
        struct
        m_traits< qvm_detail::negr_<I,OriginalMatrix> >
            {
            typedef qvm_detail::negr_<I,OriginalMatrix> this_matrix;
            typedef typename m_traits<OriginalMatrix>::scalar_type scalar_type;
            static int const rows=m_traits<OriginalMatrix>::rows;
            static int const cols=m_traits<OriginalMatrix>::cols;

            template <int Row,int Col>
            static
            BOOST_QVM_INLINE_CRITICAL
            scalar_type
            r( this_matrix const & x )
                {
                BOOST_QVM_STATIC_ASSERT(Row>=0);
                BOOST_QVM_STATIC_ASSERT(Row<rows);
                BOOST_QVM_STATIC_ASSERT(Col>=0);
                BOOST_QVM_STATIC_ASSERT(Col<cols);
                return Row==I ?
                    -m_traits<OriginalMatrix>::template r<Row,Col>(reinterpret_cast<OriginalMatrix const &>(x)) :
                    m_traits<OriginalMatrix>::template r<Row,Col>(reinterpret_cast<OriginalMatrix const &>(x));
                }

            static
            BOOST_QVM_INLINE_CRITICAL
            scalar_type
            ir( int row, int col, this_matrix const & x )
                {
                BOOST_QVM_ASSERT(row>=0);
                BOOST_QVM_ASSERT(row<rows);
                BOOST_QVM_ASSERT(col>=0);
                BOOST_QVM_ASSERT(col<cols);
                return row==I?
                    -m_traits<OriginalMatrix>::ir(row,col,reinterpret_cast<OriginalMatrix const &>(x)) :
                    m_traits<OriginalMatrix>::ir(row,col,reinterpret_cast<OriginalMatrix const &>(x));
                }
            };

        template <int J,class OriginalMatrix,int R,int C>
        struct
        deduce_m<qvm_detail::negr_<J,OriginalMatrix>,R,C>
            {
            typedef mat<typename m_traits<OriginalMatrix>::scalar_type,R,C> type;
            };

        template <int J,class OriginalMatrix,int R,int C>
        struct
        deduce_m2<qvm_detail::negr_<J,OriginalMatrix>,qvm_detail::negr_<J,OriginalMatrix>,R,C>
            {
            typedef mat<typename m_traits<OriginalMatrix>::scalar_type,R,C> type;
            };

        template <int Row,class A>
        typename boost::enable_if_c<
            is_m<A>::value,
            qvm_detail::negr_<Row,A> const &>::type
        BOOST_QVM_INLINE_TRIVIAL
        negr( A const & a )
            {
            return reinterpret_cast<typename qvm_detail::negr_<Row,A> const &>(a);
            }

        ////////////////////////////////////////////////

        namespace
        qvm_detail
            {
            template <int Col,class OriginalMatrix>
            class
            negc_
                {
                negc_( negc_ const & );
                negc_ & operator=( negc_ const & );
                ~negc_();

                public:

                template <class T>
                BOOST_QVM_INLINE_TRIVIAL
                negc_ &
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

        template <int J,class OriginalMatrix>
        struct
        m_traits< qvm_detail::negc_<J,OriginalMatrix> >
            {
            typedef qvm_detail::negc_<J,OriginalMatrix> this_matrix;
            typedef typename m_traits<OriginalMatrix>::scalar_type scalar_type;
            static int const rows=m_traits<OriginalMatrix>::rows;
            static int const cols=m_traits<OriginalMatrix>::cols;

            template <int Row,int Col>
            static
            BOOST_QVM_INLINE_CRITICAL
            scalar_type
            r( this_matrix const & x )
                {
                BOOST_QVM_STATIC_ASSERT(Row>=0);
                BOOST_QVM_STATIC_ASSERT(Row<rows);
                BOOST_QVM_STATIC_ASSERT(Col>=0);
                BOOST_QVM_STATIC_ASSERT(Col<cols);
                return Col==J?
                    -m_traits<OriginalMatrix>::template r<Row,Col>(reinterpret_cast<OriginalMatrix const &>(x)) :
                    m_traits<OriginalMatrix>::template r<Row,Col>(reinterpret_cast<OriginalMatrix const &>(x));
                }

            static
            BOOST_QVM_INLINE_CRITICAL
            scalar_type
            ir( int row, int col, this_matrix const & x )
                {
                BOOST_QVM_ASSERT(row>=0);
                BOOST_QVM_ASSERT(row<rows);
                BOOST_QVM_ASSERT(col>=0);
                BOOST_QVM_ASSERT(col<cols);
                return col==J?
                    -m_traits<OriginalMatrix>::ir(row,col,reinterpret_cast<OriginalMatrix const &>(x)) :
                    m_traits<OriginalMatrix>::ir(row,col,reinterpret_cast<OriginalMatrix const &>(x));
                }
            };

        template <int J,class OriginalMatrix,int R,int C>
        struct
        deduce_m<qvm_detail::negc_<J,OriginalMatrix>,R,C>
            {
            typedef mat<typename m_traits<OriginalMatrix>::scalar_type,R,C> type;
            };

        template <int J,class OriginalMatrix,int R,int C>
        struct
        deduce_m2<qvm_detail::negc_<J,OriginalMatrix>,qvm_detail::negc_<J,OriginalMatrix>,R,C>
            {
            typedef mat<typename m_traits<OriginalMatrix>::scalar_type,R,C> type;
            };

        template <int Col,class A>
        typename boost::enable_if_c<
            is_m<A>::value,
            qvm_detail::negc_<Col,A> const &>::type
        BOOST_QVM_INLINE_TRIVIAL
        negc( A const & a )
            {
            return reinterpret_cast<typename qvm_detail::negc_<Col,A> const &>(a);
            }

        ////////////////////////////////////////////////

        template <class A>
        typename boost::enable_if_c<
            is_m<A>::value,
            qvm_detail::transp_<A> const &>::type
        BOOST_QVM_INLINE_TRIVIAL
        transp( A const & a )
            {
            return reinterpret_cast<typename qvm_detail::transp_<A> const &>(a);
            }

        template <class A>
        typename boost::enable_if_c<
            is_m<A>::value,
            qvm_detail::transp_<A> &>::type
        BOOST_QVM_INLINE_TRIVIAL
        transp( A & a )
            {
            return reinterpret_cast<typename qvm_detail::transp_<A> &>(a);
            }

        ////////////////////////////////////////////////
        }
    }

#endif
