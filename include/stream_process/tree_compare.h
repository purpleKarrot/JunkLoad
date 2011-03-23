ifndef __VMML__TREE_COMPARE__H__
#define __VMML__TREE_COMPARE__H__

#include "tree_common.h"

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



inline size_t operator()( const vector_t< T >& vec, const vector_t< T >& vec2 ) const
{
    return vec2.smaller( vec );
}



inline const T get( const vector_t< T >& vec, const ssize_t axis ) const
{
    return vec[ axis ];
}



inline const vector_t< T >& get( const vector_t< T >& vec ) const
{
    return vec;
}



inline vector_t< T >& get( vector_t< T >& vec )
{
    return vec;
}



}; // struct vector_compare


} // namespace stream_process

#endif

