/*
 *  curvature_operator.cpp
 *  StreamProcessing
 *
 *  Created by Renato Pajarola on Thu Sep 02 2004.
 *  Copyright (c) 2004 UC Irvine. All rights reserved.
 *
 */

#include "curvature_op.h"

#include <vmmlib/vmmlib.hpp>
// FIXME
#include <vmmlib/svd.h>

#include <algorithm>

#define DIMENSION 3
#define MAX_COUNT 8u

/**
*
* @brief curvature estimation operator
*
* curvature_op reads the following inputs from the stream:
*   "position"          - point position                    - vec3f          
*   "normal"            - normal vector                     - vec3f 
*   "neighbors"         - distance/ptr pair of knn          - neighbor[k]
*   "min_ref_index"     - neighbour with smallest index     - uint32_t
*   "max_ref_index"     - neighbour with largest index      - uint32_t
*   "point_index"       - point index in stream             - uint32_t          
*
* curvature_op writes the following outputs into the stream:
*   "curv"              - curvature matrix                  - mat4d
*   "scales"            - scales vector                     - vec3f
*
*/

namespace stream_process
{

double **curvature_op::a = NULL, **curvature_op::v = NULL;

curvature_op::curvature_op() 
    : stream_op()
    , _curvature(       "curv" )
    , _scales(          "scales" )
    , _min_ref_index(   "min_ref_index" )
    , _max_ref_index(   "max_ref_index" )
    , _point_index(     "point_index" )
    , _neighbors(       "neighbors" )
    , _normal(          "normal" )
    , _position(        "position" )
    , _points_in_op( 0 )
{
    set_name( "curvature" );

    // FIXME 
    if ( !a ) 
    {
        a = (double**)calloc(DIMENSION, sizeof(double*));
        for (int i=0; i<DIMENSION; i++)
            a[i] = (double*)calloc(DIMENSION, sizeof(double));
    }
    if ( !v ) 
    {
        v = (double**)calloc(DIMENSION, sizeof(double*));
        for (int i=0; i<DIMENSION; i++)
            v[i] = (double*)calloc(DIMENSION, sizeof(double));
    }
}


void curvature_op::pull_push()
{
    stream_point* point;
    IndexReference zref;
    ExtraRef eref;

    // pull elements from previous operator
    while ( ( point = prev->front() ) ) 
    {
        prev->pop_front();

        // update priority queue that maintains smallest referenced index in all buffers
        zref.first = point->get( _min_ref_index ); // smallest referenced index
        zref.second = point->get( _point_index ); // from element
        ZQ.push( zref );
	
        // enter waiting queue
        _fifo1.push_back( point );
        ++_points_in_op;
    }
  
    // check queue elements
    while ( ! _fifo1.empty() ) 
    {
        point = _fifo1.front();
        
        // only process element if all neighbor normals are present
        const uint32_t max_ref_index = point->get( _max_ref_index );
        
        if ( max_ref_index < prev->smallest_element() )
        //stream_op::smallest_element() ) 
        {
            _fifo1.pop_front();
            // compute curvature directions
            compute_curvature( point );
            // transfer to queue
            _fifo2.push_back( point );
        } 
        else
            break;
    }
}



stream_point* 
curvature_op::front()
{
    return ( _fifo2.empty() ) ? 0 : _fifo2.front();
}



void 
curvature_op::pop_front()
{
    if ( ! _fifo2.empty() ) 
    {
    // remove unused references
	/* check for ZQ size growing too much
	if (ZQ.size() > 100000) {
	  fprintf(stderr, "\n current _fifo1 front %d and _fifo2 %d\n", _fifo1.front()->index, _fifo2.front()->index);
	  fprintf(stderr, " smallest reference %d from %d\n", ZQ.top().first, ZQ.top().second);
	  exit(2);
	}
	*/
    const uint32_t index = _fifo2.front()->get( _point_index ); 

    while ( ! ZQ.empty() )
    {
        if ( ZQ.top().second < index )
        {
            ZQ.pop();
        }
        else
            break;
    }
    _fifo2.pop_front(); 
    --_points_in_op;
    /*
    while ( ! ZQ.empty() && ZQ.top().second < _fifo2.front()->index )
        ZQ.pop();
    _fifo2.pop_front();
    */
    }

}



void 
curvature_op::clear_stage()
{
    while ( ! _fifo1.empty() ) 
    {
        stream_point* point = _fifo1.front();
        _fifo1.pop_front();
	    // perform fairing
        compute_curvature( point );
        _fifo2.push_back( point );
    }
}



size_t 
curvature_op::smallest_element()
{
    if ( ! _fifo2.empty() )
    {
        return  _fifo2.front()->get( _point_index ); 
    }
    else if ( ! _fifo1.empty() )
    {
        return _fifo1.front()->get( _point_index ); 
    }
    else
        return prev->smallest_element();
        //stream_op::smallest_element();
}



size_t 
curvature_op::smallest_reference()
{
    if ( ! ZQ.empty() )
        return std::min( (size_t) ZQ.top().first, stream_op::smallest_reference());
    else
        return stream_op::smallest_reference();
}



size_t 
curvature_op::size() const
{
    return _points_in_op;
    //return _fifo1.size() + _fifo2.size();
}



void 
curvature_op::compute_curvature( stream_point* point )
{
    vec4f vec;
    mat4d C( mat4d::ZERO );
    mat4d tmp;
  
      // get mean and variance of squared distance
      /* mean = 0.0;
      for (k = 0; k < _maxNeighbours; k++)
        mean += point->dist[k];
      mean /= (float)_maxNeighbours; */
  
    // get sampling density
    neighbor* neighbors = point->get_ptr( _neighbors );
    
    //float density = M_PI * distance[_maxNeighbours-1] / _maxNeighbours;
    float density 
        = M_PI * neighbors[ _max_neighbors - 1 ].get_distance() / _max_neighbors;
    
    float var = density; // density is already a squared length
  
    // moving least squares weighted normal quadric
    float weight = 0;
    float f;
    stream_point* nb_point;
    vec4d vecd;
    for ( size_t k = 0; k < _max_neighbors; ++k ) 
    {
        // get normal space covariance
        nb_point = neighbors[k].get_point();
        const vec3f& nb_normal  = nb_point->get( _normal );
        const vec3f& nb_pos     = nb_point->get( _position ); 
        
        vec.set( nb_normal, nb_pos.dot( nb_normal ) );       
        vecd = vec;
        tmp.tensor( vecd, vecd );
        f = exp(-0.5 * neighbors[k].get_distance() / var ); // Gaussian weight
        weight += f;
        C += tmp * f;
    }
    C *= _max_neighbors / weight; //  scaleMatrix(C, _maxNeighbours / weight); 
    
   
    // get curvature directions from covariance analysis
    get_curvature_svd( point, C );
}



void 
curvature_op::get_curvature_svd( stream_point* point, const mat4d& quadric )
//Matrix3f& curvature, vec3f& scales, const mat4d& quadric)
{
    mat4d& curvature = point->get( _curvature ); 
    vec3f& scales    = point->get( _scales ); 
        
    size_t l1, l2, l3;
    vec4d d;
    d.w() = 0.0f;
  
    // copy matrix and perform SVD
    for ( size_t i = 0; i < DIMENSION; ++i )
        for ( size_t k = 0; k < DIMENSION; ++k )
            #ifdef SPROCESS_OLD_VMMLIB
            a[i][k] = quadric.m[i][k]; // FIXME ? quadric[i][k]
            #else
            a[i][k] = quadric( k,i ); 
            #endif
  
    // perform singular value decomposition
    //svdcmp( a, DIMENSION, DIMENSION, d.array, v );

    vmml::svdecompose( a, DIMENSION, DIMENSION, d.array, v );
    
    
    // order singular values
    for ( size_t i = 0; i < 3; i++)
        d[i] = fabs(d[i]);
    l1 = 0; l2 = 1; l3 = 2;
    if (d[l1] > d[l2]) 
        std::swap( l1, l2 );

    if ( d[l2] > d[l3] ) 
    {
        if (d[l1] > d[l3]) 
        {
            size_t k = l3;
            l3 = l2;
            l2 = l1;
            l1 = k;
        } 
        else 
            std::swap( l2, l3 );
    }
    
    assert( d[l1] <= d[l2] && d[l2] <= d[l3] );
    scales[0] = d[l3]; 
    scales[1] = d[l2]; 
    scales[2] = d[l1];

    for ( size_t i = 0; i < 3; ++i ) 
    {
        #ifdef SPROCESS_OLD_VMMLIB
        curvature.m[0][i] = a[i][l3]; // FIXME ? curvature[0][i]
        curvature.m[1][i] = a[i][l2];
        curvature.m[2][i] = a[i][l1];
        #else
        curvature( i,0 ) = a[i][l3]; // FIXME ? curvature[0][i]
        curvature( i,1 ) = a[i][l2];
        curvature( i,2 ) = a[i][l1];
        #endif
    }

    vec3f vn;
    for ( size_t i = 0; i < 3; ++i )
    {
        //normalise( curvature[i] );
        #ifdef SPROCESS_OLD_VMMLIB
        vec3f* aaa = ( vec3f* ) curvature.m[i]; // FIXME ? .m[]
        aaa->normalize();        
        #else
        vn.set( curvature( i, 0 ), curvature( i,1 ), curvature( i,2 ) );
        vn.normalize();
        curvature( i,0 ) = vn( 0 );
        curvature( i,1 ) = vn( 1 );
        curvature( i,2 ) = vn( 2 );
        #endif
    }
        
}


void
curvature_op::setup_stage_0()
{
    
    // -- required inputs --
    _require( _position );
    _require( _normal );
    _require( _neighbors );
    _require( _min_ref_index );
    _require( _min_ref_index );
    _require( _point_index );
    
    // -- auxiliary variables -- 
    _reserve_array( _curvature, SP_FLOAT_64, 16 );
    
    // -- outputs --
    _reserve_array( _scales, SP_FLOAT_32, 3, IO_WRITE_TO_OUTPUT );
    
}



void 
curvature_op::setup_stage_1()
{}



void 
curvature_op::setup_stage_2()
{
    var_map::iterator it = _config->find( "nb-count" );
    if( it != _config->end() )
        _max_neighbors = (*it).second.as< size_t >();
    _max_neighbors = std::min( _max_neighbors, MAX_COUNT );

}


} // namespace stream_process
