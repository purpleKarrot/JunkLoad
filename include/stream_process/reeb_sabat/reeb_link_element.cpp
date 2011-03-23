#include "reeb_link_element.hpp"

namespace stream_process
{
/*
reeb_link_element :: reeb_link_element (reeb_link *this_link, reeb_node* this_node, reeb_link_element* upper_element, reeb_link_element *lower_element){
	this->this_link = this_link;
	this->this_node = this_node;
	this->upper_element = upper_element;
	this->lower_element = lower_element;
}

void
reeb_link_element :: set_this_link(reeb_link *this_link){
	this->this_link = this_link;
}

void
reeb_link_element :: set_upper_element(reeb_link_element *upper_element){
	this->upper_element = upper_element;
}

void
reeb_link_element :: set_lower_element(reeb_link_element *lower_element){
	this->lower_element = lower_element;
}

void 
reeb_link_element :: set_this_it (list <reeb_link_element> :: iterator this_it){
	this->this_it = this_it;
}	

	
reeb_link* 
reeb_link_element :: get_this_link(){
	return this_link;
}

reeb_node *
reeb_link_element :: get_node_ptr(){
	return this_node;
}


reeb_link_element*
reeb_link_element :: get_upper_element(){
	return upper_element;
}

reeb_link_element* 
reeb_link_element :: get_lower_element(){
	return lower_element;
}

list <reeb_link_element> :: iterator 
reeb_link_element :: get_this_it(){
	return this_it;
}	
*/
void 
reeb_link_element :: attach (reeb_link_element *main_link_element, reeb_node * top_node){
	reeb_node *node_it = this_link->get_reeb_node()[1];
	reeb_link_element *element_it = this;
	
	while ( node_it != this_node ){
		reeb_link *main_link = element_it->get_this_link();
		main_link_element = main_link->add_element (top_node, main_link_element, NULL);
		node_it = main_link->get_reeb_node()[0];
		element_it = element_it->get_lower_element();
	}
}	



} // namespace stream_process
