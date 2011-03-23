#ifndef __STREAM_PROCESS__CURV_SPLAT_OP_ALGORITHM__HPP__
#define __STREAM_PROCESS__CURV_SPLAT_OP_ALGORITHM__HPP__

#include <stream_process/curvature_estimation_op_algorithm.hpp>
#include <stream_process/rt_struct_member.h>
#include <stream_process/data_types.hpp>

#include <stream_process/neighbor.h>

namespace stream_process
{

// hp_float_t is used for precision-sensitive operations
// to prevent numeric errors when using single-precision floats as float_t

template< typename float_t = float, typename hp_float_t = double >
class curv_splat_op_algorithm
    : public curvature_estimation_op_algorithm< float_t, hp_float_t >
{
public:
    typedef curvature_estimation_op_algorithm< float_t, hp_float_t > super;
    
    typedef vmml::vector< 3, float_t >          vec3;
    typedef vmml::vector< 3, hp_float_t >       vec3hp;

    typedef vmml::matrix< 4, 4, hp_float_t >    mat4hp;

    curv_splat_op_algorithm();

    virtual op_algorithm* clone();
    
    virtual void setup_stage_0();
    
    virtual std::string about() const;
    
    virtual bool does_provide( const std::string& name, data_type_id type_ );

protected:
	virtual void _compute( stream_point* point );
	virtual void _estimate_splat( stream_point* point );

    // created attributes
    rt_struct_member< vec3  >           _ellipse_axis;
    rt_struct_member< float_t >         _length; // of the ellipse axis
    rt_struct_member< float_t >         _ratio; // of the two ellipse axes
  
    vec3    _diff;
    vec3    _curv1;
    vec3    _curv2;

}; // class curv_splat_op_algorithm


template< typename float_t, typename hp_float_t >
curv_splat_op_algorithm< float_t, hp_float_t >::
curv_splat_op_algorithm()
    : super()
    , _ellipse_axis(    "axis" )
    , _length(          "length" )
    , _ratio(           "ratio" )
{
    op_algorithm::set_name( "curvature and splat estimation" );
}


template< typename float_t, typename hp_float_t >
void
curv_splat_op_algorithm< float_t, hp_float_t >::
_compute( stream_point* point )
{
    super::_compute( point );

    _estimate_splat( point );

}



template< typename float_t, typename hp_float_t >
void
curv_splat_op_algorithm< float_t, hp_float_t >::
_estimate_splat( stream_point* point )
{
    // use principal curvature directions as ellipse axis

    // second and third singular curvature values are used (first is along normal)
    // NOT REALLY! These scales are more extreme than as computed in splat_operator1
    const vec3& position    = point->get( super::_position );
    const vec3& scales      = point->get( super::_scales );
    const mat4hp& curv      = point->get( super::_curvature ); 

    vec3& axis              = point->get( _ellipse_axis );
    float_t& length         = point->get( _length );
    float_t& ratio          = point->get( _ratio ); 

    neighbor* neighbors     = point->get_ptr( super::_neighbors );
          
    length  = scales[1];
    ratio   = scales[2] / scales[1];

    assert( ! std::isnan( ratio ) && ! std::isinf( ratio ) );

    // clamp ratio 
    if ( ratio < 0.5f )
        ratio = 0.5f;

    // adjust elliptical splat extent
    const float_t a = length * length;
    const float_t b = a * ratio * ratio;

    float_t max = 0.0;

    float_t x,y,f;

    neighbor* cur_nb    = neighbors;
    neighbor* nb_end    = neighbors + super::_max_neighbors;
    for( ; cur_nb != nb_end; ++cur_nb )
    {
        // project neighbor into tangential ellipse plane
        // note that larger curvature curv[1] is shorter ellipse axis
        const vec3f& nb_pos = cur_nb->get_point()->get( super::_position );
    
        _diff = nb_pos - position;
        
        _curv1.set( curv( 0,1 ), curv( 1,1 ), curv( 2,1 ) );
        _curv2.set( curv( 0,2 ), curv( 1,2 ), curv( 2,2 ) );
       
        x = dot( _diff, _curv2 );
        y = dot( _diff, _curv1 );

        // test point in ellipse equation
        f = x * x / a + y * y / b;

        // get largest value f
        if ( f > max )
            max = f;
    }

    // scale major ellipse axis length
    max = vmml::details::getSquareRoot< float_t >( max );

    length *= max;
    
    // note that larger curvature curv[1] is shorter ellipse axis
    axis.set( curv( 0,2 ), curv( 1,2 ), curv( 2,2 ) );

}



template< typename float_t, typename hp_float_t >
void
curv_splat_op_algorithm< float_t, hp_float_t >::
setup_stage_0()
{
    super::setup_stage_0();

    // -- output variables -- 
    get_data_type_id_from_type< float_t > get_float_type;
    _reserve_array( _ellipse_axis, get_float_type(), 3, IO_WRITE_TO_OUTPUT );

    _reserve( _length, IO_WRITE_TO_OUTPUT );
    _reserve( _ratio, IO_WRITE_TO_OUTPUT );

}



template< typename float_t, typename hp_float_t >
std::string
curv_splat_op_algorithm< float_t, hp_float_t >::
about() const
{
    return "estimates curvature, and elliptic splat extents based on local curvature";
}



template< typename float_t, typename hp_float_t >
op_algorithm*
curv_splat_op_algorithm< float_t, hp_float_t >::clone()
{
    return new curv_splat_op_algorithm< float_t, hp_float_t >( *this );
}




template< typename float_t, typename hp_float_t >
bool
curv_splat_op_algorithm< float_t, hp_float_t >::
does_provide( const std::string& name, data_type_id type_ )
{
    if ( super::does_provide( name, type_ ) )
        return true;

    if ( name ==  _ellipse_axis.get_name() )
        return true;
    if ( name == _ratio.get_name() )
        return true;
    if ( name == _length.get_name() )
        return true;
	return false;
}

} // namespace stream_process

#endif
