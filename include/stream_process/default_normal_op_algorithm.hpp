#ifndef __STREAM_PROCESS__DEFAULT_NORMAL_OP_ALGORITHM__HPP__
#define __STREAM_PROCESS__DEFAULT_NORMAL_OP_ALGORITHM__HPP__

#include "op_algorithm.hpp"
#include "rt_struct_member.h"
#include "neighbor.h"
#include "VMMLibIncludes.h"

namespace stream_process
{

class default_normal_op_algorithm : public op_algorithm
{
public:
    default_normal_op_algorithm();
    default_normal_op_algorithm( const default_normal_op_algorithm& orig );
    virtual ~default_normal_op_algorithm();

    virtual op_algorithm* clone(){ return new default_normal_op_algorithm( *this ); };

    virtual void setup_stage_0();
    virtual void setup_stage_2();

protected:
	virtual void _compute( stream_point* point );
    
    bool get_normal( vec3f& normal, const mat4d& cov);
    bool get_normal_svd( vec3f& normal, const mat4d& cov);

    // normal_op creates these members
    rt_struct_member< mat4d >           _tmp_covar;
    rt_struct_member< vec3f >           _normal;
    rt_struct_member< float >           _radius;
    // normal_op uses these members
    rt_struct_member< vec3f >           _position;
    rt_struct_member< neighbor >        _neighbors;
    
    size_t                              _max_neighbors;
    double **a, **v;

}; // class default_normal_op_algorithm

} // namespace stream_process

#endif

