#ifndef __STREAM_PROCESS__PERTURB_VECTOR__HPP__
#define __STREAM_PROCESS__PERTURB_VECTOR__HPP__

#include <cstdlib>

namespace stream_process
{

template< typename T >
struct perturb_vector
{

// perturbs vector by a small amount
void operator()( T& vec_, const typename T::value_type range_ = 0.0001 ) const
{
    typename T::value_type p = ( rand() & 1 ) ? range_ : -range_;
    
    typename T::iterator it = vec_.begin(), it_end = vec_.end();
    for( ; it != it_end; ++it )
    {
        *it += p;
        p = ( rand() & 1 ) ? range_ : -range_;
    }
    
    normalize( vec_ );
}

void perturb_if_unit( T& vec_, const typename T::value_type range_ = 0.0001 ) const
{
    size_t zeroes = 0;
    typename T::const_iterator it = vec_.begin(), it_end = vec_.end();
    for( ; it != it_end; ++it )
    {
        if ( *it == 0.0 )
        {
            ++zeroes;
        }
    }
    
    if ( zeroes == vec_.size() - 1 )
    {
        operator()( vec_, range_ );
    }
}


}; // class perturb_vector

} // namespace stream_process

#endif

