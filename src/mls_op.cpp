/*
 *  mls_operator.cpp
 *  StreamProcessing
 *
 *  Created by Renato Pajarola on Thu Sep 02 2004.
 *  Copyright (c) 2004 UC Irvine. All rights reserved.
 *
 */

#include "mls_op.hpp"

#include <algorithm>


/**
 *
 * @brief MLS surface extraction operator
 *
 * mls_op reads the following inputs from the stream:
 *   "position"          - point position                    - vec3f
 *   "normal"            - normal vector                     - vec3f
 *   "min_ref_index"     - neighbour with smallest index     - uint32_t
 *   "max_ref_index"     - neighbour with largest index      - uint32_t
 *   "point_index"       - point index in stream             - uint32_t
 *   "mls_support"       - MLS blending kernel radius        - float
 *
 * mls_op writes the following outputs into the stream:
 *   "…"                 - …                                 - mat4d
 *
 */

namespace stream_process
{
  
  mls_op::mls_op() 
  : stream_op()
  , _position(        "position" )
  , _normal(          "normal" )
  , _min_ref_index(   "min_ref_index" )
  , _max_ref_index(   "max_ref_index" )
  , _point_index(     "point_index" )
  , _mls_support(     "mls_support" )
  {
    set_name( "mls_surface" );
  }
  
  
  void mls_op::pull_push()
  {
    stream_point* point;
	
    // pull elements from previous operator
    while ( ( point = prev->front() ) ) 
    {
	  prev->pop_front();
	  
	  // enter waiting queue
	  _in_queue.push_back( point );
    }
	
    // check queue elements
    while ( ! _in_queue.empty() ) 
    {
	  point = _in_queue.front();
	  
	  // only process element if all neighbor normals are present
	  // NOT NECESSARY FOR MLS EXTRACTION AS WE DO NOT GO THROUGH NEIGHBORS, RIGHT?
	  const uint32_t max_ref_index = point->get( _max_ref_index );
	  
	  if ( max_ref_index < prev->smallest_element() )
	  {
		_in_queue.pop_front();
		// perform MLS surface extraction

		// transfer to queue
		_out_queue.push_back( point );
	  } 
	  else
		break;
    }
  }
  
  
  stream_point* 
  mls_op::front()
  {
    return ( _out_queue.empty() ) ? 0 : _out_queue.front();
  }
  
  
  void 
  mls_op::pop_front()
  {
    if ( ! _out_queue.empty() ) 
    {
	  _out_queue.pop_front(); 
    }
  }
  
  
  void 
  mls_op::clear_stage()
  {
    while ( ! _in_queue.empty() ) 
    {
	  stream_point* point = _in_queue.front();
	  _in_queue.pop_front();
	  // perform MLS surface extraction
	  
	  _out_queue.push_back( point );
    }
  }
  
  
  size_t 
  mls_op::smallest_element()
  {
    if ( ! _out_queue.empty() )
    {
	  return  _out_queue.front()->get( _point_index ); 
    }
    else if ( ! _in_queue.empty() )
    {
	  return _in_queue.front()->get( _point_index ); 
    }
    else
	  return prev->smallest_element();
  }
  
  
  void
  mls_op::setup_stage_0()
  {
    // -- required inputs --
    _require( _position );
    _require( _normal );
    _require( _min_ref_index );
    _require( _min_ref_index );
    _require( _point_index );
    _require( _mls_support );
    
    // -- auxiliary variables -- 
    //_reserve_array( _curvature, SP_FLOAT_64, 16 );
    
    // -- outputs --
    //_reserve_array( _scales, SP_FLOAT_32, 3, IO_WRITE_TO_OUTPUT );
  }
  
  
  void 
  mls_op::setup_stage_1()
  {}
  
  
  void 
  mls_op::setup_stage_2()
  {}
  
} // namespace stream_process
