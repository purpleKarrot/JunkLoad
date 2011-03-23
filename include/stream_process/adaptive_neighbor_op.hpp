/*
 *  adaptive_neighbor_op.h
 *  StreamProcessing Renato
 *
 *  Created by Renato Pajarola on 16.04.08.
 *  Copyright 2008 University of Zurich. All rights reserved.
 *
 */

#ifndef _Adaptive_Neighbor_Op_H_
#define _Adaptive_Neighbor_Op_H_

#include "stream_op.h"

#include <deque>
#include "rt_struct_member.h"
#include "neighbor.h"

namespace stream_process
{

class adaptive_neighbor_op : public stream_op
  {
  public:
	adaptive_neighbor_op();
	
	virtual void pull_push();
    virtual stream_point* front();
    virtual void pop_front();
	
    virtual size_t size() const { return _fifo.size(); };

    virtual void setup_stage_0();
    virtual void setup_stage_1();
    virtual void setup_stage_2();
	
  protected:
	bool _has_radius;
	float _density;						// density threshold

	std::deque< stream_point* > _fifo;

	virtual void adaptive_neighborhood( stream_point* point );
	
    // normal_op creates these members
    rt_struct_member< uint32_t >        _adaptive_range;
	rt_struct_member< float >           _mls_support;
	
	// adaptive_neighbor_op uses these members
    rt_struct_member< vec3f >           _position;
    rt_struct_member< vec3f >           _normal;
    rt_struct_member< float >           _radius;
    rt_struct_member< neighbor >        _neighbors;
	rt_struct_member< uint32_t >        _nb_count;
    rt_struct_member< uint32_t >        _min_ref_index;
    rt_struct_member< uint32_t >        _max_ref_index;
	
  };
  
} // namespace stream_process

#endif
