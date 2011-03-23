#ifndef __VMML_ACTIVE_SET_H__
#define __VMML_ACTIVE_SET_H__

#include "stream_point.h"

#include "pr_kd_tree_node.h"

#include "tree_op.hpp"
#include "tree_op_shared_data.hpp"
#include "stats_op.h"

namespace stream_process
{

class chain_operators;
class active_set
{
public:
    active_set();
    virtual ~active_set() {};

    size_t get_size() const;

    tree_op_shared_data& get_shared_data();
    const tree_op_shared_data& get_shared_data() const;
    
    pr_kd_tree_node& get_root_node();
    
protected:
    friend class chain_manager;
    
    chain_operators*                _chain_operators;
    size_t _points_in_active_set;
    tree_op< pr_kd_tree_node >*     _tree_op;
    stats_op*                       _stats_op;
};


} // namespace stream_process

#endif
