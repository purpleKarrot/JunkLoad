#ifndef __STREAM_PROCESS__COMPUTE_RADIUS__HPP__
#define __STREAM_PROCESS__COMPUTE_RADIUS__HPP__

#include <stream_process/stream_data.hpp>
#include <stream_process/neighbors.hpp>

#include <vmmlib/math.hpp>

namespace stream_process
{

template< typename sp_types_t >
class compute_radius
{
public:

    STREAM_PROCESS_TYPES

	compute_radius( 
		const attribute_accessor< vec3 >&       get_position, 
		const attribute_accessor< nbh_type >&	get_neighbors,
		attribute_accessor< sp_float_type >&       get_radius
	)	: _get_position( get_position )
		, _get_neighbors( get_neighbors )
		, _get_radius( get_radius )
		, _boundary_points( 0 )
		, _max_neighbors( 0 )
	{
		set_max_neighbors( 8 );
	}
	
    inline void compute_radius_kth_nb( stream_data* point );
    inline void compute_mls_support_radius( stream_data* point );
		
    
    inline void debug_radius_func( stream_data* point );
    
	void set_max_neighbors( size_t max_nb );
	
protected:
	const attribute_accessor< vec3 >&       _get_position;
	const attribute_accessor< nbh_type >&	_get_neighbors;
	attribute_accessor< sp_float_type >&	_get_radius;
	
	sp_float_type	_boundary_points;
	size_t          _max_neighbors;

}; // class compute_radius



template< typename sp_types_t >
inline void
compute_radius< sp_types_t >::
compute_mls_support_radius( stream_data* point )
{
	const nbh_type& nbh     = _get_neighbors( point );
	sp_float_type& radius_  = _get_radius( point );

	size_t max_nb = nbh.size();
	if ( max_nb > _max_neighbors )
		max_nb = _max_neighbors;

    assert( nbh.is_sorted() );
    const sp_float_type& dist_to_farthest_nb = nbh[ max_nb - 1 ].get_distance();

	radius_ = 2.0 * sqrt( dist_to_farthest_nb / 
		( static_cast< sp_float_type >( _max_neighbors ) - _boundary_points ) );

    sp_float_type check_ = sqrt( dist_to_farthest_nb ) * 5.0;
    if ( radius_ > check_ )
        radius_ = check_;
}



template< typename sp_types_t >
inline void
compute_radius< sp_types_t >::
compute_radius_kth_nb( stream_data* point )
{
	const nbh_type& nbh     = _get_neighbors( point );
	sp_float_type& radius_  = _get_radius( point );
	
	size_t max_nb = nbh.size();
	if ( max_nb > _max_neighbors )
		max_nb = _max_neighbors;

    assert( nbh.is_sorted() );
    const sp_float_type& distance = nbh[ max_nb - 1 ].get_distance();
	
	radius_ = sqrt( distance );
}



template< typename sp_types_t >
inline void
compute_radius< sp_types_t >::
debug_radius_func( stream_data* point )
{
	sp_float_type& radius_  = _get_radius( point );
	radius_ = 0.1;
}



template< typename sp_types_t >
void
compute_radius< sp_types_t >::
set_max_neighbors( const size_t max_neighbors_ )
{
    _max_neighbors = max_neighbors_;
    
    const sp_float_type pi = M_PI;
    _boundary_points = sqrt( pi ) *
        sqrt( static_cast< sp_float_type >( _max_neighbors ) ) - pi;
}



} // namespace stream_process

#endif

