#ifndef __STREAM_PROCESS__CURVATURE__HPP__
#define __STREAM_PROCESS__CURVATURE__HPP__

#include "VMMLibIncludes.h"
#include <vmmlib/lapack_linear_least_squares.hpp>

#include <vector>

namespace stream_process
{

template< typename float_t >
class curvature
{
public:
    typedef vmml::vector< 2, float_t >      vec2;
    typedef vmml::vector< 3, float_t >      vec3;
    typedef vmml::vector< 4, float_t >      vec4;
    typedef vmml::matrix< 3, 3, float_t >   mat3;
    typedef vmml::matrix< 4, 4, float_t >   mat4;

    struct params
    {
        params() : nb_positions( 0 ) {}
        
        std::vector< vec3 >* nb_positions;
        vec3 position;
        vec3 normal;
        vec3 tangent;
    };
    
    curvature();


    float_t compute_normal_curvature( const params& params_ );
    
    // PRE: compute_normal_curvature has been called before
    void get_a_and_b( float_t& a, float_t& b );
    
    const std::vector< vec3 >& get_projected_nbs() const;
    const std::vector< vec3 >& get_transformed_projected_nbs() const;
    
    const vec4& get_normal_plane() const;

    void    set_tolerance( float_t tolerance_ );
    float_t get_tolerance();

protected:
    void _reset( size_t neighbor_count );

    const vec3 _x0;
    const vec3 _y0;
    const vec3 _z0;
    
    vec3 _x1;
    vec3 _y1;
    vec3 _z1;
    
    vec4 _normal_plane;
    
    std::vector< vec3 > _projected_nbs;
    std::vector< vec3 > _transformed_proj_nbs;

    mat4  _rotation;
    mat4  _rotation_inverse;
    
    float_t _a;
    float_t _b;
    float_t _kr;
    
    float_t _tolerance;
    
    vmml::lapack::linear_least_squares_xgels< 5, 2, float_t > 
        _lapack_llsq;
    
    vmml::matrix< 5, 2, float_t >   _A;
    vmml::vector< 5, float_t >      _B;
    vmml::vector< 2, float_t >      _X;
    
}; // class curvature



template< typename float_t >
curvature< float_t >::curvature()
    : _x0 ( 1.0, 0.0, 0.0 )
    , _y0 ( 0.0, 1.0, 0.0 )
    , _z0 ( 0.0, 0.0, 1.0 )
    , _rotation( mat4::IDENTITY )
    , _tolerance( 1e-6 ) // FIXME
{}



template< typename float_t >
float_t
curvature< float_t >::compute_normal_curvature(
    const params& params_ )
{
    assert( params_.nb_positions != 0 );
    const std::vector< vec3 >& nb_positions = *params_.nb_positions;

    _reset( nb_positions.size() );

    const vec3& position    = params_.position;
    const vec3& normal      = params_.normal;
    const vec3& tangent     = params_.tangent;

    //assert( normal.dot( tangent ) == 0.0 );
    //FIXME assert( normal.dot( tangent ) < _tolerance );

    _x1 = tangent; //normal.cross( vec3(0,0,1));
    _y1 = normal;
    _z1 = normal.cross( _x1 );
    
    _x1.normalize();
    _z1.normalize();


    assert( _y1.length() - 1.0 < _tolerance );


    typename std::vector< vec3 >::const_iterator nb, nb_end;
    nb      = nb_positions.begin();
    nb_end  = nb_positions.end();
 
    _normal_plane.set( _z1, - _z1.dot( position ) );   
    
    for( ; nb != nb_end; ++nb )
    {
        _projected_nbs.push_back( _normal_plane.projectPointOntoPlane( *nb ) );
    }
    
    _rotation = mat4::IDENTITY;
    
    #ifdef SPROCESS_OLD_VMMLIB
    _rotation.setColumn( 0, _x1 );
    _rotation.setColumn( 1, _y1 );
    _rotation.setColumn( 2, _z1 );
    #else
    for( size_t rowIndex = 0; rowIndex < 3; ++rowIndex )
    {
        _rotation( rowIndex, 0 ) = _x1( rowIndex );
        _rotation( rowIndex, 1 ) = _y1( rowIndex );
        _rotation( rowIndex, 2 ) = _z1( rowIndex );
    }
    #endif
    
    if ( ! _rotation.getInverse( _rotation_inverse ) )
    {
        throw exception( "cannot compute inverse of matrix", SPROCESS_HERE );
    }
   
    nb      = _projected_nbs.begin();
    nb_end  = _projected_nbs.end();
    
    for( ; nb != nb_end; ++nb )
    {
        const vec3& nb_ = *nb;
        vec3 new_nb( _rotation_inverse * ( nb_ - position ) );
        _transformed_proj_nbs.push_back( new_nb );
        //std::cout << "new: " << new_nb << std::endl;
    }
    
    size_t max_k = 5;
    typename std::vector< vec3 >::iterator it, it_end;
    
    it      = _transformed_proj_nbs.begin();
    it_end  = _transformed_proj_nbs.end();
    
    // build matrix for llsq
    for( size_t i = 0; it != it_end && i < max_k; ++it, ++i )
    {
        const float_t& x = (*it).x();
        _A( i, 0 )  = x;
        _A( i, 1 )  = x * x;
        _B( i )     = (*it).y();
    }
    
    _lapack_llsq.compute( _A, _B, _X );
    
    _a = _X( 0 );
    _b = _X( 1 );
    	   
    // compute normal curvature
    _kr = -2.0 * _b / pow( 1 + _a * _a, 1.5 );
    
    return _kr;
}



template< typename float_t >
void
curvature< float_t >::get_a_and_b( float_t& a, float_t& b )
{
    a = _a;
    b = _b;
}



template< typename float_t >
const std::vector<  vmml::vector< 3, float_t >  >&
curvature< float_t >::get_projected_nbs() const
{
    return _projected_nbs;
}



template< typename float_t >
const std::vector<  vmml::vector< 3, float_t >  >&
curvature< float_t >::get_transformed_projected_nbs() const
{
    return _transformed_proj_nbs;
}



template< typename float_t >
void
curvature< float_t >::_reset( size_t neighbor_count )
{

    _a  = 0.0;
    _b  = 0.0;
    _kr = 0.0;

    _projected_nbs.clear();
    _projected_nbs.reserve( neighbor_count );

    _transformed_proj_nbs.clear();
    _transformed_proj_nbs.reserve( neighbor_count );

}


template< typename float_t >
#ifdef SPROCESS_OLD_VMMLIB
const vmml::Vector4< float_t >&
#else
const vmml::vector< 4, float_t >&
#endif
curvature< float_t >::get_normal_plane() const
{
    return _normal_plane;
}


template< typename float_t >
void
curvature< float_t >::set_tolerance( float_t tolerance_ )
{
    _tolerance = tolerance_;
}


template< typename float_t >
float_t
curvature< float_t >::get_tolerance()
{
    return _tolerance;
}



} // namespace stream_process

#endif

