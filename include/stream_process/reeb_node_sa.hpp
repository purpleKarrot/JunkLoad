#ifndef __STREAM_PROCESS__REEB_NODE_SA__HPP__
#define __STREAM_PROCESS__REEB_NODE_SA__HPP__

#include <stream_process/stream_process_types.hpp>

namespace stream_process
{

template< typename sp_types_t >
class reeb_link_sa;

template< typename sp_types_t >
class reeb_link_element_sa;

template< typename sp_types_t >
class reeb_node_sa
{
public:
    STREAM_PROCESS_TYPES
    
    typedef reeb_link_sa< sp_types_t >          reeb_link;
    typedef reeb_link_element_sa< sp_types_t >  reeb_link_element;
    typedef reeb_node_sa< sp_types_t >          reeb_node;

	reeb_node_sa(
        stream_point* point_,
        double fn_val_,
        int point_index_,
        vec3 position_)
        : point( point_ )
        , fn_val( fn_val_ )
        , point_index( point_index_ )
        , point_rejected( false )
        , point_lost( false )
        , position( position_ )
        , top_ref( 0 )
    {}

	void erase_link (reeb_link* link_);
	reeb_link* attempt_to_clean();

	double get_fn_val(){ return fn_val; }

	int get_index() { return point_index; }
	void set_index(int point_index) { this->point_index = point_index; }

	stream_point * get_stream_point() { return point; }

	void insert_link (reeb_link *link) { links.insert( links.begin(), link); }
	std::list <reeb_link *> * get_links() { return &links; }

	void set_top_ref (reeb_link_element * top_ref) { this->top_ref = top_ref; }
	reeb_link_element* get_top_ref() { return top_ref; }

	typename std::list <reeb_node *> :: iterator get_graph_it(){ return graph_it; }
	void set_graph_it(typename std::list <reeb_node *> :: iterator graph_it) { this->graph_it = graph_it; }

	typename std::list <reeb_node *> :: iterator add_release_trigger(reeb_node *other_node){
		release_trigger.push_front (other_node);
		return release_trigger.begin();
	}	

 	std::list <reeb_node *> * get_release_trigger(){ return &release_trigger; }
	void erase_trigger (typename std::list <reeb_node *> :: iterator it) { release_trigger.erase (it); }
	void set_my_trigger (typename std::list <reeb_node *> :: iterator my_trigger){ this->my_trigger = my_trigger; }
	void erase_my_trigger () { my_trigger_node->erase_trigger (my_trigger); }
	void empty_trigger(){ release_trigger.clear(); }
	void set_my_trigger_node(reeb_node *my_trigger_node){ this->my_trigger_node = my_trigger_node; }

	vec3f get_position() { return position; }

	bool is_connected(int);
	void add_direct_connection(int other_point){ direct_connection.push_back(other_point); }
	
	bool is_point_lost () { return point_lost; }
	void lose_point () { point_lost = true; };

	bool is_point_rejected() { return point_rejected; }
	void reject_point() { point_rejected = true; }

protected:
	int point_index;
	double fn_val;
	stream_point* point;
	std::list <reeb_link *> links;
	vec3f position;
	std::vector <int> direct_connection;
	bool point_rejected;
	bool point_lost;
	reeb_link_element *top_ref;
	typename std::list <reeb_node *> :: iterator graph_it;
	std::list <reeb_node *> release_trigger;
	reeb_node * my_trigger_node;
	typename std::list <reeb_node *> :: iterator my_trigger;

}; // class reeb_node_sa

} // namespace stream_process

#endif

