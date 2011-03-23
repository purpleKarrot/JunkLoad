#ifndef __STREAM_PROCESS__REEB_LINK_SA__HPP__
#define __STREAM_PROCESS__REEB_LINK_SA__HPP__

#include <stream_process/stream_process_types.hpp>

namespace stream_process
{

template< typename sp_types_t >
class reeb_node_sa;

template< typename sp_types_t >
class reeb_link_element_sa;


template< typename sp_types_t >
class reeb_link_sa
{
public:
    STREAM_PROCESS_TYPES
    
    typedef reeb_link_sa< sp_types_t >              reeb_link;
    typedef reeb_node_sa< sp_types_t >              reeb_node;
    typedef reeb_link_element_sa< sp_types_t >      reeb_link_element;

	reeb_link_sa (reeb_node *, reeb_node*);

	void duplicate (reeb_link *);
	void smart_copy (reeb_link *);
	reeb_link_element* add_element (reeb_node*, reeb_link_element*, reeb_link_element*);
	reeb_link* split_link (reeb_node*);
	void split_duplicate (reeb_link *);
	void erase_all_ref ();
	void clean_element (reeb_link_element *);
	
	
	reeb_node** get_reeb_node(){
		return nodes;
	}

	void set_reeb_node(reeb_node **nodes){
		this->nodes[0] = nodes[0];
		this->nodes[1] = nodes[1];
	}
	
	void increment_element_count(int increment){
		element_count += increment;
	}

	int get_element_count(){
		return element_count;
	}

	std::list <reeb_link_element> * get_points(){
		return &pts;
	}

	typename std::list <reeb_link *> :: iterator get_graph_it(){
		return graph_it;
	}

	void set_graph_it (typename std::list <reeb_link *> :: iterator graph_it){
		this->graph_it = graph_it;
	}

protected:
	reeb_node *nodes[2];
	std::list <reeb_link_element> pts;
	int element_count;
	typename std::list <reeb_link *> :: iterator graph_it;

}; // class reeb_link_sa

} // namespace stream_process

#endif

