#include "reeb_node.hpp"

namespace stream_process
{
/*	reeb_node::reeb_node(stream_point *point, double fn_val, int point_index, vec3f position){
		this->point = point;
		this ->fn_val = fn_val;
		this->point_index = point_index;
		point_rejected = false;
		this->position = position;
		this->top_ref = NULL;
	}	
	
	void
	reeb_node :: insert_link (reeb_link *link){
		links.insert(links.begin(), link);
		return;	
	}
*/	
	void
	reeb_node :: erase_link (reeb_link *link){
		list <reeb_link *> :: iterator it_end = links.end();
		for (list <reeb_link *> :: iterator it = links.begin(); it != it_end; it++){
			if (*it == link){
				links.erase(it);
				return;
			}
		}
		return;
	}
	
/*	void
	reeb_node :: add_direct_connection(int other_point){
		direct_connection.push_back(other_point);
		return;
	}
*/		
	bool
	reeb_node :: is_connected( int other_point ){
		for (unsigned int i = 0; i < direct_connection.size(); i++){
			if (direct_connection[i] == other_point){
				return true;
			}
		}
		
		return false;
	}	

	reeb_link * 
	reeb_node :: attempt_to_clean(){
		/* Do nothing if the corresponding node is still active... */ 
		if (!point_rejected){
			return NULL;
		}

		/* If this is not a degree 2 node... */
		if (links.size() != 2){
			return NULL;
		}

		/* Identify top and bottom link... */
		reeb_link *top_link, *bottom_link;
		list <reeb_link *> :: iterator it = links.begin();
		
		if ( (*it)->get_reeb_node()[0] == this){
			top_link = *it;
			it++;
			bottom_link = *it;
		}
		else {
			bottom_link = *it;
			it ++;
			top_link = *it;
		}	

		/* If the size of list is not same */

		if ( top_link->get_element_count() != bottom_link->get_element_count()){
			return NULL;
		}

		/* More rigorous test to check 'deletability' needed...
			If lower ref of top_link element is not bottom_link... */
		list <reeb_link_element> *top_link_element_list = top_link->get_points();
		list <reeb_link_element> :: iterator jt_end = top_link_element_list->end();

		for (list <reeb_link_element> :: iterator jt = top_link_element_list->begin();
			jt != jt_end; jt++){
			
			if (! (*jt).get_lower_element() ){
				return NULL;
			}

			if ( (*jt).get_lower_element()->get_this_link() != bottom_link ){
				return NULL;
			}
		}	
		
		/* Now it is our painful duty to delete it */
		top_link->erase_all_ref();
		
		reeb_node **temp_node = bottom_link->get_reeb_node();
		temp_node[1] = top_link->get_reeb_node()[1];
		bottom_link->set_reeb_node ( temp_node );

		temp_node[1]->erase_link (top_link);
		temp_node[1]->insert_link (bottom_link);

		return top_link;
	}	

} // namespace stream_process
