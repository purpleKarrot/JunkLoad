#include "active_set.h"

#include "stats_op.h"
#include "chain_operators.hpp"

namespace stream_process
{
active_set::active_set()
    : _chain_operators( 0 )
    , _tree_op( 0 )
    , _stats_op( 0 )
{}


size_t 
active_set::get_size() const
{
    assert( _chain_operators );
    return _chain_operators->get_active_set_size();
}



tree_op_shared_data&
active_set::get_shared_data()
{
    assert( _tree_op );
    return _tree_op->get_shared_data();
}



const tree_op_shared_data&
active_set::get_shared_data() const
{
    assert( _tree_op );
    return _tree_op->get_shared_data();
}



pr_kd_tree_node&
active_set::get_root_node()
{
    assert( _tree_op );
    return _tree_op->get_root_node();
}


} // namespace stream_process
