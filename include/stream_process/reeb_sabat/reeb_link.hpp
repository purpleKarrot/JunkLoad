#ifndef __STREAM_PROCESS__REEB_LINK__HPP__
#define __STREAM_PROCESS__REEB_LINK__HPP__

#include <list>
#include "reeb_node.hpp"
#include "reeb_graph_op_algorithm.hpp"
#include "reeb_link_element.hpp"

using namespace std;

namespace stream_process
{

class reeb_node;
class reeb_link_element;

class reeb_link
{
public:
	reeb_link (reeb_node *, reeb_node*);
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

	list <reeb_link_element> * get_points(){
		return &pts;
	}

	list <reeb_link *> :: iterator get_graph_it(){
		return graph_it;
	}

	void set_graph_it (list <reeb_link *> :: iterator graph_it){
		this->graph_it = graph_it;
	}

protected:
	reeb_node *nodes[2];
	list <reeb_link_element> pts;
	int element_count;
	list <reeb_link *> :: iterator graph_it;
}; // class reeb_link

} // namespace stream_process

#endif

