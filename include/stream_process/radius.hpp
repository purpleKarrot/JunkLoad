#ifndef __STREAM_PROCESS__RADIUS__HPP__
#define __STREAM_PROCESS__RADIUS__HPP__

#include <stream_process/rt_struct_member.h>
#include <vmmlib/details.hpp>
#include <stream_process/neighbor.h>

namespace stream_process
{

template< typename float_t >
class radius
{
public:
    radius( rt_struct_member< neighbor >& neighbors_, size_t max_nb = 8 );

    float_t compute_radius_kth_nb( stream_point* point ) const;
    float_t compute_mls_support_radius( stream_point* point ) const;
    
    void set_max_neighbors( const size_t max_neighbors_ );
    
protected:
    rt_struct_member< neighbor >&   _neighbors;

    size_t      _max_neighbors;
    float_t     _boundary_points;

}; // class radius



template< typename float_t >
radius< float_t >::radius( rt_struct_member< neighbor >& neighbors_, size_t max_nb )
    : _neighbors( neighbors_ )
    , _max_neighbors()
{
    set_max_neighbors( max_nb );
}



template< typename float_t >
float_t
radius< float_t >::compute_mls_support_radius( stream_point* point ) const
{
    using vmml::details::getSquareRoot;

    neighbor* neighbors         = point->get_ptr( _neighbors );

    const float_t& dist_to_farthest_nb
        = neighbors[ _max_neighbors - 1 ].get_distance();

    return 2.0 * getSquareRoot( 
        dist_to_farthest_nb / ( _max_neighbors - 0.5 * _boundary_points )
        );

}


template< typename float_t >
float_t
radius< float_t >::compute_radius_kth_nb( stream_point* point ) const
{
    using vmml::details::getSquareRoot;

    neighbor* neighbors         = point->get_ptr( _neighbors );

    return getSquareRoot( neighbors[ _max_neighbors - 1 ].get_distance() );

}



template< typename float_t >
void
radius< float_t >::set_max_neighbors( const size_t max_neighbors_ )
{
    using vmml::details::getSquareRoot;

    _max_neighbors = max_neighbors_;
    
    const float_t pi = M_PI;
    _boundary_points = 
        2.0 * getSquareRoot( pi ) *
        getSquareRoot( static_cast< float_t >( _max_neighbors ) ) - pi;
}

} // namespace stream_process

#endif

