#ifndef __VMML__KDTREE_AABB__H__
#define __VMML__KDTREE_AABB__H__

#include "aabb.h"
#include "kdtree_split.h"

namespace vmml
{

template< typename T,
          template< typename T > class vector_t,
          unsigned int dimensions
          >
class kdtree_aabb : public aabb< T, vector_t, dimensions >
{
public:
    kdtree_aabb();
    kdtree_aabb( const vector_t< T >& min, const vector_t< T >& max );

    void split( KDTREE_SPLIT_AXIS axis,     
        kdtree_aabb& child0, kdtree_aabb& child1 );
}; //class kdtree_aabb



template< typename T, template< typename T > class vector_t, unsigned int dimensions >
kdtree_aabb< T, vector_t, dimensions >
    ::kdtree_aabb( const vector_t< T >& min, const vector_t< T >& max )
    : aabb< T, vector_t, dimensions >( min, max )
{}



template< typename T, template< typename T > class vector_t, unsigned int dimensions >
kdtree_aabb< T, vector_t, dimensions >
    ::kdtree_aabb()
: aabb< T, vector_t, dimensions >()
{}



template< typename T, template< typename T > class vector_t, unsigned int dimensions >
inline void kdtree_aabb< T, vector_t, dimensions >
    ::split( KDTREE_SPLIT_AXIS axis, kdtree_aabb& child0, kdtree_aabb& child1 )
{
    memcpy( &child0, this, sizeof( kdtree_aabb ) );
    memcpy( &child1, this, sizeof( kdtree_aabb ) );
    T diff = 0.5 * ( child0._max[ axis ] - child0._min[ axis ] );
    child0._max[ axis ] -= diff;  
    child1._min[ axis ] += diff;
}


};//namespace vmml

#endif
