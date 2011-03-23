#ifndef _MLS_Op_H_
#define _MLS_Op_H_

#include "stream_op.h"
#include "rt_struct_member.h"
#include "neighbor.h"

#include <deque>

namespace stream_process
{
  
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
   * @author renato pajarola
   * @author fabio marton
   *
   */
  
  template< typename T > class rt_struct_member;
  
  class mls_op 
    : public stream_op 
	{
	public:
	  mls_op();
	  
	  virtual void pull_push();
	  virtual stream_point* front();
	  virtual void pop_front();
	  virtual void clear_stage();
	  
	  virtual size_t smallest_element();
	  
	  virtual size_t size() const { return _in_queue.size() + _out_queue.size(); };
	  
	  virtual void setup_stage_0();
	  virtual void setup_stage_1();
	  virtual void setup_stage_2();
	  
	protected:  

	  std::deque< stream_point* > _in_queue;
	  std::deque< stream_point* > _out_queue;
	  	  
	  // MLS op creates these members
	  
	  // MLS op uses these members
	  rt_struct_member< uint32_t >        _min_ref_index;
	  rt_struct_member< uint32_t >        _max_ref_index;
	  rt_struct_member< uint32_t >        _point_index;
	  rt_struct_member< vec3f >           _normal;
	  rt_struct_member< vec3f >           _position;
	  rt_struct_member< float >           _mls_support;
	};
  
  
} // namespace stream_process

#endif

