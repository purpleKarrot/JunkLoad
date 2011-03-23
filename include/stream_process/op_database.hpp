#ifndef __STREAM_PROCESS__OP_DATABASE__HPP__
#define __STREAM_PROCESS__OP_DATABASE__HPP__

#include <stream_process/op_manager.hpp>
#include <stream_process/stream_config.hpp>

#include <stream_process/attribute_accessor.hpp>

#include <stream_process/operator_base.hpp>

#include <stream_process/covariance_stream_op.hpp>
#include <stream_process/estimate_normal_stream_op.hpp>
#include <stream_process/radius_stream_op.hpp>

#include <stream_process/estimate_curvature_stream_op.hpp>
#include <stream_process/estimate_splat_stream_op.hpp>

#include <stream_process/neighbor_store_stream_op.hpp>

#include <stream_process/neighbor_stream_op.hpp>
#include <stream_process/nb_op.hpp>
#include <stream_process/tree_node.hpp>
#include <stream_process/point_region_kd_tree.hpp>
#include <stream_process/quad_tree.hpp>

#include <stream_process/face_reader_op.hpp>
#include <stream_process/face_print_op.hpp>
#include <stream_process/vertex_print_op.hpp>

#include <stream_process/zero_vertex_normal_op.hpp>
#include <stream_process/face_normal_op.hpp>
#include <stream_process/normalize_vertex_normal_op.hpp>

#include <stream_process/natural_normals_op.hpp>

#include <stream_process/accurate_curvature_op.hpp>

#include <stream_process/paraboloid_fit_curvature_op.hpp>
#include <stream_process/normal_plane_proj_curv_op.hpp>

#include <stream_process/curvature_paraboloid_op.hpp>

#include <stream_process/miao_curvature_op.hpp>

#include <stream_process/bagatelo_wu_curvature_op.hpp>

#include <stream_process/stream_file_reader.hpp>
#include <stream_process/stream_file_writer.hpp>

#include <stream_process/chain_io_stream_op.hpp>
#include <stream_process/stats_chain_op.hpp>

#include <stream_process/example_stream_operator.hpp>

#include <stream_process/perturb_normal_stream_op.hpp>

#include <stream_process/reeb_graph_stream_op.hpp>

namespace stream_process
{
class op_database
{
public:
    template< typename sp_types_t >
    void insert_ops( op_manager< sp_types_t >& op_man );

protected:
    template< typename sp_types_t >
	void _create_nb_ops( op_manager< sp_types_t >& op_man );

}; // class op_database




template< typename sp_types_t >
void
op_database::insert_ops( op_manager< sp_types_t >& op_man )
{
    typedef operator_base< sp_types_t >         op_base_type;
    typedef stream_operator< op_base_type >     stream_op_type;
    typedef chain_operator< op_base_type >      chain_op_type;
    
    op_man.add_stream_op( "read", new stream_file_reader< stream_op_type >() );
    op_man.add_stream_op( "face_read", new face_reader_op< stream_op_type >() );

    op_man.add_stream_op( "face_print", new face_print_op< stream_op_type >() );

    op_man.add_stream_op( "zero_vertex_normal", new zero_vertex_normal_op< stream_op_type >() );
    op_man.add_stream_op( "face_normal", new face_normal_op< stream_op_type >() );
    op_man.add_stream_op( "normalize_vertex_normal", new normalize_vertex_normal_op< stream_op_type >() );

    op_man.add_stream_op( "write", new stream_file_writer< stream_op_type >() );

    op_man.add_stream_op( "example", new example_stream_operator< stream_op_type >() );

    op_man.add_stream_op( "covariance", new covariance_stream_op< stream_op_type >() );
    op_man.add_stream_op( "radius", new radius_stream_op< stream_op_type >() );
    op_man.add_stream_op( "normal", new estimate_normal_stream_op< stream_op_type >() );
    op_man.add_stream_op( "nat_normal", new natural_normals_op< stream_op_type >() );

    op_man.add_stream_op( "perturb_normal", new perturb_normal_stream_op< stream_op_type >() );

    op_man.add_stream_op( "acc_curv", new accurate_curvature_op< stream_op_type >() );

    op_man.add_stream_op( "pb_curv", new paraboloid_fit_curvature_op< stream_op_type >() );
    op_man.add_stream_op( "npp_curv", new normal_plane_proj_curv_op< stream_op_type >() );

    op_man.add_stream_op( "curv_params", new curvature_paraboloid_op< stream_op_type >() );

    op_man.add_stream_op( "miao_curv", new miao_curvature_op< stream_op_type >() );

    op_man.add_stream_op( "curvature", new estimate_curvature_stream_op< stream_op_type >() );
    op_man.add_stream_op( "splat", new estimate_splat_stream_op< stream_op_type >() );

    op_man.add_stream_op( "nb_store", new neighbor_store_stream_op< stream_op_type >() );

    op_man.add_stream_op( "bw_curv", new bagatelo_wu_curvature_op< stream_op_type >() );

    op_man.add_stream_op( "reeb_graph", new reeb_graph_stream_op< stream_op_type >() );
    
	_create_nb_ops< sp_types_t >( op_man );
	
	op_man.add_stream_op( "chain-in", new chain_io_stream_op< stream_op_type >( true ) );
	op_man.add_stream_op( "chain-out", new chain_io_stream_op< stream_op_type >( false) );
    
    op_man.add_chain_op( "stats", new stats_chain_op< chain_op_type >() );
    op_man.add_stream_op( "vertex_print", new vertex_print_op< stream_op_type >() );

    op_man.add_chain( "passthrough", "read write" );
    op_man.add_chain( "normal", "read neighbor covariance radius normal write" );
    op_man.add_chain( "splat", "read neighbor covariance radius normal curvature splat write" );
    op_man.add_chain( "nsplat", "read neighbor covariance radius nat_normal curvature splat write" );
    op_man.add_chain( "fsplat", "read face_read neighbor covariance zero_vertex_normal face_normal normalize_vertex_normal radius curvature splat write" );
    op_man.add_chain( "acsplat", "read neighbor radius nat_normal acc_curv write" );

    op_man.add_chain( "bwsplat", "read neighbor radius covariance normal curvature splat bw_curv write" );
    op_man.add_chain( "bwsplatn", "read neighbor radius nat_normal curvature splat bw_curv write" );

    op_man.add_chain( "newsplat", "read neighbor covariance radius normal curvature splat curv_params write" );
    op_man.add_chain( "nppsplat", "read neighbor radius nat_normal covariance curvature splat npp_curv write" );

    op_man.add_chain( "miao", "read neighbor radius nat_normal miao_curv write" );

    op_man.add_chain( "debug", "read neighbor radius nat_normal acc_curv write" );

}


template< typename sp_types_t >
void
op_database::_create_nb_ops( op_manager< sp_types_t >& op_man )
{
    typedef operator_base< sp_types_t >     op_base_type;
    typedef stream_operator< op_base_type > stream_op_type;
    typedef chain_operator< op_base_type >  chain_op_type;
	typedef std::vector< stream_data* >		bucket_type;
	typedef typename sp_types_t::vec3		vec3;
	typedef attribute_accessor< vec3 >		vec3_acc_type;

	{
        typedef point_region_kd_tree< 3, vec3, stream_data*, vec3_acc_type,
            bucket_type > tree_type;
        typedef tree_node< tree_type, stream_data* >    node_type;
		typedef nb_op< stream_op_type, node_type >      nb_op_type;
		typedef neighbor_stream_op< stream_op_type, node_type >	nb_stream_op_type;
        
        nb_op_type* nb_exemplar = new nb_op_type();
		op_man.add_stream_op( "nb_3d", nb_exemplar );    

        nb_stream_op_type* nbs_exemplar = new nb_stream_op_type();
        op_man.add_stream_op( "neighbor_3d", nbs_exemplar );
	}
	{
        typedef point_region_kd_tree< 2, vec3, stream_data*, vec3_acc_type,
            bucket_type > tree_type;
        typedef tree_node< tree_type, stream_data* >    node_type;
		typedef nb_op< stream_op_type, node_type >      nb_op_type;
		typedef neighbor_stream_op< stream_op_type, node_type >	nb_stream_op_type;

        nb_op_type* nb_exemplar = new nb_op_type();
		op_man.add_stream_op( "nb", nb_exemplar );    
		op_man.add_stream_op( "nb_2d", nb_exemplar );    

        nb_stream_op_type* nbs_exemplar = new nb_stream_op_type();
        op_man.add_stream_op( "neighbor", nbs_exemplar );
        op_man.add_stream_op( "neighbor_2d", nbs_exemplar );
	}

}

} // namespace stream_process

#endif

