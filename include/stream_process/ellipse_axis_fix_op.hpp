#ifndef __STREAM_PROCESS__ELLIPSE_AXIS_FIX_OP__HPP__
#define __STREAM_PROCESS__ELLIPSE_AXIS_FIX_OP__HPP__

#include "op_algorithm.hpp"
#include "stream_point.h"
#include "rt_struct_member.h"

namespace stream_process
{

template< typename float_t >
class ellipse_axis_fix_op : public op_algorithm
{
public:
    typedef vmml::vector< 3, float_t >  vec3;
    typedef vmml::vector< 4, float_t >  vec4;

    ellipse_axis_fix_op();
    virtual ~ellipse_axis_fix_op();

    virtual op_algorithm* clone();
    
    
    virtual void setup_stage_2(); // for dependecy checks

protected:
	virtual void _compute( stream_point* point );

    rt_struct_member< vec3 >    _position;
    rt_struct_member< vec3 >    _normal;
    rt_struct_member< vec3 >    _ellipse_axis;

    vec3                        _tmp_point;
    vec4                        _tangent_plane;

}; // class ellipse_axis_fix_op


template< typename float_t >
ellipse_axis_fix_op< float_t >::ellipse_axis_fix_op()
    : _position(        "position"  )
    , _normal(          "normal"    )
    , _ellipse_axis(    "axis"      )
{
    set_name( "ellipse axis fix" );
}



template< typename float_t >
ellipse_axis_fix_op< float_t >::~ellipse_axis_fix_op()
{}



template< typename float_t >
void
ellipse_axis_fix_op< float_t >::_compute( stream_point* point )
{
    const vec3& position    = _position.get( point );
    
    vec3& normal            = _normal.get( point );
    vec3& ellipse_axis      = _ellipse_axis.get( point );
    
    normal.normalize();
    ellipse_axis.normalize();

    _tangent_plane.set(
        normal.x(),
        normal.y(),
        normal.z(),
        -dot( normal, position )
        );
    
    _tmp_point = position + ellipse_axis;

    ellipse_axis = _tangent_plane.projectPointOntoPlane( _tmp_point );
    ellipse_axis -= position;
    ellipse_axis.normalize(); 
}



template< typename float_t >
void
ellipse_axis_fix_op< float_t >::setup_stage_2()
{
    _require( _position );
    _require( _normal );
    _require( _ellipse_axis );
}



template< typename float_t >
op_algorithm*
ellipse_axis_fix_op< float_t >::clone()
{
    return new ellipse_axis_fix_op( *this );
}



} // namespace stream_process

#endif

