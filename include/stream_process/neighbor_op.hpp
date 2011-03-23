#ifndef __VMML__NEIGHBOR_OP__H__
#define __VMML__NEIGHBOR_OP__H__

#include "active_set_op.h"
#include "stream_op.h"
#include "neighbor.h"

#include "pr_kd_tree_node.h"
#include "knn_search_visitor.h"
#include "index_queue.h"
#include "tree_op_shared_data.hpp"

namespace stream_process
{

class neighbor_op : public stream_op
{
public:
    typedef std::deque< stream_point* > stream_point_queue;

    neighbor_op();
    virtual ~neighbor_op();
    
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
    void            _activate_next_point();
    
    void _DEBUG_print_neighbors( stream_point* point ) const;
    void _DEBUG_check_neighbors( stream_point* point ) const;
    
    rt_struct_member< vec3f >           _position;
    rt_struct_member< uint32_t >        _point_index;
    rt_struct_member< neighbor >        _neighbors;
    rt_struct_member< uint32_t >        _nb_count;
    rt_struct_member< uint32_t >        _min_ref_index;
    rt_struct_member< uint32_t >        _max_ref_index;
       
    // in and out buffers 
    stream_point_queue  _in_queue;
    stream_point_queue  _out_queue;

    // we store the points in op ourselves to prevent possibly slow 
    // size() queries
    size_t  _points_in_op;

    // members for current query point data
    stream_point*   _query_point;
    vec3f           _query_position;
    neighbor*       _query_neighbors;
    
    index_queue*    _index_queue;

    // for safe_range, z_max...
    tree_op_shared_data*    _tree_op_shared_data;

    // functors for neighbor comparison
    pair_compare_first_adapter< float, stream_point*, std::greater< float > >
        _pair_greater;
    pair_compare_first_adapter< float, stream_point*, std::less< float > >
        _pair_less;

};



inline size_t 
neighbor_op::smallest_element()
{
    if ( _query_point )
        return _query_point->get( _point_index );
    else if ( ! _in_queue.empty() )
        return _in_queue.back()->get( _point_index );
    else
        return prev->smallest_element();
}



inline size_t 
neighbor_op::smallest_reference()
{
    if ( ! _index_queue->empty() )
    {
        return std::min(  
            static_cast< size_t >( _index_queue->top().first ), 
            stream_op::smallest_reference() 
            );
    }
    else
        return stream_op::smallest_reference();
}



inline size_t 
neighbor_op::size() const
{
    return _points_in_op;
}



// neighbor op options 
struct neighbor_op_options : public op_options
{
    neighbor_op_options();
};


} // namespace stream_process

#endif

