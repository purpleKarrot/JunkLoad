#ifndef __STREAM_PROCESS__COMPUTE_CENTROID__HPP__
#define __STREAM_PROCESS__COMPUTE_CENTROID__HPP__

#include <stream_process/stream_data.hpp>
#include <stream_process/stream_process_types.hpp>

#include <boost/function.hpp>

namespace stream_process
{


template< typename sp_types_t, typename T >
class compute_centroid : public sp_types_t
{
public:
    STREAM_PROCESS_TYPES
    
    typedef T                               fp_type;
    typedef vmml::vector< 3, fp_type >      fp_vec3;
    
	compute_centroid(
		const attribute_accessor< vec3 >&           get_position,
		const attribute_accessor< nbh_type >&       get_nbs,
        const attribute_accessor< sp_float_type >&  get_radius,
        boost::function< T ( T ) >&                 weight_func,
        boost::function< void ( T, size_t ) >&      weight_func_feature_size
        )
		: _get_position( get_position )
		, _get_neighbors( get_nbs )
        , _get_radius( get_radius )
        , _weight( weight_func )
        , _set_feature_size( weight_func_feature_size )
	{}
    
    void operator()( const stream_data* point, vec3& centroid )
    {
        fp_vec3 x( _get_position( point ) ), nb_pos;

        const nbh_type& nbh     = _get_neighbors( point );
        
        fp_type max_dist        = nbh.get_farthest().get_distance();
        
        _set_feature_size( max_dist * 2.0, 0 );

        T accum_w;
        fp_vec3 accum_wp;
        accum_wp = accum_w = 0.0;

		typename nbh_type::const_iterator it = nbh.begin(), it_end	= nbh.end();
		for( ; it != it_end; ++it )
		{
			const neighbor_type& nb	= *it;
			nb_pos = _get_position( nb.get_ptr() ); // pi
            
            T w = _weight( nb.get_distance() );

            accum_w += w;
            accum_wp += nb_pos * w;

        }
        
        centroid = accum_wp / accum_w;
    }

    void operator()( const stream_data* point, vec3& centroid, const vec3& query_pos )
    {
        fp_vec3 x( query_pos ), nb_pos;
    
        const nbh_type& nbh     = _get_neighbors( point );
        
        fp_type max_dist        = nbh.get_farthest().get_distance();
        
        _set_feature_size( max_dist * 2.0, 0 );

        T accum_w;
        fp_vec3 accum_wp;
        accum_wp = accum_w = 0.0;

		typename nbh_type::const_iterator it = nbh.begin(), it_end	= nbh.end();
		for( ; it != it_end; ++it )
		{
			const neighbor_type& nb	= *it;
			nb_pos = _get_position( nb.get_ptr() ); // pi
            
            T w = _weight( ( x - nb_pos ).length() );

            accum_w += w;
            accum_wp += nb_pos * w;

        }
        
        centroid = accum_wp / accum_w;
        
    }

protected:
	const attribute_accessor< vec3 >&           _get_position;
	const attribute_accessor< nbh_type >&       _get_neighbors;
    const attribute_accessor< sp_float_type >&  _get_radius;

    boost::function< T ( T ) >&             _weight;
    boost::function< void ( T, size_t ) >&  _set_feature_size;
    
    fp_vec3 _w_cov;
    fp_vec3 _w_cov_p;
    
}; // class compute_centroid

} // namespace stream_process

#endif

