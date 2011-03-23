#ifndef __STREAM_PROCESS__TREE_OP_SHARED_DATA__HPP__
#define __STREAM_PROCESS__TREE_OP_SHARED_DATA__HPP__

#include <limits>
#include "index_queue.h"

namespace stream_process
{

struct tree_op_shared_data
{
    tree_op_shared_data() 
        : z_max( std::numeric_limits< float >::min() )
        , query_z( std::numeric_limits< float >::min() )
        , safe_range( std::numeric_limits< float >::max() )
        , do_search( false )
    {}
       
    inline void compute_safe_range();
    inline float get_safe_range() const;

    float   z_max;
    float   query_z;
    float   safe_range;
    bool    do_search;
    
    index_queue     index_queue_;

}; // struct tree_op_shared_data



inline void 
tree_op_shared_data::compute_safe_range()
{
    safe_range = z_max - query_z;
    // all the distances are squared for performance, so...
    safe_range *= safe_range;
}



inline float 
tree_op_shared_data::get_safe_range() const
{
    return safe_range;
}


} // namespace stream_process

#endif

