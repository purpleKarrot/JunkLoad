#include "built_in_operators.hpp"

#include "chain_manager.hpp"

#include "operator_factory.hpp"
#include "modular_operator_factory.hpp"

#include "read_op.h"
#include "write_op.h"
#include "noise_op.h"
#include "kd_heap_neighbor_op.h"
#include "normal_op.h"
#include "splat_op.h"
#include "curvature_op.h"
#include "splat_op2.h"
#include "fairing_op.h"
#include "outlier_op.hpp"
#include "test_op.h"
#include "neighbor_op.hpp"
#include "normal_test_op.hpp"
#include "adaptive_neighbor_op.hpp"
#include "mls_op.hpp"
#ifdef STREAM_PROCESS_USE_APSS
#include "apss_op.hpp"
#endif

#include "neighbor_indices_store_op.hpp"
#include "transform_back_op.hpp"

#include "tree_op.hpp"
#include "pr_kd_tree_node.h"

#include "stats_op.h"

// modular ops
#include "in_out_strategy.hpp"
#include "safe_in_out_strategy.hpp"

#include "normal_compare_op_algorithm.hpp"
#include "curvature_computation_algorithm.hpp"
#include "nb_index_out_op_algorithm.hpp"
#include "radius_op_algorithm.hpp"
#include <stream_process/ellipse_axis_fix_op.hpp>
#include <stream_process/covariance_op_algorithm.hpp>
#include <stream_process/normal_orient_hack_op_algorithm.hpp>
#include <stream_process/normal_orient_hack_2_op_algorithm.hpp>

#include <stream_process/normal_estimation_op_algorithm.hpp>
#include <stream_process/cov_normal_radius_op_algorithm.hpp>

#include <stream_process/curvature_estimation_op_algorithm.hpp>
#include <stream_process/curv_splat_op_algorithm.hpp>


namespace stream_process
{
built_in_operators::built_in_operators( chain_manager& chain_manager_ )
{
    // add built-in stream operators to the chain manager.
    chain_manager_.register_stream_op_factory( "read", 
        new default_stream_op_factory< read_op, read_op_options >() 
        );

    chain_manager_.register_stream_op_factory( "neighbor", 
        new default_stream_op_factory< neighbor_op, 
            neighbor_op_options >()
        );

    chain_manager_.register_stream_op_factory( "kd_heap_neighbor", 
        new default_stream_op_factory< kd_heap_neighbor_op, 
            kd_heap_neighbor_op_options >()
        );

    chain_manager_.register_stream_op_factory( "write", 
        new default_stream_op_factory< write_op, write_op_options >() 
        );

    chain_manager_.register_stream_op_factory( "normal_old", 
        new default_stream_op_factory< normal_op >() 
        );

    chain_manager_.register_stream_op_factory( "splat_old", 
        new default_stream_op_factory< splat_op >() 
        );

    chain_manager_.register_stream_op_factory( "splat2_old", 
        new default_stream_op_factory< splat_op2 >() 
        );

    chain_manager_.register_stream_op_factory( "fair", 
        new default_stream_op_factory< fairing_op >() 
        );

    chain_manager_.register_stream_op_factory( "curvature_old", 
        new default_stream_op_factory< curvature_op >() 
        );

    chain_manager_.register_stream_op_factory( "outlier", 
        new default_stream_op_factory< outlier_op >() 
        );

    chain_manager_.register_stream_op_factory( "noise", 
        new default_stream_op_factory< noise_op >() 
        );

    chain_manager_.register_stream_op_factory( "test", 
        new default_stream_op_factory< test_op >() 
        );

    chain_manager_.register_stream_op_factory( "fair_test", 
        new default_stream_op_factory< normal_test_op >() 
        );

#ifdef STREAM_PROCESS_USE_APSS
    chain_manager_.register_stream_op_factory( "sphere_fit", 
        new default_stream_op_factory< apss_op >() 
        );
#endif

#if 0
    chain_manager_.register_stream_op_factory( "nb_indices_out", 
        new default_stream_op_factory< neighbor_indices_store_op >() 
        );
#endif

    chain_manager_.register_stream_op_factory( "transform_back_op", 
        new default_stream_op_factory< transform_back_op >() 
        );


    chain_manager_.register_stream_op_factory( "adaptive_neighbor", 
		new default_stream_op_factory< adaptive_neighbor_op >() 
		);


	chain_manager_.register_stream_op_factory( "mls_surface", 
		new default_stream_op_factory< mls_op >() 
		);


	chain_manager_.register_stream_op_factory( "normal_compare", 
		new modular_stream_operator_factory< normal_compare_op_algorithm >() 
		);

	chain_manager_.register_stream_op_factory( "curv_new_x", 
		new modular_stream_operator_factory< curvature_computation_algorithm< double > >() 
		);


	chain_manager_.register_stream_op_factory( "nb_indices_out", 
		new modular_stream_operator_factory< nb_index_out_op_algorithm >() 
		);


	chain_manager_.register_stream_op_factory( "radius", 
		new modular_stream_operator_factory< radius_op_algorithm< uint32_t, float > >() 
		);


	chain_manager_.register_stream_op_factory( "axis_fix", 
		new modular_stream_operator_factory< ellipse_axis_fix_op< float > >() 
		);

	chain_manager_.register_stream_op_factory( "covariance", 
		new modular_stream_operator_factory< covariance_op_algorithm<> >() 
		);

	chain_manager_.register_stream_op_factory( "curvature", 
		new modular_stream_operator_factory< curvature_estimation_op_algorithm<>, 
            safe_in_out_strategy< uint32_t > >() 
		);

	chain_manager_.register_stream_op_factory( "curv-splat", 
		new modular_stream_operator_factory< curv_splat_op_algorithm<>, 
            safe_in_out_strategy< uint32_t > >() 
		);

	chain_manager_.register_stream_op_factory( "normal", 
		new modular_stream_operator_factory< normal_estimation_op_algorithm<> >() 
		);

	chain_manager_.register_stream_op_factory( "normal+", 
		new modular_stream_operator_factory< cov_normal_radius_op_algorithm<> >() 
		);

	chain_manager_.register_stream_op_factory( "normal_orient", 
		new modular_stream_operator_factory< normal_orient_hack_op_algorithm< float, uint32_t > >() 
		);

	chain_manager_.register_stream_op_factory( "normal_orient2", 
		new modular_stream_operator_factory< normal_orient_hack_2_op_algorithm< float, uint32_t > >() 
		);

  // add built-in chain operators to the chain manager.

    chain_manager_.register_chain_op_factory( "tree", 
        new default_chain_op_factory< tree_op< pr_kd_tree_node > >() 
        );

    chain_manager_.register_chain_op_factory( "stats", 
        new default_chain_op_factory< stats_op >() 
        );
        
    
    
}

} // namespace stream_process

