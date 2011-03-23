#ifndef __STREAM_PROCESS__ESTIMATE_NORMAL__HPP__
#define __STREAM_PROCESS__ESTIMATE_NORMAL__HPP__

#include <stream_process/stream_data.hpp>
#include <stream_process/attribute_accessor.hpp>
#include <stream_process/neighbors.hpp>
#include <stream_process/svd.hpp>

namespace stream_process
{

template< typename sp_types_t >
class estimate_normal
{
public:
    
    STREAM_PROCESS_TYPES
	
	typedef vmml::vector< 3, size_t >			vec3s;

	estimate_normal(
		const attribute_accessor< vec3 >&       get_position,
		const attribute_accessor< nbh_type >&   get_neighbors,
		const attribute_accessor< mat4hp >&     get_covariance,
		attribute_accessor< vec3 >&             get_normal
	);

	inline bool operator()( stream_data* point );
	
protected:
	bool _singular_values_ok( stream_data* point );
	inline void _compute_normal_from_triangles( stream_data* point );
	inline void _compute_normal_from_covariance( stream_data* point );

	const attribute_accessor< vec3 >&		_get_position;
	const attribute_accessor< nbh_type >&   _get_neighbors;
	const attribute_accessor< mat4hp >&		_get_covariance;
	attribute_accessor< vec3 >&				_get_normal;

    size_t                  _max_neighbors;
	hp_float_type           _limit;

    svd< hp_float_type >    _svd;

    vec3                    _major_axis;
    vec3                    _minor_axis;
    vec3                    _tmp_normal;

}; // class estimate_normal


template< typename sp_types_t >
estimate_normal< sp_types_t >::
estimate_normal(
	const attribute_accessor< vec3 >&       get_position,
	const attribute_accessor< nbh_type >&   get_neighbors,
	const attribute_accessor< mat4hp >&     get_covariance,
	attribute_accessor< vec3 >&             get_normal
)	: _get_position( get_position )
	, _get_neighbors( get_neighbors )
	, _get_covariance( get_covariance )
	, _get_normal( get_normal )
	, _max_neighbors( 8 )
	, _limit( 0.0000000001 )
{}



template< typename sp_types_t >
inline bool
estimate_normal< sp_types_t >::
operator()( stream_data* point )
{
    const mat4hp& covariance  = _get_covariance( point ); 

    _svd.compute_3x3( covariance );

	bool svd_good = _singular_values_ok( point );

	if ( svd_good )
        _compute_normal_from_covariance( point );
    else
        _compute_normal_from_triangles( point ); // find alternate normal
		
    
    vec3& normal = _get_normal( point );
    normal.normalize();
        
	return svd_good;
}



template< typename sp_types_t >
bool
estimate_normal< sp_types_t >::
_singular_values_ok( stream_data* point )
{
    const vec3hp& sigma     = _svd.get_sigma();

    size_t k = 0;
    for( size_t index = 0; index < 3; ++index )
    {
        if ( fabs( sigma[ index ] ) < _limit )
            ++k;
    }
    if ( k > 1 )
    {
        // two singular values zero => points form a line
        std::cerr
            << "multiple zero singular values found during SVD of covariance."
            << std::endl;
        return false;
    }
    return true;
}



template< typename sp_types_t >
inline void
estimate_normal< sp_types_t >::
_compute_normal_from_triangles( stream_data* point )
{
    // find alternate normal

	const vec3& position	= _get_position( point );
	vec3& normal			= _get_normal( point );
	const nbh_type& nbh     = _get_neighbors( point );

    normal = 0.0;
    
    size_t max = nbh.size();
	if ( max > _max_neighbors )
		max = _max_neighbors;

	assert( max > 1 );

	size_t end = max - 1;
    for ( size_t k = 0; k < end; ++k ) 
    {
        const vec3& nb_pos		= _get_position( nbh[ k ].get_ptr() ); 
        const vec3& nb_pos2		= _get_position( nbh[ k + 1 ].get_ptr() ); 

        // average from triangles
        _tmp_normal.compute_normal( position, nb_pos, nb_pos2 );

        normal += _tmp_normal;
    }
}




template< typename sp_types_t >
inline void
estimate_normal< sp_types_t >::
_compute_normal_from_covariance( stream_data* point )
{
    // if we're here we know that the svd got reasonable results, so:
   
    const mat3hp& u     = _svd.get_u();
    const vec3hp& sigma = _svd.get_sigma();
    
    const vec3s& order  = _svd.get_order();
    
    // get normal to fitting plane (cross-product of largest pair)
    for( size_t index = 0; index < 3; ++index )
    {
        _minor_axis( index ) = u( index, order( 1 ) );
        _major_axis( index ) = u( index, order( 2 ) );
    }

    _major_axis.normalize();
    _minor_axis.normalize();

    vec3& normal = _get_normal( point ); 

    normal.cross( _minor_axis, _major_axis );
}


} // namespace stream_process

#endif

