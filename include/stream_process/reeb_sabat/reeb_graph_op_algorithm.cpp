#include "reeb_graph_op_algorithm.hpp"
#include "chain_manager.hpp"
#include "fstream"

namespace stream_process
{
reeb_graph_op_algorithm::reeb_graph_op_algorithm()
	: _position(		"position" )
	, _neighbors(		"neighbors" )
	, _nb_count(		"nb_count" )
	, _min_ref_index(	"min_ref_index" )
	, _max_ref_index(	"max_ref_index" )
	, _reeb_node(		"reeb_node" )
	,_point_index(		"point_index")
	//, _radius(			"radius" )
{
	set_name( "reeb graph" );
	graph = new reeb_graph();
}

reeb_graph_op_algorithm :: ~reeb_graph_op_algorithm(){

	graph->print_graph();
	delete(graph);
	
	return;
}	

void
reeb_graph_op_algorithm::_compute( stream_point* point )
{
	const uint32_t nb_count	= point->get( _nb_count );
	neighbor* nb_begin		= point->get_ptr( _neighbors );
	neighbor* nb_end		= nb_begin + nb_count;
	const uint32_t min_ref_index = point->get (_min_ref_index);
	const uint32_t max_ref_index = point->get (_max_ref_index);
	reeb_node *v = point->get (_reeb_node);

	int point_index = v->get_index ();

	/* For each neighbour */
	for( neighbor* nb = nb_begin; (nb != nb_end); nb++ )
	{
		
		stream_point *nb_pt = nb->get_point();
		uint32_t nb_index = nb_pt->get( _point_index );

		reeb_node *nb_node = nb_pt->get ( _reeb_node );
		 
		if (!nb_node){
			continue;
		}	
		/* If a direct connection  does not exist - create link and glue*/
		if (nb_index < point_index){
			if (nb_node->is_connected (point_index)){
				/* Create and glue link*/
				create_and_glue_link ( v, nb_node );
			}
		}
		else {
			/* Add the connection... */
			v->add_direct_connection(nb_index);
			if ( (nb_index == max_ref_index) && (max_ref_index > point_index) ){
				v->set_my_trigger ( nb_node-> add_release_trigger(v) );
			//	nb_node-> add_release_trigger(v);
			}	
		}
	}
	
	list <reeb_node *> * release_list = v-> get_release_trigger();
	list <reeb_node *> :: iterator end_it = release_list->end();
	for (list <reeb_node *> :: iterator it = release_list->begin(); it != end_it; it++){
		clean_up (*it);
	}
	
	v->empty_trigger();
	if (point_index >= max_ref_index){
		clean_up(v);
	}
	return;
}

void
reeb_graph_op_algorithm::setup_stage_0()
{
    _reserve_custom_type( _reeb_node, SP_UNKNOWN_DATA_TYPE, 
        sizeof( void* ), 1 );
		
	_chain_manager->notify_on_release( this );
	_chain_manager->notify_on_insertion (this);
}

void 
reeb_graph_op_algorithm :: create_and_glue_link (reeb_node *top_node, reeb_node *bottom_node){
	
	/* Sort as top and bottom node */
	if( top_node->get_fn_val() == bottom_node->get_fn_val() ){
		if ( top_node->get_index() < bottom_node->get_index() ){
			/* Swap */
			reeb_node *temp;
			temp = top_node;
			top_node = bottom_node;
			bottom_node = temp;
		}
	}	
	else if ( top_node->get_fn_val() < bottom_node->get_fn_val()){
		/* Swap */
		reeb_node *temp;
		temp = top_node;
		top_node = bottom_node;
		bottom_node = temp;
	}	

	/* Initial work...
		-> Get the top link to link equal to and below the top node
		-> Get the bottom link, if having val greater than top node - to below or equal to top node
	*/

	reeb_link_element *top_link_element, *bottom_link_element, *pre_top_element;
	
	top_link_element = top_node->get_top_ref();
	bottom_link_element = bottom_node->get_top_ref();
	pre_top_element = NULL;
	
	reeb_node *x;
	/* Bring down the top link... */
	while (top_link_element != NULL){
		x = top_link_element->get_this_link()->get_reeb_node()[1];
		if (x == top_node){
			break;
		}
		
		pre_top_element = top_link_element;
		top_link_element = top_link_element->get_lower_element();
	}


	/* Bring down the bottom link... */

	while (bottom_link_element != NULL){
		x = bottom_link_element->get_this_link()->get_reeb_node()[0];

		if( x->get_fn_val() == top_node->get_fn_val() ){
			if ( x->get_index() < top_node->get_index() ){
				break;
			}
		}	
		else if ( x->get_fn_val() < top_node->get_fn_val()){
			break;
		}

		bottom_link_element = bottom_link_element->get_lower_element();
	}	
	
	/* Actual merge operation
		-> Create new link + surgury at top
		-> Merge operations...
	*/

	if (bottom_link_element == NULL){
		x = bottom_node;
	}
	else{
		x = bottom_link_element->get_this_link()->get_reeb_node()[1];
	}
	
	if (x != top_node){
		bool situation;
		if( x->get_fn_val() == top_node->get_fn_val() ){
			if ( x->get_index() < top_node->get_index() ){
				situation = true;
			}
			else {
				situation = false;
			}	
		}
		else if ( x->get_fn_val() < top_node->get_fn_val()){
			situation = true;
		}
		else {
			situation = false;
		}
		
		if (situation){
			/* Create new link... */
			reeb_link *new_link = new reeb_link (top_node, x);
			graph->add_link (new_link);
			bottom_link_element = new_link->add_element (bottom_node, NULL, bottom_link_element);
		}	
		else{
			/* Surgury at top... */
			graph->add_link ( bottom_link_element->get_this_link()->split_link (top_node) );
			bottom_link_element = bottom_link_element->get_lower_element();
//			reeb_link *new_link = bottom_link_element->get_this_link();
//			graph->add_link (new_link);
		}
	}	
	
	if (top_link_element){
		merge_from_top (top_link_element, bottom_link_element, top_node, bottom_node);
	}
	else{
		bottom_link_element->attach (pre_top_element, top_node);
	}	
}	

void 
reeb_graph_op_algorithm :: merge_from_top (reeb_link_element* top_link_element, reeb_link_element* bottom_link_element, reeb_node *top_node, reeb_node *bottom_node){

	reeb_node *main_node = top_node;

	while (1){
		/* Check which case applies
			1-> Common...
			2-> Same length...
			3-> bottom_link is longer...
			4-> top_link is longer...
		*/
		int situation = 0;
		
		reeb_link *top_link, *bottom_link;
		reeb_node ** topper_nodes, **bottommer_nodes;

		top_link = top_link_element->get_this_link();
		bottom_link = bottom_link_element->get_this_link();
		
		topper_nodes = top_link->get_reeb_node();
		bottommer_nodes = bottom_link->get_reeb_node();
			
		if (top_link == bottom_link){
			situation = 1;
		}
		else if (topper_nodes[0] == bottommer_nodes[0]){
			situation = 2;
		}
		else if ( bottommer_nodes[0]->get_fn_val() == topper_nodes[0]->get_fn_val() ){
			if ( bottommer_nodes[0]->get_index() < topper_nodes[0]->get_index() ){
				situation = 3;
			}
			else{
				situation = 4;
			}	
		}	
		else if ( bottommer_nodes[0]->get_fn_val() < topper_nodes[0]->get_fn_val()){
			situation = 3;
		}
		else {
			situation = 4;
		}
		
		switch (situation){
		case 1:
			/* Common link */

			bottom_link_element = bottom_link_element->get_lower_element();
			
			if (top_link_element->get_lower_element()){
				top_link_element = top_link_element->get_lower_element();
			}
			else {
				if (bottom_link_element){
					bottom_link_element->attach (top_link_element, top_node);
				}
				return;
			}	
					
			main_node = topper_nodes[0];
			
			break;
		case 2:
			/* Equal length */
		
			top_link->duplicate (bottom_link);
			bottom_link_element = bottom_link_element->get_lower_element();

			if (top_link_element->get_lower_element()){
				top_link_element = top_link_element->get_lower_element();
			}
			else {
				if (bottom_link_element){
					bottom_link_element->attach (top_link_element, top_node);
					bottom_link_element = NULL;
				}
			}	

			main_node = topper_nodes[0];
			
			topper_nodes[0]->erase_link (bottom_link);
			topper_nodes[1]->erase_link (bottom_link);
			graph->delete_link (bottom_link);
			delete (bottom_link);

			break;
		case 3:
			/* bottom_link is longer */

			top_link->smart_copy (bottom_link);

			if (top_link_element->get_lower_element()){
				top_link_element = top_link_element->get_lower_element();
			}
			else {
				bottom_link_element->attach (top_link_element, top_node);
				return;
			}

			main_node = topper_nodes[0];

			break;
		case 4:
			/* top_link is longer */

			bottom_link->smart_copy (top_link);
			bottom_link_element = bottom_link_element->get_lower_element();
			main_node = bottommer_nodes[0];
			
			break;
		}	


		/* Condition to break out... */
		if ( (bottom_link_element == NULL) || (main_node == bottom_node) ){
			break;
		}	
	}

}

void
reeb_graph_op_algorithm::on_release( stream_point* point )
{
	reeb_node *guilty_node = point->get ( _reeb_node );
	if (guilty_node){
		guilty_node->erase_my_trigger();	
		clean_up (guilty_node);
	}
}

void 
reeb_graph_op_algorithm :: clean_up (reeb_node * guilty_node){
	
	/* Things to do
		-> Reject this node
		-> from top to bottom
			-> clean up... 
			-> merge links at nodes - if possible...
		-> Try to merge lowest node	
	*/		
	
	/* Reject this node... */
	guilty_node->reject_point();
	reeb_node * temp = NULL;
	guilty_node-> get_stream_point()-> set (_reeb_node, temp);

	reeb_link_element *dirty_top_ref = guilty_node->get_top_ref();

	if (!dirty_top_ref){
		return;
	}
	reeb_node *last_node;
	reeb_link *waste_link;
	while (dirty_top_ref){
		reeb_node *main_node = dirty_top_ref->get_this_link()->get_reeb_node()[1];
		last_node = dirty_top_ref->get_this_link()->get_reeb_node()[0];
		
		/* Suicide command to dirty_top_ref...*/
		reeb_link_element * temp_element = dirty_top_ref->get_lower_element ();
		dirty_top_ref->get_this_link()->clean_element (dirty_top_ref);
		dirty_top_ref = temp_element;
		
		/* attempt to clean main_node */
		waste_link = main_node->attempt_to_clean();

		if (waste_link){
			graph->delete_node (main_node);
			graph->delete_link (waste_link);

			delete (main_node);
			delete (waste_link);
		}	
	}

	/* for last node */
	waste_link = last_node->attempt_to_clean();

	if (waste_link){
		graph->delete_node (last_node);
		graph->delete_link (waste_link);

		delete (last_node);
		delete (waste_link);
	}	
	
	return;
}

void
reeb_graph_op_algorithm :: on_insertion (stream_point * point)
{	
	const vec3f& position	= point->get( _position );
	const uint32_t point_index = point->get( _point_index );
	
	reeb_node * v = new reeb_node (point, position[2], point_index, position);
	point->set(_reeb_node, v);
	graph->add_node(v);
	point->set ( _reeb_node, v );
	
	return;
}	
	
} //namespace stream_process
