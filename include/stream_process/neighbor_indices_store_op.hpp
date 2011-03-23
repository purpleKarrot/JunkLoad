#ifndef __VMML__NEIGHBOR_INDICES_STORE_OP__H__
#define __VMML__NEIGHBOR_INDICES_STORE_OP__H__

#include "active_set_op.h"
#include "stream_op.h"
#include "neighbor.h"

/**
*
*   this operator is mainly for debug purposes. it stores the point indices
*   of all the neighbors in the knn.
*
*/

namespace stream_process
{

class neighbor_indices_store_op : public stream_op
{
public:
    typedef std::deque< stream_point* > stream_point_queue;

    neighbor_indices_store_op();
    virtual ~neighbor_indices_store_op();
    
    // -- stream op interface --
    virtual void pull_push();
    virtual stream_point* front();

    virtual void pop_front();
    virtual void clear_stage();
    
    // smallest unprocessed element in this or previous stages
    inline virtual size_t smallest_element();	
    // smallest reference of any unprocessed element in this or previous stages
    inline virtual size_t smallest_reference();	
    inline virtual size_t size() const;
    
    virtual void setup_stage_0();
    virtual void setup_stage_1();
    virtual void setup_stage_2();
         
protected:
    void _write_indices( stream_point* point );

    rt_struct_member< uint32_t >        _point_index;
    rt_struct_member< neighbor >        _neighbors;
    rt_struct_member< uint32_t >        _nb_count;
    rt_struct_member< uint32_t >        _nb_indices;
       
    // in and out buffers 
    stream_point_queue  _in_queue;
    
    size_t _points_in_op;

};



inline size_t 
neighbor_indices_store_op::smallest_element()
{
    if ( ! _in_queue.empty() )
        return _in_queue.back()->get( _point_index );
    else
        return prev->smallest_element();
}



inline size_t 
neighbor_indices_store_op::smallest_reference()
{
    return stream_op::smallest_reference();
}



inline size_t 
neighbor_indices_store_op::size() const
{
    return _points_in_op;
}


} // namespace stream_process

#endif

