/*
 *  splat_operator.cpp
 *  StreamProcessing
 *
 *  Created by Renato Pajarola on Wed Jul 14 2004.
 *  Copyright (c) 2004 UC Irvine. All rights reserved.
 *
 */

#include "splat_op.h"
#include "EigenvalueSolver.h"

#define MAX_COUNT 8

//#define SPROCESS_SPLAT_OP_PROJECT_ELLIPSE_AXIS

namespace stream_process
{
/*
* splat operator
*
* splat_op reads the following inputs from the stream:
*   "position"          - point position                        - vec3f          
*   "normal"            - normal vector                         - vec3f 
*   "neighbors"         - distance/ptr pair of knn              - neighbor[k]
*   "tmp_covar"         - covariance matrix                     - mat4d
*
* splat_op writes the following outputs into the stream:
*   "axis"              - major principal semiaxis orientation  - mat4d
*   "length"            - major principal semiaxis length       - float
*   "ratio"             - semiaxis aspect ratio                 - float
*
*/

splat_op::splat_op()
    : stream_op()
    , _axis(        "axis" )
    , _length(      "length" )
    , _ratio(       "ratio" )
    , _normal(      "normal" )
    , _tmp_covar(   "tmp_covar" )
    , _neighbors(   "neighbors" )
    , _nb_count(    "nb_count" )
    , _position(    "position" )
{
    set_name( "splat" );
}



void 
splat_op::pull_push()
{
    stream_point* point;
    
    // pull elements from previous operator
    while ( ( point = prev->front() ) ) 
    {
        prev->pop_front();
        // compute elliptical disk
        compute_splat( point );
        _fifo.push_back( point );
    }
}



stream_point* 
splat_op::front()
{
    return ( _fifo.empty() ) ?  0 : _fifo.front();
}



void 
splat_op::pop_front()
{
    if ( ! _fifo.empty() )
        _fifo.pop_front();
}



size_t 
splat_op::size() const
{
    return _fifo.size();
}



void 
splat_op::compute_splat( stream_point* point )
{
    //static int max_k = MIN(ONE_RING, MAX_K);
    float a, b, f, max;
    mat4d R( mat4d::ZERO ), RT;
    
    const vec3f& normal = point->get( _normal );
    
    // generate tangential coordinate system, z-axis equal to n

    vec3f m;
    if ( normal[0] == 1.0 || normal[1] == 0.0 && normal[2] == 0.0) 
        m.set( 0.0, 0.0, 1.0 );
    else 
    {
        m.set( 0.0, -normal.z(), normal.y() );
        m.normalize();
    }
    
    vec3f l;
    l.cross( m, normal ); //vecProd(l, m, point->n);

    // get coordinate system rotation and its transpose 
    R.set(
        l.x(), l.y(), l.z(), 0.0, 
        m.x(), m.y(), m.z(), 0.0,
        normal.x(), normal.y(), normal.z(), 0.0, 
        0.0, 0.0, 0.0, 1.0 
    );

    R.transposeTo( RT );
    
    mat4d& tmp_covar    = point->get( _tmp_covar );
    float& length       = point->get( _length );
    float& ratio        = point->get( _ratio );
    
    // apply coordinate system transformation to covariance matrix and scale
    mat4d tmp( tmp_covar * RT ); //  multMatrix(tmp, point->tmp_covar, RT);
    tmp_covar = R * tmp; // multMatrix(point->tmp_covar, R, tmp);

    mat3f Axis;
    // major ellipse axis, length and aspect ratio from covariance analysis
    solveEigenvalues2D( Axis, length, ratio, R, tmp_covar );
    
    // clamp ratio
    if ( ratio < 0.5f )
        ratio = 0.5f;
    if ( ratio > 1.0f )
        ratio = 1.0f;
        
    
    Axis = Axis.getTransposed();
    // adjust elliptical splat extent
    a = length * length;
    b = a * ratio * ratio;
    max = 0.0;

    const vec3f& pos        = point->get( _position );
    const uint32_t nb_count = point->get( _nb_count );

    uint32_t used_nb_count = std::min( nb_count, (uint32_t)MAX_COUNT );

    neighbor* current_nb    = point->get_ptr( _neighbors ); 
    neighbor* neighbors_end = current_nb + used_nb_count;
    
    vec3f diff; 
    float x,y;
    for( ; current_nb != neighbors_end; ++current_nb )
    {
        const vec3f& nb_pos = current_nb->get_point()->get( _position );
        // project neighbor into tangential ellipse plane
        diff = nb_pos - pos;
        
        x = diff.dot( Axis.getRow(0) );    // x = dotProd(vec, Axis[0]);
        y = diff.dot( Axis.getRow(1) );    // y = dotProd(vec, Axis[1]);

        // test point in ellipse equation
        f = x * x / a + y * y / b;

        // get largest value f
        if ( f > max )
            max = f;
    }

    /*
    for ( k = 0; k < _maxNeighbours; ++k ) //? nb count?
    {
        const vec3f& nbPos = _position << neighbours[k];
        // project neighbor into tangential ellipse plane
        vec3f vec = nbPos - pos;
        
        vec3f vvv( Axis.m[0] ); // FIXME ? .m[]
        x = vec.dot( vvv ); // x = dotProd(vec, Axis[0]);
        vvv.set( Axis.m[1] ); // FIXME ? .m[]
        y = vec.dot( vvv ); // y = dotProd(vec, Axis[1]);

        // test point in ellipse equation
        f = x * x / a + y * y / b;

        // get largest value f
        if (f > max)
            max = f;
    }
    */
    // scale major ellipse axis length
    max = sqrt( max );
    length = length * max;

    vec3f& axis = point->get( _axis );
    axis = Axis.getRow(0);
    
    //for ( k = 0; k < 3; ++k )
    //    axis[k] = Axis.m[0][k]; // FIXME ? .m[]
    
    #ifdef SPROCESS_SPLAT_OP_PROJECT_ELLIPSE_AXIS
    _compute_proj_axis( point );
    #endif
}



#ifdef SPROCESS_SPLAT_OP_PROJECT_ELLIPSE_AXIS
void
splat_op::_compute_proj_axis( stream_point* point )
{
    const vec3f& position   = point->get( _position );
    const vec3f& normal     = point->get( _normal );
    vec3f& axis             = point->get( _axis );

    vec3f axis_proj;//        = point->get( _axis_proj );

    vec4f tangent_plane( normal.x(), normal.y(), normal.z(), -normal.dot( position ) );
    
    vec3f ae = position + axis;
    axis_proj = tangent_plane.projectPointOntoPlane( ae );
    axis_proj -= position;
    axis_proj.normalize(); 
    
    axis = axis_proj;
}
#endif


void
splat_op::setup_stage_0()
{
    // -- required inputs --
    _require( _position );
    _require( _normal );
    _require( _tmp_covar );
    _require( _neighbors ); 
    
    // -- outputs --
    #ifdef SPROCESS_OLD_VMMLIB
    _reserve_vmmlib_type( _axis, IO_WRITE_TO_OUTPUT );
    #else
    _reserve_array( _axis, SP_FLOAT_32, 3, IO_WRITE_TO_OUTPUT );
    #endif
    
    _reserve( _length,  IO_WRITE_TO_OUTPUT );
    _reserve( _ratio,   IO_WRITE_TO_OUTPUT );
}


// post construction initialisation
void splat_op::setup_stage_1()
{}


void 
splat_op::setup_stage_2()
{}


} // namespace stream_process
