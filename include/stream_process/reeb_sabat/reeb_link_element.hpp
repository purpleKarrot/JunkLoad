#ifndef __STREAM_PROCESS__REEB_LINK_ELEMENT__HPP__
#define __STREAM_PROCESS__REEB_LINK_ELEMENT__HPP__

#include "reeb_link.hpp"
#include "reeb_node.hpp"
#include "reeb_graph_op_algorithm.hpp"

#include <list>

using namespace std;

namespace stream_process{

class reeb_node;
class reeb_link;

class reeb_link_element{
public:
	void attach (reeb_link_element *, reeb_node *);
	
	reeb_link_element (reeb_link *this_link, reeb_node* this_node, reeb_link_element* upper_element, reeb_link_element *lower_element){
		this->this_link = this_link;
		this->this_node = this_node;
		this->upper_element = upper_element;
		this->lower_element = lower_element;
	}

	void set_this_link(reeb_link *this_link) { this->this_link = this_link; }
	void set_node_ptr(reeb_node * this_node) { this->this_node = this_node; } 
	void set_upper_element(reeb_link_element *upper_element) { this->upper_element = upper_element; }
	void set_lower_element(reeb_link_element *lower_element) { this->lower_element = lower_element; }
	void set_this_it (list <reeb_link_element> :: iterator this_it) { this->this_it = this_it; }
	
	reeb_link* get_this_link() { return this_link; }
	reeb_node* get_node_ptr() { return this_node; }
	reeb_link_element* get_upper_element() { return upper_element; }
	reeb_link_element* get_lower_element() { return lower_element; }
	list <reeb_link_element> :: iterator get_this_it() { return this_it; }



protected:
	reeb_link *this_link;
	reeb_node *this_node;
	reeb_link_element* upper_element;
	reeb_link_element* lower_element;
	list <reeb_link_element> :: iterator this_it;
};




}	// namespace stream_process

#endif
