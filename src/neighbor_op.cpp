#include "neighbor_op.hpp"

#include "active_set.h"
#include "point_info.hpp"

#include "Utils.h"
#include "Log.h"

#include "chain_manager.hpp"

#define MAX_COUNT 8

namespace stream_process
{


neighbor_op::neighbor_op()
    : stream_op()
    , _position(        "position" )
    , _point_index(     "point_index" )
    , _neighbors(       "neighbors" )
    , _nb_count(        "nb_count" )
    , _min_ref_index(   "min_ref_index" )
    , _max_ref_index(   "max_ref_index" )
	, _in_queue()
	, _out_queue()
    , _points_in_op( 0 )
    , _query_point( 0 )
    , _query_position( 0.0f, 0.0f, 0.0f)
    , _query_neighbors( 0 )
	, _index_queue( 0 )
    , _tree_op_shared_data( 0 )
{
    set_name( "neighbor" );
    
    _chain_manager->create_chain_op( "tree" );
}


    
neighbor_op::~neighbor_op()
{}



void 
neighbor_op::pull_push()
{
    stream_point* point;
    while( ( point = prev->front() ) )
    {
        assert( _factory->is_valid_ptr( point ) );
        
        prev->pop_front();

        _in_queue.push_back( point );
        ++_points_in_op;
    }
    
    if ( _query_point == 0 )
    {
        if ( ! _in_queue.empty() && _tree_op_shared_data->do_search )
            _activate_next_point();
    }
    else
    {
        // when the distance to the kth neighbor is smaller than the
        // safe range, we can safely push the neighbor into the out queue.
        while ( _query_neighbors->get_distance() < _tree_op_shared_data->safe_range )
        {
            // get important member fields
            const uint32_t point_index  = _query_point->get( _point_index );
            const uint32_t nb_count     = _query_point->get( _nb_count );
            neighbor* neighbors_        = _query_point->get_ptr( _neighbors );
            neighbor* nb_end            = neighbors_ + nb_count;

            // sort the neighbors smallest-first
            std::sort( neighbors_, nb_end, _pair_less );
            
            // initialize min_ref and max_ref
            neighbor* current_neighbor = neighbors_;
            uint32_t index = current_neighbor->get_point()->get( _point_index );
            
            uint32_t& min_ref = _query_point->get( _min_ref_index );
            uint32_t& max_ref = _query_point->get( _max_ref_index );
            min_ref = max_ref = index;
                           
            for( ; current_neighbor != nb_end; ++current_neighbor )
            {
                index = current_neighbor->get_point()->get( _point_index );
                // we need to know the min and max referenced point indices
                if ( index < min_ref )
                    min_ref = index;
                else if ( index > max_ref )
                    max_ref = index;
            }
            
            // update index_queue
            if ( min_ref < _index_queue->top().first )
                _index_queue->push( index_reference( min_ref, point_index ) );
            
            // we're done with this point
            _out_queue.push_back( _query_point );
            _query_point = 0;

            if ( ! _in_queue.empty() )
                _activate_next_point();
            else
                break;
        }
    }
}



void
neighbor_op::clear_stage()
{
    while ( !_in_queue.empty() )
    {
        if ( _query_point == 0 )
            _activate_next_point();
        
        // get important member fields
        const uint32_t nb_count = _query_point->get( _nb_count );
        neighbor* neighbors_    = _query_point->get_ptr( _neighbors );
        neighbor* nb_end        = neighbors_ + nb_count;

        // sort the neighbors smallest-first
        std::sort( neighbors_, nb_end, _pair_less );

        // initialize min_ref and max_ref
        neighbor* current_neighbor = neighbors_;
        uint32_t index = current_neighbor->get_point()->get( _point_index );

        uint32_t& min_ref = _query_point->get( _min_ref_index );
        uint32_t& max_ref = _query_point->get( _max_ref_index );
        min_ref = max_ref = index;
                       
        for( ; current_neighbor != nb_end; ++current_neighbor )
        {
            index = current_neighbor->get_point()->get( _point_index );
            // we need to know the min and max referenced point indices
            if ( index < min_ref )
                min_ref = index;
            else if ( index > max_ref )
                max_ref = index;
        }

        _out_queue.push_back( _query_point );
        _query_point = 0;
    }
}



void
neighbor_op::_activate_next_point()
{
    _query_point        = _in_queue.front();
    _in_queue.pop_front();

    assert( _query_point );
    _query_position     = _query_point->get( _position );
    _query_neighbors    = _query_point->get_ptr( _neighbors );

    _tree_op_shared_data->query_z = _query_position[ 2 ];
    _tree_op_shared_data->compute_safe_range();
}



stream_point* 
neighbor_op::front()
{
    return _out_queue.empty() ? 0 : _out_queue.front();
}



void 
neighbor_op::pop_front()
{
    #ifdef TREE_STATS
    if ( _debug_p_counter % 2500 == 0 )
    {
        LOGINFO << "node count: " << _root.get_node_count() << "\n"
            << "load count: " << _root.get_load_count() << std::endl;
    }
    #endif
    assert( ! _out_queue.empty() );
    
    uint32_t index = _out_queue.front()->get( _point_index );
    _out_queue.pop_front();
    
    while ( ! _index_queue->empty() && _index_queue->top().second < index )
    {
        _index_queue->pop();
    }
    --_points_in_op;
}



void 
neighbor_op::setup_stage_0()
{}



void 
neighbor_op::setup_stage_1()
{}


void 
neighbor_op::setup_stage_2()
{
    //tree_op_tmp.get_root_node().accept_visitor( &visitor );
    _tree_op_shared_data = & _active_set->get_shared_data();
    _index_queue = & _tree_op_shared_data->index_queue_;
}



void 
neighbor_op::_DEBUG_print_neighbors( stream_point* point ) const
{
    neighbor* nbs = point->get_ptr( _neighbors );
    const uint32_t nb_count = point->get( _nb_count );

    for( size_t i = 0; i < nb_count; ++i )
    {
        neighbor& nb = nbs[ i ]; 
        std::cout << "nb " << i 
            << " point_index " << nb.get_point()->get( _point_index ) 
            << " distance " << nb.get_distance() << "\n";
    }
    std::cout << std::endl;

}


void 
neighbor_op::_DEBUG_check_neighbors( stream_point* point ) const
{
    const vec3f& position       = point->get( _position );
    neighbor* nb                = point->get_ptr( _neighbors );
    const uint32_t& nb_count    = point->get( _nb_count );

    for( size_t i = 0; i < nb_count; ++i )
    {
        const vec3f& nb_pos  = nb[i].get_point()->get( _position );
        float dist = position.distanceSquared( nb_pos );
        if ( dist != nb[ i ].get_distance() )
        {
            std::cout
                << "point    "     << point->get( _point_index ) << "\n"
                << "neighbor "  << nb[ i ].get_point()->get( _point_index ) << "\n"
                << "nb " << i << " of " << nb_count << "\n"
                << "real dist: " << dist << " dist in list " << nb->get_distance() << "\n"
                << std::endl;
            
        
            throw exception( "Invalid entry in neighbor list", 
                SPROCESS_HERE );
        }

    }
}



neighbor_op_options::neighbor_op_options()
    : op_options( "active_set neighborhood detection")
{
    _op_options.add_options()
        ( "nb-count,k", 
            boost::program_options::value< size_t >()->default_value( MAX_COUNT ),
            "Specify the value of k.\n"
            "  k is the number of neighbors used in the k-nearest neighbor search. Please note that some operators might override this setting." 
            )
        ( "bucket-size,b",
            boost::program_options::value< size_t >(),
            "Bucket size of a tree node (advanced).\n"
            "  The bucket size can drastically influence the performance of the neighborhood operator. FIXME"
            );
    _pos_op_options.add( "nb-count", 1 );
}



    
} // namespace stream_process
