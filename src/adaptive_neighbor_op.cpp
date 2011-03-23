/*
 *  adaptive_neighbor_op.cpp
 *  StreamProcessing Renato
 *
 *  Created by Renato Pajarola on 16.04.08.
 *  Copyright 2008 University of Zurich. All rights reserved.
 *
 */

#include "adaptive_neighbor_op.hpp"

#include "VMMLibIncludes.h"

#include <algorithm>

#define MAX_COUNT 32
#define DENSITY   0.02

namespace stream_process
{

adaptive_neighbor_op::adaptive_neighbor_op() 
: stream_op()
, _has_radius( false )
, _density( DENSITY )
, _adaptive_range( "adaptive_range" )
, _mls_support(    "mls_support" )
, _position(       "position" )
, _normal(         "normal" )
, _radius(         "radius" )
, _neighbors(      "neighbors" )
, _nb_count(       "nb_count" )
, _min_ref_index(  "min_ref_index" )
, _max_ref_index(  "max_ref_index" )
{
  set_name("adaptive_neighbor");
}


void 
adaptive_neighbor_op::pull_push()
{  
  stream_point* point;
  
  // pull elements from previous operator
  while ( ( point = prev->front() ) ) 
  {
	prev->pop_front();
	// compute adaptive neighborhood values
	adaptive_neighborhood( point );
	_fifo.push_back( point );
  }
}


stream_point* 
adaptive_neighbor_op::front()
{
  return ( _fifo.empty() ) ? 0 : _fifo.front();
}


void 
adaptive_neighbor_op::pop_front()
{
  if ( ! _fifo.empty() )
  {
	_fifo.pop_front();
  }
}


void 
adaptive_neighbor_op::adaptive_neighborhood( stream_point* point )
{
  int i, nb_count;

  // initialize variable references
  neighbor *neighbors = point->get_ptr( _neighbors );
  nb_count = point->get( _nb_count );
  unsigned int &range = point->get( _adaptive_range );
  float &support = point->get( _mls_support );
  
  
  // adjust neighborhood for constant density
  i = 3;
  while (i < nb_count && (float)i / (M_PI * neighbors[i-1].get_distance()) > _density) {
	i++;
  }
  range = i;
  
  // compute isotropic support range for MLS point set surface
  float boundary_points = 2.0*sqrt(M_PI) * sqrt(nb_count) - M_PI;

  const float& dist_to_farthest_nb = neighbors[nb_count-1].get_distance();

  support = 2.0 * sqrt( dist_to_farthest_nb / (nb_count - 0.5*boundary_points));
  
  float &radius = point->get( _radius );
  if (!_has_radius) {
	//radius = sqrt(neighbors[range-1].get_distance());
	radius = support;
	//radius = sqrt(neighbors[7].get_distance());
  }

  // test purpose: compute density at local point
  //local density = (float)range / (M_PI * neighbors[range-1].get_distance());  
}


void
adaptive_neighbor_op::setup_stage_0()
{
  _require( _position );
  _require( _normal );
  _require( _neighbors );
  _require( _min_ref_index );
  _require( _max_ref_index );
  
  _reserve( _adaptive_range, IO_WRITE_TO_OUTPUT );
  _reserve( _mls_support, IO_WRITE_TO_OUTPUT );
  
  _has_radius = _factory->has_member( "radius" );
  if ( !_has_radius )
    _reserve( _radius, IO_WRITE_TO_OUTPUT );

  // we need at least 32 neighbors for this.
  var_map::iterator it = _config->find( "nb-count" );
  size_t max_neighbors = 0;
  if( it != _config->end() )
	max_neighbors = (*it).second.as< size_t >();
  else
	throw exception( "nb-count config value not found.", SPROCESS_HERE );
  if ( max_neighbors < MAX_COUNT ) 
	(*it).second.as< size_t >() = MAX_COUNT;    
}

void 
adaptive_neighbor_op::setup_stage_1()
{}

void 
adaptive_neighbor_op::setup_stage_2()
{}

} // namespace stream_process
