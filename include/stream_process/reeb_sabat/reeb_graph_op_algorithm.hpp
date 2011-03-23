#ifndef __STREAM_PROCESS__REEB_GRAPH_OP_ALGORITHM__HPP__
#define __STREAM_PROCESS__REEB_GRAPH_OP_ALGORITHM__HPP__

#include "op_algorithm.hpp"
#include "neighbor.h"
#include <fstream>
#include <iomanip>
#include "reeb_node.hpp"
#include "reeb_link.hpp"
#include "reeb_graph.hpp"
#include "reeb_link_element.hpp"

#include "point_release_notification.hpp"
#include "point_insertion_notification.hpp"

using namespace std;

namespace stream_process
{

class reeb_node;
class reeb_link;
class reeb_graph;
class reeb_link_element;

class reeb_graph_op_algorithm : public op_algorithm,  public point_insertion_notification_target, public point_release_notification_target
{
public:
	reeb_graph_op_algorithm();
	~reeb_graph_op_algorithm();
	
    virtual op_algorithm* clone(){ return new reeb_graph_op_algorithm( *this ); };

	virtual void setup_stage_0();
	
	virtual void on_release( stream_point* point );
	void clean_up (reeb_node * point);

	virtual void on_insertion (stream_point * point);
protected:
	virtual void _compute( stream_point* point );
	
	// The functions needed by this operator...
	void create_and_glue_link(reeb_node *, reeb_node *);
	void merge_from_top (reeb_link_element *, reeb_link_element *, reeb_node *, reeb_node *);

	reeb_graph* graph;

	// members used 
    rt_struct_member< vec3f >			_position;
    rt_struct_member< neighbor >		_neighbors;
    rt_struct_member< uint32_t >		_nb_count;
    rt_struct_member< uint32_t >        _min_ref_index;
    rt_struct_member< uint32_t >        _max_ref_index;
	rt_struct_member< uint32_t >        _point_index;
    //rt_struct_member< float >			_radius;

	// members created
    rt_struct_member< reeb_node* >		_reeb_node;

}; // class reeb_graph_op_algorithm

} // namespace stream_process

#endif

