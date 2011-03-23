#ifndef __VMML__STATS_OP__H__
#define __VMML__STATS_OP__H__

#include "active_set_op.h"
#include "rt_struct_member.h"

namespace stream_process
{
class stats_op : public active_set_op
{
public:
    stats_op();
    virtual ~stats_op();
    
    virtual void insert( stream_point* point );
    virtual void remove( stream_point* point );
    
    virtual void setup_stage_0();
    virtual void setup_stage_2();
    
    virtual void prepare_processing();
    virtual void prepare_shutdown();
    
    virtual void print_end_stats();
    
protected:
    rt_struct_member< double >   _lifetime;
    rt_struct_member< uint32_t > _point_index;

    double _start_time;
    double _avg_time;
    uint32_t _largest_index;
    uint32_t _max_active_set_size;
    uint64_t _total_point_count;
    
}; // class stats_op


} // namespace stream_process

#endif

