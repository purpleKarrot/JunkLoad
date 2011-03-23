#include "neighbor_indices_store_op.hpp"

#include "active_set.h"
#include "point_info.hpp"

namespace stream_process
{


neighbor_indices_store_op::neighbor_indices_store_op()
    : stream_op()
    , _point_index(     "point_index" )
    , _neighbors(       "neighbors" )
    , _nb_count(        "nb_count" )
    , _nb_indices(      "nb_indices" )
	, _in_queue()
    , _points_in_op( 0 )
{
    set_name( "neighbor_indices_store" );
}


    
neighbor_indices_store_op::~neighbor_indices_store_op()
{}



void 
neighbor_indices_store_op::pull_push()
{
    stream_point* point;
    while( ( point = prev->front() ) )
    {
        prev->pop_front();

        _write_indices( point );

        _in_queue.push_back( point );

        ++_points_in_op;
    }
}



void
neighbor_indices_store_op::_write_indices( stream_point* point )
{
    uint32_t nb_count           = point->get( _nb_count );
    uint32_t* nb_index          = point->get_ptr( _nb_indices );
    uint32_t* nb_indices_end    = nb_index + nb_count;
    
    neighbor* current_nb        = point->get_ptr( _neighbors );
    
    for( ; nb_index != nb_indices_end; ++nb_index, ++current_nb )
    {
        *nb_index = current_nb->get_point()->get( _point_index );
    }
    
    
}



stream_point* 
neighbor_indices_store_op::front()
{
    if ( _in_queue.empty() ) 
        return 0;
    return _in_queue.front();
}



void 
neighbor_indices_store_op::pop_front()
{
    assert( ! _in_queue.empty() );
    _in_queue.pop_front();
    --_points_in_op;
}



void
neighbor_indices_store_op::clear_stage()
{
}



void 
neighbor_indices_store_op::setup_stage_0()
{
    _require( _neighbors );
    _require( _nb_count );
    _require( _point_index );
}



void 
neighbor_indices_store_op::setup_stage_1()
{
    size_t nb_count = 0;
    var_map::iterator it = _config->find( "nb-count" );
    if( it != _config->end() )
        nb_count = (*it).second.as< size_t >();

    _reserve_array( _nb_indices, SP_UINT_32, nb_count, IO_WRITE_TO_OUTPUT );
}



void 
neighbor_indices_store_op::setup_stage_2()
{
}



} // namespace stream_process
