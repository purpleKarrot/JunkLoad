#include "reeb_graph.hpp"
#include "reeb_link_element.hpp"

using namespace std;

namespace stream_process
{

	class reeb_link_element;

/*	reeb_graph::reeb_graph(){
		 //do nothing... 
		
	}
*/

	reeb_graph :: ~reeb_graph(){
	
		for (list <reeb_node *> :: iterator it  = nodes.begin(); it != nodes.end(); it ++){
			delete (*it);
		}
		
		for (list <reeb_link *> :: iterator it = links.begin(); it != links.end(); it++){
			delete (*it);
		}
		
		return;
	}	


	void 
	reeb_graph::add_node(reeb_node *node){
		nodes.push_front(node);
		node->set_graph_it (nodes.begin());
		return;
	}	
	
	void 
	reeb_graph::add_link(reeb_link *link){
		links.push_front(link);
		link->set_graph_it (links.begin());
		return;
	}
		
	void 
	reeb_graph::delete_node(reeb_node *node){
		nodes.erase (node->get_graph_it());
		return;
	}

	void 
	reeb_graph::delete_link(reeb_link *link){
		links.erase (link->get_graph_it());
		return;
	}	


	void 
	reeb_graph::print_graph(){
		
//		print_graph_for_tulip ();
//		return;
	
		/* If tulip type io is desired, code should never reach here... */
		
		cerr << nodes.size() << ' ';
		cerr << links.size() << endl;
		ofstream fp;
		fp.open("reeb-output.ply", ios::out);

		/* Header */
		fp << "ply" << endl;
		fp << "format ascii 1.0" << endl;
		fp << "comment by Sabat @ IISc" << endl;
		fp << "element vertex " << nodes.size() << endl;
		fp << "property float32 x" << endl;
		fp << "property float32 y" << endl;
		fp << "property float32 z" << endl;
		fp << "element face " << links.size() << endl;
		fp << "property list uint8 int32 vertex_indices" << endl;
		fp << "end_header" << endl;
		

		/* Vertex */
		int i = 0;
	        for (list <reeb_node *> :: iterator it  = nodes.begin(); it != nodes.end(); it ++, i++){
                	(*it)->set_index(i);
                	fp << (*it)->get_position()[0] << ' ' << (*it)->get_position()[1] << ' ' << (*it)->get_position()[2] << endl;

/*			int it_size = (*it)->get_links()->size();

			if (it_size == 0){
				cerr << "\nWarning -- degree 0 point\n";
			}
			else if (it_size == 2){
				cerr << "\nError -- degree 2 point\n";
			}
			else if (it_size > 3){
				cerr << "\nWarning -- degree 3+ node : " << it_size << endl;
			}	
*/		
		}
		

		/* Faces */
		for (list <reeb_link *> :: iterator it = links.begin(); it != links.end(); it++, i++){
			fp << 3 << ' ';
	                fp << (*it)->get_reeb_node()[0]->get_index() << ' ';
			fp << (*it)->get_reeb_node()[1]->get_index() << ' ';
	                fp << (*it)->get_reeb_node()[0]->get_index() << ' '; 
			fp << endl;
			
/*			if ((*it)->get_reeb_node()[0]->get_index() == (*it)->get_reeb_node()[1]->get_index()){
				cerr << "\nError -- Self-referring edges\n";
			}
*/		
		}
		
		fp.close();
		return;
	}	
	
	void
	reeb_graph :: print_graph_for_tulip (){
		ofstream fp;
		fp.open("reeb-output.dat", ios::out);
			
		fp << "(tlp \"2.0\"" << endl;
		fp << ";(nodes <node_id> <node_id> ...)" << endl;
		
		fp << "(nodes ";
		int i = 0;
		for (list < reeb_node * > :: iterator it = nodes.begin(); it != nodes.end(); it++, i++ ){
			fp << i << ' ' ;
			(*it)->set_index (i);
		}
		fp << ")\n";
		
		
		fp << ";edge <edge_id> <source_id> <target_id>)" << endl;
		
		i = 0;
		for (list < reeb_link * > :: iterator it = links.begin(); it != links.end() ; it++, i++ ){
			fp << "(edge ";
			fp << i << ' ' ; 
			fp << (*it)->get_reeb_node()[0]->get_index() << ' ' << (*it)->get_reeb_node()[1]->get_index();
			fp << ")\n";
		}
	
//		fp << "(property  0 layout \"viewLayout\"" << endl ;
//		fp << "(default \"(803,703,998)\" \"()\")" << endl ;

//		i = 0;
//		for (list <reeb_node *> :: iterator it = nodes.begin(); it != nodes.end(); it++, i++){
//			fp << "(node ";
//			fp << i;
//			fp << " \"(";
//			vec3f pos;
//			pos = (*it)->get_position ();
//			fp << pos[0] << ',' << pos[1] << ',' << pos[2];
//			fp << ")\")";
//			fp << endl;
//		}
//		
//		fp << ")\n)";
		fp << ")";
		fp.close();
	}
	
} // namespace stream_process

