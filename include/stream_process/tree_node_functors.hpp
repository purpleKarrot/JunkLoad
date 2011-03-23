#ifndef __STREAM_PROCESS__TREE_NODE_FUNCTORS__HPP__
#define __STREAM_PROCESS__TREE_NODE_FUNCTORS__HPP__

//#include "rt_struct_member.h"
//#include "stream_point.h"

namespace stream_process
{
#if 0
struct extract_stream_point_position
{
	inline const vec3f& operator()( const stream_point* p ) const
	{
		return p->get( position );
	}
	
	static void set_position( const rt_struct_member< vec3f >& position_ )
	{
		position = position_;
	}
	
	static rt_struct_member< vec3f > position;
};

rt_struct_member< vec3f > extract_stream_point_position::position;

#endif

struct trivial_extract_position
{
	inline const vec3f& operator()( const vec3f& p ) const
	{
		return p;
	}
};


} // namespace stream_process

#endif

