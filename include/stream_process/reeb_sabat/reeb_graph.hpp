#ifndef __STREAM_PROCESS__REEB_GRAPH__HPP__
#define __STREAM_PROCESS__REEB_GRAPH__HPP__

#include <iostream>
#include <list>
#include <fstream>

#include <stream_process/reeb_link.hpp>
#include <stream_process/reeb_node.hpp>


namespace stream_process
{

class reeb_node;
class reeb_link;
	
class reeb_graph
{
public:

	reeb_graph(){}
	void print_graph();
	void print_graph_for_tulip ();

	~reeb_graph();

	void add_node(reeb_node *node);
	void add_link(reeb_link *link);
	void delete_node(reeb_node *node); 
	void delete_link(reeb_link *link);

/*	void add_node(reeb_node *node){
		nodes.push_front (node);
		node->set_graph_it (nodes.begin());
	}	
	
	void add_link(reeb_link * link){
		links.push_front (link);
		link->set_graph_it (links.begin());
	}	

	void delete_node(reeb_node *node) {
		nodes.erase ( node -> get_graph_it() ); 
	}

	void delete_link(reeb_link *link) {
		links.erase ( link -> get_graph_it() ); 
	}
*/		
protected:
	std::list< reeb_node* > nodes;
	std::list< reeb_link* > links;
}; // class reeb_graph

} // namespace stream_process

#endif

