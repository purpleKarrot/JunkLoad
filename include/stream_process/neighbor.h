#ifndef __VMML__NEIGHBOR_H__
#define __VMML__NEIGHBOR_H__

#include <float.h>
#include <functional>

#include "stream_point.h"

namespace stream_process
{

struct neighbor : public std::pair< float, stream_point* >
{
    neighbor(){ clear(); };
    neighbor( float distance, stream_point* point ) 
        : std::pair< float, stream_point* >( distance, point ) {} 

    const neighbor& operator=( const std::pair< float, stream_point* >& other )
    {
        std::pair< float, stream_point* >::first    = other.first;
        std::pair< float, stream_point* >::second   = other.second;
        return *this;
    }

    inline stream_point* get_point() const
    {
        return second;
    }

    inline float get_distance() const
    {
        return first;
    }
    
    inline void set_distance( float distance_ )
    {
        first = distance_;
    }
    
    inline void set( float distance, stream_point* point )
    {
        std::pair< float, stream_point* >::first    = distance;
        std::pair< float, stream_point* >::second   = point;        
    }

    inline void clear()
    {
        std::pair< float, stream_point* >::first 
            = std::numeric_limits< float >::max();
        std::pair< float, stream_point* >::second = 0;
    }

}; // struct neighbor



//struct neighbor_distance_less 
//struct neighbor_distance_greater 
// -> use pair_compare_first_adapter in priority_queue_functors.h
struct neighbor_ptr_distance_less 
    : public std::binary_function< neighbor const*, neighbor const*, bool >
{
    bool operator()( neighbor const* nb_0, neighbor* const nb_1 )
    {
        return _less( nb_0->first, nb_1->first );
    }
    
    std::less< float > _less;
}; // struct neighbor_ptr_distance_less



struct neighbor_ptr_distance_greater 
    : public std::binary_function< neighbor const*, neighbor const*, bool >
{
    bool operator()( neighbor const* nb_0, neighbor* const nb_1 )
    {
        return _greater( nb_0->first, nb_1->first );
    }

    std::greater< float > _greater;
}; // struct neighbor_ptr_distance_greater



} // namespace stream_process

#endif

