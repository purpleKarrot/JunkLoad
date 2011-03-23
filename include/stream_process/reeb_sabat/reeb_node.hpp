#ifndef __STREAM_PROCESS__REEB_NODE__HPP__
#define __STREAM_PROCESS__REEB_NODE__HPP__

#include <list>
#include "reeb_link.hpp"
#include "reeb_link_element.hpp"
#include "reeb_graph_op_algorithm.hpp"

using namespace std;

namespace stream_process
{

class reeb_link;
class reeb_link_element;

class reeb_node
{
public:
	void erase_link (reeb_link *);
	bool is_connected(int);
	reeb_link* attempt_to_clean();
	
	reeb_node(stream_point *point, double fn_val, int point_index, vec3f position){
		this->point = point;
		this ->fn_val = fn_val;
		this->point_index = point_index;
		point_rejected = false;
		point_lost = false;
		this->position = position;
		this->top_ref = NULL;
	}

	list <reeb_node *> :: iterator add_release_trigger(reeb_node *other_node){
		release_trigger.push_front (other_node);
		return release_trigger.begin();
	}	

	double get_fn_val(){ return fn_val; }

	int get_index() { return point_index; }

	list <reeb_link *> * get_links() { return &links; }

	stream_point * get_stream_point() { return point; }

	bool is_point_rejected() { return point_rejected; }

	void reject_point() { point_rejected = true; }

	void set_index(int point_index) { this->point_index = point_index; }

	void set_top_ref (reeb_link_element * top_ref) { this->top_ref = top_ref; }

	reeb_link_element* get_top_ref() { return top_ref; }

	list <reeb_node *> :: iterator get_graph_it(){ return graph_it; }

	void set_graph_it(list <reeb_node *> :: iterator graph_it) { this->graph_it = graph_it; }

	list <reeb_node *> * get_release_trigger(){ return &release_trigger; }
	
	void erase_trigger (list <reeb_node *> :: iterator it) { release_trigger.erase (it); }

	void set_my_trigger (list <reeb_node *> :: iterator my_trigger){ this->my_trigger = my_trigger; }

	void erase_my_trigger () { my_trigger_node->erase_trigger (my_trigger); }
	
	void empty_trigger(){ release_trigger.clear(); }

	void set_my_trigger_node(reeb_node *my_trigger_node){ this->my_trigger_node = my_trigger_node; }

	vec3f get_position() { return position; }
	
	void add_direct_connection(int other_point){ direct_connection.push_back(other_point); }
	
	void insert_link (reeb_link *link) { links.insert( links.begin(), link); }

	bool is_point_lost () { return point_lost; }

	void lose_point () { point_lost = true; };
protected:
	int point_index;
	double fn_val;
	stream_point* point;
	list <reeb_link *> links;
	vec3f position;
	vector <int> direct_connection;
	bool point_rejected;
	bool point_lost;
	reeb_link_element *top_ref;
	list <reeb_node *> :: iterator graph_it;
	list <reeb_node *> release_trigger;
	reeb_node * my_trigger_node;
	list <reeb_node *> :: iterator my_trigger;
}; // class reeb_node

} // namespace stream_process

#endif

