#ifndef __STREAM_PROCESS__PAIR_FUNCTORS__H__
#define __STREAM_PROCESS__PAIR_FUNCTORS__H__

#include <functional>
#include <algorithm>

namespace stream_process
{

template< 
    typename first_t, 
    typename second_t, 
    typename compare_t
    >
struct pair_compare_first_adapter
    : std::binary_function< 
        std::pair< first_t, second_t > const&,
        std::pair< first_t, second_t > const&,
        bool 
        >
{
    bool operator()( 
        const std::pair< first_t, second_t >& first_pair, 
        const std::pair< first_t, second_t >& second_pair 
        )
    {
        return compare_t()( first_pair.first, second_pair.first );
    }

}; // struct pair_compare_first_adapter


template< 
    typename first_t, 
    typename second_t, 
    typename compare_t
    >
struct pair_ptr_compare_first_adapter
    : std::binary_function< 
        std::pair< first_t, second_t > const*,
        std::pair< first_t, second_t > const*,
        bool 
        >
{
    bool operator()( 
        const std::pair< first_t, second_t >* first_pair, 
        const std::pair< first_t, second_t >* second_pair 
        )
    {
        return compare_t()( first_pair->first, second_pair->first );
    }
    

}; // struct pair_compare_first_adapter


} // namespace stream_process

#endif
