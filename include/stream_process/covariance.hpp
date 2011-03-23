#ifndef __STREAM_PROCESS__COVARIANCE__HPP__
#define __STREAM_PROCESS__COVARIANCE__HPP__

#include <stream_process/rt_struct_member.h>
#include <stream_process/neighbor.h>

#include <stream_process/attribute_accessor.hpp>
#include <stream_process/neighbors.hpp>

namespace stream_process
{

template<
    typename float_t    = float, 
    typename hp_float_t = double
    >
class covariance
{
public:
    typedef vmml::vector< 3, float_t >          vec3;
    typedef vmml::vector< 4, float_t >          vec4;

    typedef vmml::vector< 3, hp_float_t >       vec3hp;
    typedef vmml::vector< 4, hp_float_t >       vec4hp;

    typedef vmml::matrix< 4, 4, hp_float_t >    mat4hp;
    
    covariance(
        rt_struct_member< vec3 >&       position, 
        rt_struct_member< neighbor >&   neighbors
        );
    
    const mat4hp& compute( stream_point* point );

    void set_max_neighbors( size_t max_k );

protected:
    rt_struct_member< vec3 >&       _position;
    rt_struct_member< neighbor >&   _neighbors;
    
    size_t                          _max_neighbors;
    
    mat4hp                          _covariance;
    mat4hp                          _C;
    vec3hp                          _diff;

}; // class covariance



template< typename float_t, typename hp_float_t >
covariance< float_t, hp_float_t >::
covariance( rt_struct_member< vec3 >& position, 
    rt_struct_member< neighbor >& neighbors
    )
    : _position( position )
    , _neighbors( neighbors )
    , _C( mat4hp::IDENTITY )
    , _max_neighbors( 8 )
{}




template< typename float_t, typename hp_float_t >
const typename covariance< float_t, hp_float_t >::mat4hp&
covariance< float_t, hp_float_t >::
compute( stream_point* point )
{
    _covariance.zero();
    
    // get the point position point attribute
    const vec3& position = point->get( _position );
    
    // get the neighbor array pointer point attribute
    neighbor* neighbors = point->get_ptr( _neighbors );
    
    const float_t distance = neighbors[ _max_neighbors - 1 ].get_distance();
    hp_float_t density = M_PI * distance / _max_neighbors;
    
    neighbor* current_nb    = neighbors;
    neighbor* neighbors_end = neighbors + _max_neighbors;
    for(  ; current_nb != neighbors_end; ++current_nb )
    {
        const vec3& nb_pos = current_nb->get_point()->get( _position );
        
        _diff.set(
            position.x() - nb_pos.x(),
            position.y() - nb_pos.y(),
            position.z() - nb_pos.z()
            );

        _C.tensor( _diff, _diff ); 
         
        // compute gaussian weight and use it to scale _C
        const hp_float_t f = exp( -0.5 * current_nb->get_distance() / density );

        _C *= f; 

        _covariance += _C;
    }
    
    return _covariance;
}




template< typename float_t, typename hp_float_t >
void
covariance< float_t, hp_float_t >::
set_max_neighbors( size_t max_k )
{
    _max_neighbors = max_k;
}





} // namespace stream_process

#endif

