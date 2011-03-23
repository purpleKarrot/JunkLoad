#ifndef __STREAM_PROCESS__REEB_GRAPH_SA__HPP__
#define __STREAM_PROCESS__REEB_GRAPH_SA__HPP__

#include <stream_process/reeb_node_sa.hpp>
#include <stream_process/reeb_link_sa.hpp>
#include <stream_process/reeb_link_element_sa.hpp>

namespace stream_process
{

template< typename sp_types_t >
class reeb_graph_sa
{
public:
    STREAM_PROCESS_TYPES
    
    typedef reeb_node_sa< sp_types_t >             reeb_node;
    typedef reeb_link_sa< sp_types_t >             reeb_link;
    typedef reeb_link_element_sa< sp_types_t >     reeb_link_element;

protected:

}; // class reeb_graph_sa

} // namespace stream_process

#endif

