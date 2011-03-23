/*
 *  splat_operator2.cpp
 *  StreamProcessing
 *
 *  Created by Renato Pajarola on Sat Sep 11 2004.
 *  Copyright (c) 2004 UC Irvine. All rights reserved.
 *
 */

#include "splat_op2.h"

namespace stream_process
{

/* splat operator
*
* see also splat_op
* 
* splat_op2 reads the following inputs from the stream:
*   "curv"              - curvature matrix                      - mat4d
*   "scales"            - scales vector                         - vec3f
*
* splat_op2 writes the following outputs into the stream:
*   "axis"              - major principal semiaxis orientation  - mat4d
*   "length"            - major principal semiaxis length       - vec3f
*   "ratio"             - semiaxis aspect ratio                 - vec3f
*/

splat_op2::splat_op2()
    : splat_op()
    , _scales(      "scales" )
    , _curvature(   "curv" )
{
    set_name( "splat2" );
}

void splat_op2::compute_splat( stream_point* point )
{  
    // use principal curvature directions as ellipse axis

    // second and third singular curvature values are used (first is along normal)
    // NOT REALLY! These scales are more extreme than as computed in splat_operator1
    float& length           = point->get( _length );
    float& ratio            = point->get( _ratio ); 
    const vec3f& scales     = point->get( _scales );
    const vec3f& pos        = point->get( _position );
    const mat4d& curv       = point->get( _curvature ); 
    const uint32_t nb_count = point->get( _nb_count );
    neighbor* neighbors     = point->get_ptr( _neighbors );
          
    length  = scales[1];
    ratio   = scales[2] / scales[1];
    if ( ratio < 0.5f )
        ratio = 0.5f;

    // adjust elliptical splat extent
    //a = point->length * point->length;
    float a = length * length;
    float b = a * ratio * ratio;
    float max = 0.0f;

    neighbor* cur_nb    = neighbors;
    neighbor* nb_end    = neighbors + nb_count;
    vec3f vec, curv1, curv2;
    float x,y,f;
    for( ; cur_nb != nb_end; ++cur_nb )
    {
        // project neighbor into tangential ellipse plane
        // note that larger curvature curv[1] is shorter ellipse axis
        const vec3f& nb_pos = cur_nb->get_point()->get( _position );
    
        vec = nb_pos - pos;
        
        curv1.set( curv( 0,1 ), curv( 1,1 ), curv( 2,1 ) );
        curv2.set( curv( 0,2 ), curv( 1,2 ), curv( 2,2 ) );
       
        x = dot( vec, curv2 );
        y = dot( vec, curv1 );
        // test point in ellipse equation
        f = x * x / a + y * y / b;

        // get largest value f
        if ( f > max )
            max = f;
    }

    // scale major ellipse axis length
    max = sqrt( max );
    length *= max;
    
    vec3f& axis = _axis.get( point );

    // note that larger curvature curv[1] is shorter ellipse axis
    axis.set( curv( 0,2 ), curv( 1,2 ), curv( 2,2 ) );
}



} // namespace stream_process
