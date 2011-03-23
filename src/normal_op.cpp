/*
 *  normal_operator.cpp
 *  StreamProcessing
 *
 *  Created by Renato Pajarola on Wed Jul 14 2004.
 *  Copyright (c) 2004 UC Irvine. All rights reserved.
 *
 */
#include "normal_op.h"

#include "VMMLibIncludes.h"
#include <vmmlib/svd.h>


#include <algorithm>

#define MAX_COUNT 8

namespace stream_process
{

double **normal_op::a = NULL, **normal_op::v = NULL;

/** 
* normal estimation operator
*
* normal_op reads the following inputs from the stream:
*   "position"  - point position                - vec3f          
*   "neighbors" - distance/ptr pair of knn      - neighbor[k]   
*
* normal_op writes the following outputs into the stream:
*   "normal"    - normal vector                 - vec3f 
*   "radius"    - bounding sphere radius        - float
*   "tmp_covar" - covariance matrix             - Matrix4d
*
*/
    
normal_op::normal_op() 
    : stream_op()
    , _tmp_covar(   "tmp_covar" )
    , _normal(      "normal" )
    , _radius(      "radius" )
    , _position(    "position" )
    , _neighbors(   "neighbors" )
    , _point_index( "point_index" )
    , _max_neighbors( MAX_COUNT )
{

    set_name( "normal" );

    // FIXME 
    if ( !a ) 
    {
        a = (double**)calloc(3, sizeof(double*));
        for ( size_t  i = 0; i < 3; ++i )
            a[i] = (double*)calloc(3, sizeof(double));
    }
    if ( !v ) 
    {
        v = (double**)calloc(3, sizeof(double*));
        for ( size_t i = 0; i < 3; ++i )
            v[i] = (double*)calloc(3, sizeof(double));
    }
}



void 
normal_op::pull_push()
{
  stream_point* point;
  // pull elements from previous operator
    while ( point = prev->front() ) 
    {
        assert( _factory->is_valid_ptr( point ) );
        prev->pop_front();
        // initialize point  self-covariance
        //tensorProd(tmp->covar, tmp->v, tmp->v);		// currently done in neighborhood as here it is too late
	    // compute splat normal
        compute_normal( point );
        
        #ifdef STREAM_PROCESS_NORMAL_DEBUG
        _normal_debug_helper.write_debug_info_new( point );
        #endif
        
        //vec3f n = point->get( _normal );
        _fifo.push_back( point );
    }
}



stream_point* 
normal_op::front()
{
    return ( _fifo.empty() ) ? 0 : _fifo.front();
}



void 
normal_op::pop_front()
{
    assert( ! _fifo.empty() );
    {
        _fifo.pop_front();
    }
}



void 
normal_op::compute_normal( stream_point* point )
{
    size_t max_k = std::min( _max_neighbors, (size_t)MAX_COUNT );

    size_t k;
    float f, var, density; // mean;

    // get mean and variance of squared distance
    /* mean = 0.0;
    for (k = 0; k < max_k; k++)
        mean += point->dist[k];
    mean /= (float)max_k; */

    // get sampling density
    //density = M_PI * point->dist[max_k-1] / max_k;
    neighbor* neighbors = point->get_ptr( _neighbors );
    
    const float dist = neighbors[ max_k - 1 ].get_distance();
    //const float dist = _nbDistances.get( point, max_k - 1 );
    density = M_PI * dist / max_k;
    var = density;		// density is already a squared length

    // moving least squares weighted covariance
    //point->tmp_covar = mat4d::ZERO;  //emptyMatrix(point->tmp_covar);

    mat4d& tmp_covar = point->get( _tmp_covar );
    tmp_covar = mat4d::ZERO;
    
    const vec3f& pos = point->get( _position );
    
    neighbor* current_nb    = neighbors;
    neighbor* neighbors_end = neighbors + max_k;
    
    vec3f N;
    vec3d Nd;
    mat4d C;
    for(  ; current_nb != neighbors_end; ++current_nb )
    {
        const vec3f& nb_pos = current_nb->get_point()->get( _position );
        
        N = pos - nb_pos; 
        Nd = N;
        C.tensor( Nd, Nd ); 
         
        // Gaussian weight
        f = exp(-0.5 * current_nb->get_distance() / var); 
        tmp_covar += C * f;
    
    }
    
    /*
    for (k = 0; k < max_k; ++k ) 
    {
        // get covariance
        //N = point->v - point->list[k]->v; // sub(N, point->v, point->list[k]->v);
        const vec3f& nbPos = _position.get( neighbours[k] ); 
  
        N = pos - nbPos; 
        vec3d Nd( N.xyz );
        C.tensor( Nd, Nd );   // tensorProd3(C, N, N);
	    //f = exp(-0.5 * point->dist[k] / var);		// Gaussian weight
        const float& distance = _nbDistances.get( point, k );
 
        f = exp(-0.5 * distance / var);		// Gaussian weight
        //point->tmp_covar += C * f;//addScaledMatrix(point->tmp_covar, f, C);
        tmpCovar += C * f;
    }
    */

  // get normal orientation from covariance analysis
  //for (k = 0; k < 3; k++)
	//N[k] = point->n[k];
    //test = getNormalSVD(point->n, point->tmp_covar);

    vec3f& normal = point->get( _normal ); 
    bool test = get_normal_svd( normal, tmp_covar );

    if ( !test ) 
    {
        // find alternate normal
        normal = 0.0;
        
        for ( k = 0; k < max_k-1; ++k ) 
        {
            const vec3f& nb_pos = neighbors[ k ].get_point()->get( _position ); 
            const vec3f& nb_pos2 = neighbors[ k+1 ].get_point()->get( _position );
            // average from triangles
            #ifdef SPROCESS_OLD_VMMLIB
            N.normal( pos, nb_pos, nb_pos2 );
            #else
            N.computeNormal( pos, nb_pos, nb_pos2 );
            #endif
            normal += N;
        }
        normal.normalize();
    }
    // flip normal inside/outside according to input data
    //if (hasnormal)
    //  if (dotProd(point->n, N) < 0.0)
    //    scale(point->n, -1.0);

    float& radius = point->get( _radius );
    radius = 0.0f; 
    // point->size = 0.0;
    for ( k = 0; k < max_k; ++k )
    {
        const float& distance = neighbors[ k ].get_distance();
        if ( distance > radius ) // point->dist[k] > point->size)
        {
            //point->size = point->dist[k];
            radius = distance;
        }
    }
    radius = sqrt( radius );
    //point->size = sqrt(point->size);
}


bool normal_op::get_normal_svd( vec3f& normal, const mat4d& cov )
{
    size_t i, k, l1, l2, l3;
    vec3d v1, v2;
    vec3d d;
  
    // copy matrix and perform SVD
    for (i = 0; i < 3; i++)
        for (k = 0; k < 3; k++)
            #ifdef SPROCESS_OLD_VMMLIB
            a[i][k] = cov.m[i][k]; // FIXME ? .m[]
            #else
            a[ i ][ k ] = cov( k, i );
            #endif
    // perform singular value decomposition
    vmml::svdecompose( a, i, k, d.array, v );
  
    // order singular values
    for (i = 0; i < 3; i++)
        d[i] = fabs(d[i]);
    l1 = 0; l2 = 1; l3 = 2;
    if (d[l1] > d[l2]) 
    {
        k = l1;
        l1 = l2;
        l2 = k;
    }
    if (d[l2] > d[l3]) 
    {
        if (d[l1] > d[l3]) 
        {
            k = l3;
            l3 = l2;
            l2 = l1;
            l1 = k;
        } 
        else
        {
            k = l2;
            l2 = l3;
            l3 = k;
        }
    }
    assert( d[l1] <= d[l2] && d[l2] <= d[l3] );
  
    // check for degeneracy of solutions
    k = 0;
    for (i = 0; i < 3; i++) 
    {
        if ( fabs( d[i] ) < 0.0000000001 )
            ++k;
    }
    if (k > 1) 
    {
        // two singular values zero => points form a line
        std::cerr << "normal_op::getNormalSVD - Warning - multiple zero singular values found. " <<std::endl;
        return false;
  /*} else if (fabs(d[l1]) > 0.0) {
    // get normal to fitting plane (direction of smallest singular value)
    for (i = 0; i < 3; i++) {
      v1[i] = a[i][l1];		// normal axis
    }
	normalize(v1);
	normal[0] = v1[0];
	normal[1] = v1[1];
	normal[2] = v1[2];
	
    return true;*/
    } 
    else 
    {
    // get normal to fitting plane (cross-product of largest pair)
        for ( i = 0; i < 3; ++i) 
        {
            v1[i] = a[i][l2];		// minor ellipse axis
            v2[i] = a[i][l3];		// major ellipse axis
        }
        v1.normalize();
        v2.normalize();
        //FIXME vecProd(normal, v1, v2);
        normal[0] = v1[1] * v2[2] - v1[2] * v2[1];
        normal[1] = v1[2] * v2[0] - v1[0] * v2[2];
        normal[2] = v1[0] * v2[1] - v1[1] * v2[0];
	
        //normalize(normal);
        normal.normalize();


        int FIXME = 0;

        return true;
    }
}


bool normal_op::get_normal( vec3f& normal, const mat4d& cov )
{
    bool done;
    size_t l1, l2, l3;
    vec3f v1, v2;
    vec3d d;
    mat3d a, v;

    // copy matrix and do Jacobi rotations
    for ( size_t i = 0; i < 3; ++i )
    {
        for ( size_t k = 0; k < 3; ++k )
        {
            #ifdef SPROCESS_OLD_VMMLIB
            a.m[i][k] = cov.m[i][k];
            #else
            a( k, i ) = cov( k, i );
            #endif
        }
    }
#ifndef NDEBUG
    assert( a.isPositiveDefinite() );
#endif
    size_t k; // jacobi rotationCount
    done = vmml::solveJacobi3x3< double >( a, d, v, k );
    assert( done );

    // order eigenvalues
    for ( size_t i = 0; i < 3; ++i )
        d[i] = fabs(d[i]);
    l1 = 0; l2 = 1; l3 = 2;
    if (d[l1] > d[l2]) 
    {
        std::swap( l1, l2 );
    }
    if (d[l2] > d[l3]) 
    {
        if (d[l1] > d[l3]) 
        {
            k = l3;
            l3 = l2;
            l2 = l1;
            l1 = k;
        } 
        else 
        {
            k = l2;
            l2 = l3;
            l3 = k;
        }
    }
    assert( d[l1] <= d[l2] && d[l2] <= d[l3] );

    // check for degeneracy of solutions
    k = 0;
    for ( size_t i = 0; i < 3; ++i ) 
    {
        if ( fabs( d[i] ) < 0.0000000001 )
            k++;
    }
    if ( k > 1 ) 
    {
        // two eigenvalues zero => points form a line
        fprintf(stderr, "Warning: multiple zero eigenvalues found!\n");
        return false;
    } 
    else 
    {
        // get normal to fitting plane (direction of smallest eigenvalue; or cross-product of largest pair)
        for ( size_t i = 0; i < 3; ++i ) 
        {
            // note: was v.m[ i ][ l2 ] for old vmmlib 
            v1[i] = v.at( l2, i );		// minor ellipse axis 
            // note: was v.m[ i ][ l3 ] for old vmmlib 
            v2[i] = v.at( l3, i );		// major ellipse axis
        }
        normal.cross( v1, v2 );
        //vecProd(normal, v1, v2);
        //normalize(normal);
        normal.normalize();
        return true;
    }
}



void
normal_op::setup_stage_0()
{
    // -- required inputs --
    _require( _position );
    _require( _neighbors );
    
    // -- auxiliary variables -- 
    _reserve_array( _tmp_covar, SP_FLOAT_64, 16 );
    
    // -- outputs --
    _reserve_array( _normal, SP_FLOAT_32, 3, IO_WRITE_TO_OUTPUT );
    
    //rt_struct_member_info& info = rt_struct_user::get_attribute_info( "normal" );
    //info.set_setting( "transform_back", "true" );
    
    _reserve( _radius, IO_WRITE_TO_OUTPUT );
}



void 
normal_op::setup_stage_1()
{}


void 
normal_op::setup_stage_2()
{
    var_map::iterator it = _config->find( "nb-count" );
    if( it != _config->end() )
        _max_neighbors = (*it).second.as< size_t >();
    _max_neighbors = std::min( _max_neighbors, (size_t)MAX_COUNT );
    
    #ifdef STREAM_PROCESS_NORMAL_DEBUG
    _normal_debug_helper.setup_new(
        _position,
        _normal,
        _point_index,
        _neighbors
        );
    #endif
    
}

} // namespace stream_process
