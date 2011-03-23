#ifndef __VMML__KDTREE_FUNCTORS__H__
#define __VMML__KDTREE_FUNCTORS__H__

#include "kdtree_node.h"
#include "kdtree_common.h"
#include "kdtree_aabb.h"

namespace vmml
{

template< typename T,
          template< typename T > class vector_t
         > 
struct vector_greater
{

inline size_t operator()( const vector_t< T >& vec, const T& split_point,
    const ssize_t axis ) const
{
    assert( axis > -1 );
    if ( vec[ axis ] > split_point )
        return 1;
    else
        return 0;
}



inline size_t operator()( const vector_t< T >& vec1, const vector_t< T >& vec2,
                          const ssize_t axis ) const
{
    assert( axis > -1 );
    if ( vec1[ axis ] > vec2[ axis ] )
        return 1;
    else
        return 0;
}



inline size_t operator()( const vector_t< T >& vec, const node_data& node ) const
{
    assert( node.axis > -1 );
    if ( vec[ node.axis ] > node.split_value )
        return 1;
    else
        return 0;
}



}; // struct vector_compare

};//namespace vmml

#endif

