#ifndef __STREAM_PROCESS__COMPUTE_COVARIANCE_FUNCTOR__HPP__
#define __STREAM_PROCESS__COMPUTE_COVARIANCE_FUNCTOR__HPP__

#include <stream_process/stream_data.hpp>
#include <stream_process/stream_process_types.hpp>

namespace stream_process
{

template< typename sp_types_t >
class compute_covariance : public sp_types_t
{
public:
    STREAM_PROCESS_TYPES
    
	compute_covariance(
		const attribute_accessor< vec3 >&       get_position,
		const attribute_accessor< nbh_type >&	get_nbs,
		attribute_accessor< mat4hp >&           get_covariance )
		: _get_position( get_position )
		, _get_neighbors( get_nbs )
		, _get_covariance( get_covariance )
	{}
		
	void operator()( stream_data* point )
	{
		mat4hp& covariance_		= _get_covariance( point );
		const vec3& position	= _get_position( point ); 
		const nbh_type& nbh		= _get_neighbors( point );

        covariance_.zero();
		
        assert( nbh.is_sorted() );
        
		const sp_float_type max_distance	= nbh.get_farthest().get_distance();

		hp_float_type density = M_PI * max_distance / nbh.size();
		
		typename nbh_type::const_iterator it = nbh.begin(), it_end	= nbh.end();
		for( ; it != it_end; ++it )
		{
			const neighbor_type& nb	= *it;
			const vec3& nb_pos		= _get_position( nb.get_ptr() ); 
			
			_diff.set(
				position.x() - nb_pos.x(),
				position.y() - nb_pos.y(),
				position.z() - nb_pos.z()
				);

			_C.tensor( _diff, _diff ); 

			// compute gaussian weight and use it to scale _C
			const hp_float_type f = exp( -0.5 * nb.get_distance() / density );
			_C *= f; 

			covariance_ += _C;
		}
	}

protected:
	const attribute_accessor< vec3 >&		_get_position;
	const attribute_accessor< nbh_type >&		_get_neighbors;
	attribute_accessor< mat4hp >&			_get_covariance;
	
	vec3hp	_diff;
	mat4hp	_C;

}; // class compute_covariance


} // namespace stream_process

#endif

