#ifndef __STREAM_PROCESS__NORMAL_SPACE_COVARIANCE__HPP__
#define __STREAM_PROCESS__NORMAL_SPACE_COVARIANCE__HPP__

#include <stream_process/rt_struct_member.h>
#include <stream_process/neighbor.h>

namespace stream_process
{

template<
    typename float_t    = float, 
    typename hp_float_t = double
    >
class normal_space_covariance
{
public:
    typedef vmml::vector< 3, float_t >  vec3;
    typedef vmml::vector< 4, float_t >  vec4;

    typedef vmml::vector< 3, hp_float_t >       vec3hp;
    typedef vmml::vector< 4, hp_float_t >       vec4hp;

    typedef vmml::matrix< 4, 4, hp_float_t >    mat4hp;

    normal_space_covariance(
        rt_struct_member< vec3 >&       position, 
        rt_struct_member< vec3 >&       normal, 
        rt_struct_member< neighbor >&   neighbors
        );
    
    const mat4hp& compute( stream_point* point );
    
    void set_max_neighbors( size_t max_k );
    

protected:
    rt_struct_member< vec3 >&       _position;
    rt_struct_member< vec3 >&       _normal;
    rt_struct_member< neighbor >&   _neighbors;
    
    size_t                          _max_neighbors;
    
    mat4hp                          _ns_covariance;
    mat4hp                          _ns_covar_tmp;

    vec4hp                          _tangent_plane;

    hp_float_t                      _weight;

}; // class covariance



template< typename float_t, typename hp_float_t >
normal_space_covariance< float_t, hp_float_t >::
normal_space_covariance( rt_struct_member< vec3 >& position, 
    rt_struct_member< vec3 >& normal, 
    rt_struct_member< neighbor >& neighbors
    )
    : _position( position )
    , _normal( normal )
    , _neighbors( neighbors )
    , _max_neighbors( 8 )
{}




template< typename float_t, typename hp_float_t >
const typename normal_space_covariance< float_t, hp_float_t >::mat4hp&
normal_space_covariance< float_t, hp_float_t >::
compute( stream_point* point )
{
    _ns_covariance.zero(); // C
    _ns_covar_tmp.zero(); // tmp
    
    neighbor* neighbors = point->get_ptr( _neighbors );
    neighbor* current_nb    = neighbors;
    neighbor* neighbors_end = neighbors + _max_neighbors;

    const float_t distance = neighbors[ _max_neighbors - 1 ].get_distance();
    assert( distance != 0.0 );
    
    hp_float_t density = M_PI * distance / _max_neighbors;
    
    _tangent_plane  = 0.0;
    _weight         = 0.0;
    
    for(  ; current_nb != neighbors_end; ++current_nb )
    {
        const vec3& nb_pos      = current_nb->get_point()->get( _position );
        const vec3& nb_normal   = current_nb->get_point()->get( _normal );

        _tangent_plane.set( 
            nb_normal.x(),
            nb_normal.y(),
            nb_normal.z(), 
            dot( nb_pos, nb_normal )
            );
       
        _ns_covar_tmp.tensor( _tangent_plane, _tangent_plane );

        // compute gaussian weight and use it to scale _C
        const hp_float_t f = exp(-0.5 * current_nb->get_distance() / density );

        assert( ! std::isnan( f ) );

        _weight        += f;
        _ns_covar_tmp  *= f;
        _ns_covariance += _ns_covar_tmp;

    }
    
    // scale matrix
    _ns_covariance *= static_cast< hp_float_t >( _max_neighbors ) / _weight;
    
    return _ns_covariance;
}



template< typename float_t, typename hp_float_t >
void
normal_space_covariance< float_t, hp_float_t >::
set_max_neighbors( size_t max_k )
{
    _max_neighbors = max_k;
}



} // namespace stream_process

#endif

