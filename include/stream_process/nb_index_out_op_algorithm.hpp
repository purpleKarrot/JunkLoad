#ifndef __STREAM_PROCESS__NB_INDEX_OUT_OP_ALGORITHM__HPP__
#define __STREAM_PROCESS__NB_INDEX_OUT_OP_ALGORITHM__HPP__

#include "op_algorithm.hpp"
#include "stream_point.h"
#include "rt_struct_member.h"
#include "neighbor.h"

namespace stream_process
{

class nb_index_out_op_algorithm : public op_algorithm
{
public:
    nb_index_out_op_algorithm();
    virtual ~nb_index_out_op_algorithm();

    virtual op_algorithm* clone(){ return new nb_index_out_op_algorithm( *this ); };

    virtual void setup_stage_1();

protected:
	virtual void _compute( stream_point* point );

    // nb_index_out_op_algorithm creates these members
    rt_struct_member< uint32_t >        _nb_indices;
    // nb_index_out_op_algorithm uses these members
    rt_struct_member< neighbor >        _neighbors;
    rt_struct_member< uint32_t >        _nb_count;
    rt_struct_member< uint32_t >        _point_index;

    uint32_t                            _max_neighbors;
    
}; // class nb_index_out_op_algorithm

} // namespace stream_process

#endif

