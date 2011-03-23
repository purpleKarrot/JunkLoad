#ifndef __STREAM_PROCESS__NORMAL_COMPARE_OP_ALGORITHM__HPP__
#define __STREAM_PROCESS__NORMAL_COMPARE_OP_ALGORITHM__HPP__

#include "op_algorithm.hpp"
#include "rt_struct_member.h"

namespace stream_process
{

class normal_compare_op_algorithm : public op_algorithm
{
public:
    normal_compare_op_algorithm();

    virtual op_algorithm* clone(){ return new normal_compare_op_algorithm( *this ); };


protected:
	virtual void _compute( stream_point* point );
    
    rt_struct_member< vec3f >   _normal_0;
    rt_struct_member< vec3f >   _normal_1;
    rt_struct_member< float >   _radius_0;
    rt_struct_member< float >   _radius_1;
    
    
}; // class normal_compare_op_algorithm

} // namespace stream_process

#endif

