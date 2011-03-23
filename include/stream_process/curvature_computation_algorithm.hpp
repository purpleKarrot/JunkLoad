#ifndef __STREAM_PROCESS__CURVATURE_COMPUTATION_ALGORITHM__HPP__
#define __STREAM_PROCESS__CURVATURE_COMPUTATION_ALGORITHM__HPP__

#include "op_algorithm.hpp"
#include "neighbor.h"

#include "curvature.hpp"

namespace stream_process
{

template< typename float_t, typename integer_t = uint32_t >
class curvature_computation_algorithm : public op_algorithm
{
    typedef vmml::vector< 2, float_t > vec2;
    typedef vmml::vector< 3, float_t > vec3;
    typedef vmml::vector< 4, float_t > vec4;
    typedef vmml::matrix< 3, 3, float_t > mat3;
    typedef vmml::matrix< 4, 4, float_t > mat4;
        
public:
    curvature_computation_algorithm();

    virtual op_algorithm* clone();
    
    virtual void setup_stage_0();
    
protected:
	virtual void _compute( stream_point* point );
    virtual void _compute_theta_k1_k2();

    rt_struct_member< vec3f >       _position;
    rt_struct_member< vec3f >       _normal;
    rt_struct_member< float >       _radius;
    rt_struct_member< neighbor >    _neighbors;
    rt_struct_member< integer_t >   _nb_count;
    
    rt_struct_member< float >       _gaussian_curvature;
    rt_struct_member< float >       _mean_curvature;

    std::vector< vec3 >             _neighbor_positions;

    vec4                            _tangent_plane;
    
    float_t                         _k1;
    float_t                         _k2;
    float_t                         _theta;
    float_t                         _theta1;
    float_t                         _theta2;
    float_t                         _k_alpha;
    float_t                         _k_beta;
    float_t                         _k_gamma;
    
    vec3                            _t_alpha;
    vec3                            _t_beta;
    vec3                            _t_gamma;

    curvature< float_t >                    _curv;
    typename curvature< float_t >::params   _params;
    
}; // class curvature_computation_algorithm



template< typename float_t, typename integer_t >
curvature_computation_algorithm< float_t, integer_t >::curvature_computation_algorithm()
    : _position(    "position" )
    , _normal(      "normal" )
    , _radius(      "radius" )
    , _neighbors(   "neighbors" )
    , _nb_count(    "nb_count" )
    , _gaussian_curvature( "gaussian_curv" )
    , _mean_curvature( "mean_curv" )
{
    set_name( "curvature (new)" );
    // FIXME
    _neighbor_positions.reserve( 8 );
}



template< typename float_t, typename integer_t >
op_algorithm*
curvature_computation_algorithm< float_t, integer_t >::clone()
{ 
    return new curvature_computation_algorithm< float_t >( *this ); 
}



template< typename float_t, typename integer_t >
void
curvature_computation_algorithm< float_t, integer_t >::_compute( stream_point* point )
{
    vec3 position, normal;
    position                    = point->get( _position );
    normal                      = point->get( _normal );   

    const integer_t nb_count    = point->get( _nb_count );
    neighbor* nb                = point->get_ptr( _neighbors );
    const neighbor* nb_end      = nb + nb_count;

    _neighbor_positions.clear();

    vec3 tmp;
    for( ; nb != nb_end; ++nb )
    {
        tmp = (*nb).get_point()->get( _position );
        _neighbor_positions.push_back( tmp );
    }
    
    _params.position        = position;
    _params.normal          = normal;
    _params.nb_positions    = &_neighbor_positions;
    
      
    _tangent_plane.set( normal.getNormalized(), - normal.dot( position ) );
    
    _t_alpha = position + vec3( 1.0, 0.0, 0.0 );
    _t_alpha = _tangent_plane.projectPointOntoPlane( _t_alpha );
    _t_alpha -= position;
    _t_alpha.normalize();
    
    _params.tangent = _t_alpha;
    _k_alpha = _curv.compute_normal_curvature( _params );

    _t_beta = position + vec3( 0.0, 1.0, 0.0 );
    _t_beta = _tangent_plane.projectPointOntoPlane( _t_beta );
    _t_beta -= position;
    _t_beta.normalize();

    _theta1 = acos( _t_alpha.dot( _t_beta ) );

    _params.tangent = _t_beta;
    _k_beta = _curv.compute_normal_curvature( _params );

    _t_gamma = position + vec3( 0.0, 0.0, 1.0 );
    _t_gamma = _tangent_plane.projectPointOntoPlane( _t_gamma );
    _t_gamma -= position;
    _t_gamma.normalize();

    _theta2 = acos( _t_alpha.dot( _t_gamma ) );

    _params.tangent = _t_gamma;
    _k_gamma = _curv.compute_normal_curvature( _params );
    
    _compute_theta_k1_k2();
    
    // write results to stream
    float& K = point->get( _gaussian_curvature );
    float& H = point->get( _mean_curvature );
    
    K = _k1 * _k2;
    H = ( _k1 + _k2 ) * 0.5f;

}


template< typename float_t, typename integer_t >
void
curvature_computation_algorithm< float_t, integer_t >::
_compute_theta_k1_k2()
{
    /*
    float_t theta = - 0.5 * theta2 - 0.5 *
        atan( 
                ( 
                    sin( theta1-theta2 ) * sin( theta1 ) *  ( k_alpha - k_gamma ) 
                ) 
            / 
                ( 
                    -sin( theta2 ) * k_alpha 
                    + 
                    sin( theta2 ) * k_beta 
                    + 
                    cos( theta1 - theta2 ) * sin( theta1 ) * k_alpha 
                    - 
                    cos( theta1 - theta2 ) * sin( theta1 ) * k_gamma
                )
                );
    */
    float_t sin_theta1 = sin( _theta1 );
    float_t sin_theta2 = sin( _theta2 );
    float_t t1_minus_t2 = _theta1 - _theta2;
    float_t sin_t1_minus_t2 = sin( t1_minus_t2 );
    float_t cos_t1_minus_t2 = cos( t1_minus_t2 );
    
    _theta = -0.5 * _theta2 - 0.5*
        atan( 
            (
                sin_t1_minus_t2 * sin_theta1 * ( _k_alpha - _k_gamma )
            )
            / 
            ( 
                -sin_theta2 * _k_alpha 
                + sin_theta2 * _k_beta 
                + cos_t1_minus_t2 * sin_theta1 * _k_alpha 
                - cos_t1_minus_t2 * sin_theta1 * _k_gamma 
            ) 
            );

    float_t sin_theta_squared = sin( _theta );
    sin_theta_squared        *= sin_theta_squared;

    float_t cos_theta_squared = cos( _theta );
    cos_theta_squared        *= cos_theta_squared;
    
    float_t t_plus_t1_squared   = _theta + _theta1;
    t_plus_t1_squared         *= t_plus_t1_squared;
    
    float_t sin_t_t1_sq         = sin( t_plus_t1_squared );
    float_t cos_t_t1_sq         = cos( t_plus_t1_squared );
    
    _k1 = ( - _k_alpha * sin_t_t1_sq + sin_theta_squared * _k_beta ) 
        /
        ( - sin_t_t1_sq * cos_theta_squared + sin_theta_squared * cos_t_t1_sq );
    
    _k2 = - ( - _k_alpha + _k1 * cos_theta_squared ) / sin_theta_squared;
}




template< typename float_t, typename integer_t >
void
curvature_computation_algorithm< float_t, integer_t >::setup_stage_0()
{
    _reserve( _gaussian_curvature, IO_WRITE_TO_OUTPUT );
    _reserve( _mean_curvature, IO_WRITE_TO_OUTPUT );
}


} // namespace stream_process

#endif

