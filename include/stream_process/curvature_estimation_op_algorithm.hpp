#ifndef __STREAM_PROCESS__CURVATURE_ESTIMATION_OP_ALGORITHM__HPP__
#define __STREAM_PROCESS__CURVATURE_ESTIMATION_OP_ALGORITHM__HPP__

#include <stream_process/VMMLibIncludes.h>

#include <stream_process/op_algorithm.hpp>
#include <stream_process/rt_struct.h>
#include <stream_process/neighbor.h>

#include <stream_process/svd.hpp>
#include <stream_process/normal_space_covariance.hpp>

namespace stream_process
{

// hp_float_t is used for precision-sensitive operations
// to prevent numeric errors when using single-precision floats as float_t

template<
    typename float_t        = float,
    typename hp_float_t     = double
    >
class curvature_estimation_op_algorithm : public op_algorithm
{
public:
    typedef vmml::vector< 3, float_t >          vec3;
    typedef vmml::vector< 3, hp_float_t >       vec3hp;
    typedef vmml::vector< 3, size_t >           vec3s;

    typedef vmml::vector< 4, float_t >          vec4;
    typedef vmml::vector< 4, hp_float_t >       vec4hp;

    typedef vmml::matrix< 3, 3, hp_float_t >    mat3hp;
    typedef vmml::matrix< 4, 4, hp_float_t >    mat4hp;

    curvature_estimation_op_algorithm();
    virtual ~curvature_estimation_op_algorithm();

    virtual op_algorithm* clone();
    
    virtual void setup_stage_0();
    virtual void setup_stage_2();
    
    virtual bool does_provide( const std::string& name, data_type_id type_ );
    
    virtual std::string about() const;

protected:
    virtual void _compute( stream_point* point );
    
    virtual void _get_curvature_and_scales_from_svd( stream_point* point );
       
    // max neighbors available from spatial data structure op
    size_t                              _nb_count;

    // max neighbors used for curvature computation
    size_t                              _max_neighbors;

    // curv op creates these members
    // normalized basis vectors after svd, svd's U
    rt_struct_member< mat4hp >              _curvature;

    // the singular values of the svd decomposition, svd's sigma
    rt_struct_member< vec3 >                _scales;
 
    // curv op uses these members
    rt_struct_member< neighbor >            _neighbors;
    rt_struct_member< vec3 >                _normal;
    rt_struct_member< vec3 >                _position;
    
    svd< hp_float_t >                       _svd;
    
    normal_space_covariance< float_t, hp_float_t > _ns_covar_compute;

    mat4hp                                  _ns_covariance;
    mat4hp                                  _ns_covar_tmp;
   
    vec4hp                                  _tangent_plane;
    hp_float_t                              _weight;

    vec3hp                                  _row;

}; // class curvature_estimation_op_algorithm



template< typename float_t, typename hp_float_t >
curvature_estimation_op_algorithm< float_t, hp_float_t >::
curvature_estimation_op_algorithm()
    : op_algorithm()
    , _curvature( "curv" )
    , _scales( "scales" )
    , _neighbors( "neighbors" )
    , _normal( "normal" )
    , _position( "position" )
    , _ns_covar_compute( _position, _normal, _neighbors )
{
    set_name( "curvature estimation" );
}



template< typename float_t, typename hp_float_t >
curvature_estimation_op_algorithm< float_t, hp_float_t >::
~curvature_estimation_op_algorithm()
{}



template< typename float_t, typename hp_float_t >
void
curvature_estimation_op_algorithm< float_t, hp_float_t >::
_compute( stream_point* point )
{
    _ns_covariance = _ns_covar_compute.compute( point );
    
    _svd.compute_3x3( _ns_covariance );

    _get_curvature_and_scales_from_svd( point );
}




template< typename float_t, typename hp_float_t >
void
curvature_estimation_op_algorithm< float_t, hp_float_t >::
_get_curvature_and_scales_from_svd( stream_point* point )
{
    vec3& scales        = point->get( _scales );

    const mat3hp& u     = _svd.get_u();
    const vec3hp& sigma = _svd.get_sigma();
    const vec3s& order  = _svd.get_order();
    
    const size_t& l1    = order.x();
    const size_t& l2    = order.y();
    const size_t& l3    = order.z();

    // copy ordered singular values into scales vector
    scales.set( sigma[ l3 ], sigma[ l2 ], sigma[ l1 ] );
    
    mat4hp& curvature    = point->get( _curvature );
    curvature.zero();

    for( size_t index = 0; index < 3; ++index )
    {
        _row.set( u( index, l3 ), u( index, l2 ), u( index, l1 ) );
        _row.normalize();
        
        for( size_t j = 0; j < 3; ++j )
        {
            curvature( index, j ) = _row( j ); 
        }
    }
}



template< typename float_t, typename hp_float_t >
op_algorithm*
curvature_estimation_op_algorithm< float_t, hp_float_t >::
clone()
{
    return new curvature_estimation_op_algorithm< float_t, hp_float_t >();
}



template< typename float_t, typename hp_float_t >
void
curvature_estimation_op_algorithm< float_t, hp_float_t >::
setup_stage_0()
{
    // -- required inputs --
    _require( _position );
    _require( _normal );
    _require( _neighbors );
    
    // -- auxiliary variables -- 
    get_data_type_id_from_type< float_t > get_float_type;
    _reserve_array( _scales, get_float_type(), 3, IO_WRITE_TO_OUTPUT );

    get_data_type_id_from_type< hp_float_t > get_hp_float_type;
    _reserve_array( _curvature, get_hp_float_type(), 16, IO_WRITE_TO_OUTPUT );
}



template< typename float_t, typename hp_float_t >
void
curvature_estimation_op_algorithm< float_t, hp_float_t >::
setup_stage_2()
{
    var_map::iterator it = _config->find( "nb-count" );
    if( it != _config->end() )
        _nb_count = (*it).second.as< size_t >();
    else
    {
        throw exception( "nb-count is required.", SPROCESS_HERE );
    }

    // clamp neighbors // FIXME have a look at it again
    _max_neighbors = std::min( _nb_count, (size_t) 8 );
}



template< typename float_t, typename hp_float_t >
std::string
curvature_estimation_op_algorithm< float_t, hp_float_t >::
about() const
{
    return "estimates the local curvatures using the normal space mls covariance.";
}



template< typename float_t, typename hp_float_t >
bool
curvature_estimation_op_algorithm< float_t, hp_float_t >::
does_provide( const std::string& name, data_type_id type_ )
{
    if ( name == _curvature.get_name() )
        return true;
    else if ( name == _scales.get_name() )
        return true;
	return false;
}



} // namespace stream_process

#endif

