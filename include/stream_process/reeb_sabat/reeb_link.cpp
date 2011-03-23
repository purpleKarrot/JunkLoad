#include "reeb_link.hpp"

namespace stream_process
{
	reeb_link::reeb_link(reeb_node *a, reeb_node *b){
		if ( a->get_fn_val() == b->get_fn_val() ){
			if ( a->get_index() > b->get_index() ){
				reeb_node *temp = a;
				a = b;
				b = temp;
			}
		}	
		else if (a->get_fn_val() > b->get_fn_val() ){
			reeb_node *temp = a;
			a = b;
			b = temp;
		}

		
		nodes[0] = a;
		nodes[1] = b;
		element_count = 0;
		
		/* Insert the point into terminatinng nodes... */
		a->insert_link(this);
		b->insert_link(this);
	}
	
	reeb_link_element* 
	reeb_link :: add_element (reeb_node *this_node, reeb_link_element *upper_element, reeb_link_element *lower_element){
		reeb_link_element new_element (this, this_node, upper_element, lower_element);
//		pts.push_back (new_element);
		pts.push_front (new_element);
		increment_element_count(true);

		list <reeb_link_element> :: iterator it = pts.begin();
		reeb_link_element *new_element_add = & (*it);
		new_element_add->set_this_it (it);

		if (upper_element){
			upper_element->set_lower_element (new_element_add);
		}
		else {
			this_node->set_top_ref (new_element_add);
		}	

		if (lower_element){
			lower_element->set_upper_element (new_element_add);
		}

		return new_element_add;
	}	

	void 
	reeb_link :: duplicate (reeb_link *other){
		list <reeb_link_element> *o_pts = other->get_points();
		list <reeb_link_element> :: iterator o_pts_end = o_pts->end();
	
		for (list <reeb_link_element> :: iterator it = o_pts->begin(); it != o_pts_end; it++){
			(*it).set_this_link(this);
		}
		pts.splice (pts.begin(), *o_pts);
		increment_element_count (other->get_element_count());
	}

	void 
	reeb_link :: smart_copy (reeb_link *other){

		/* Things to do...
			-> for each element
				-> Copy the element
				-> Modify upward and downward elements approproately
			-> Modify end points...
		*/	
		list <reeb_link_element> *o_pts = other->get_points();
		list <reeb_link_element> :: iterator o_pts_end = o_pts->end();

		pts.insert ( pts.begin(), o_pts->begin(), o_pts_end );
		increment_element_count (other->get_element_count());

		list <reeb_link_element> :: iterator new_element_it = pts.begin();
		for (list <reeb_link_element> :: iterator it = o_pts->begin(); it != o_pts_end; it++, new_element_it++){
//			pts.push_back (*it);
//			increment_element_count (+1);
		
			reeb_link_element * new_element = & (*new_element_it);
			new_element->set_this_link (this);
			new_element->set_this_it (new_element_it);

			new_element->set_lower_element ( & (*it) );
			(*it).set_upper_element (new_element);

			if (new_element->get_upper_element()){
				new_element->get_upper_element()->set_lower_element (new_element);
			}
			else {
				new_element->get_node_ptr()->set_top_ref (new_element);
			}	
		}

		/* Modify end points... */
		reeb_node **other_nodes =  other->get_reeb_node();
		
		other_nodes[1]->erase_link (other);
		other_nodes[1] = nodes[0];
		other_nodes[1]->insert_link (other);
		other->set_reeb_node (other_nodes);
	}

	reeb_link*
	reeb_link :: split_link (reeb_node *split_node){
		reeb_link *new_link = new reeb_link (split_node, nodes[0]);

		/* Change nodes of this link */
		nodes[0]->erase_link (this);
		nodes[0] = split_node;
		nodes[0] ->insert_link (this);

		/* Smart duplicate ... */
	
		new_link->split_duplicate (this);

		return new_link;

//		return calling_link_element->get_lower_element();
		
	}

	void
	reeb_link :: split_duplicate (reeb_link *other){
		list <reeb_link_element> *o_pts = other->get_points();
		list <reeb_link_element> :: iterator o_pts_end = o_pts->end();

		pts.insert ( pts.begin(), o_pts->begin(), o_pts_end );
		increment_element_count (other->get_element_count());

		list <reeb_link_element> :: iterator new_element_it = pts.begin();
		for (list <reeb_link_element> :: iterator it = o_pts->begin(); it != o_pts_end; it++, new_element_it ++){
			
//			pts.push_back (*it);
//			increment_element_count (+1);
//			list <reeb_link_element> :: iterator temp_it = pts.end();
//			temp_it --;
			
			reeb_link_element *old_element = & (*it);
			reeb_link_element * new_element = & (*new_element_it);
			new_element->set_this_link (this);
			new_element->set_this_it (new_element_it);
			
			/* Things to do...
				Set its upper element to other_element...
				and vice verca
				and modify lower element to point to this ...
			*/	

			new_element->set_upper_element (old_element);
			old_element->set_lower_element (new_element);

			if (new_element->get_lower_element()){
				new_element->get_lower_element()->set_upper_element (new_element);
			}
		}	
	}

	void 
	reeb_link :: clean_element (reeb_link_element *guilty_element){
		reeb_link_element *lower_element = guilty_element->get_lower_element();

		guilty_element->get_this_link()->get_points()->erase ( guilty_element->get_this_it() );
		increment_element_count (-1);
//		return lower_element;
	}	
		
        void 
	reeb_link :: erase_all_ref (){
		list <reeb_link_element> :: iterator pts_end = pts.end();

		for (list <reeb_link_element> :: iterator it = pts.begin(); it != pts_end; it++){
			if ( (*it).get_upper_element() ){
				(*it).get_upper_element()->set_lower_element( (*it).get_lower_element() );
				(*it).get_lower_element()->set_upper_element( (*it).get_upper_element() );
			}
			else {
				
				(*it).get_lower_element()->set_upper_element( NULL );
				(*it).get_node_ptr()->set_top_ref ( (*it).get_lower_element() );
			}	
		}
	}	
} // namespace stream_process

