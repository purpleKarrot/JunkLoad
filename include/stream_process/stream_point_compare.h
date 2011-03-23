#ifndef __VMML__STREAM_POINT_COMPARE__H__
#define __VMML__STREAM_POINT_COMPARE__H__

#include "tree_common.h"

#include "stream_point.h"
#include "rt_struct_member.h"

namespace vmml
{

template< typename T,
          template< typename T > class vector_t
         > 
struct stream_point_compare
{

rt_struct_member< vec3f > position;

stream_point_compare() : position( "position" )
{}

inline size_t operator()( const stream_point* p, const T& split_point,
    const ssize_t axis ) const
{
    assert( axis > -1 );
#ifndef NDEBUG
    T f = position.get( p )[ axis ];
    if ( f > split_point )
#else
    if ( position.get( p )[ axis ] > split_point )
#endif
        return 1;
    else
        return 0;
}



inline size_t operator()( const stream_point* p, const vector_t< T >& vec2,
                          const ssize_t axis ) const
{
    assert( axis > -1 );
    if (  position.get( p )[ axis ] > vec2[ axis ] )
        return 1;
    else
        return 0;
}



inline size_t operator()( const stream_point* p, const vector_t< T >& vec2 ) const
{
    return vec2.smaller( position.get( p ) );
}



inline const T get( const stream_point* p, const ssize_t axis ) const
{
    return position.get( p )[ axis ];
}



inline const vector_t< T >& get( const stream_point* p ) const
{
    return position.get( p );
}



inline vector_t< T >& get( const stream_point* p )
{
    return position.get( p );
}

}; // struct stream_op_compare


template< size_t offset, size_t axis >
struct stream_point_less 
    : std::binary_function< stream_point const *, stream_point const *, bool > 
{
    stream_point_less()
    {
        position = offset;
    }

    bool operator()( stream_point const* x, stream_point const* y ) const
    {
        return position.get( x )[ axis ] < position.get( y )[ axis ];
    }

    rt_struct_member< vec3f > position;
};




template< size_t offset, size_t axis >
struct stream_point_greater 
    : std::binary_function< stream_point const *, stream_point const *, bool > 
{
    stream_point_greater()
    {
        position = offset;
    }

    bool operator()( stream_point const* x, stream_point const* y ) const
    {
        return position.get( x )[ axis ] > position.get( y )[ axis ];
    }

    rt_struct_member< vec3f > position;
};



};//namespace vmml

#endif

