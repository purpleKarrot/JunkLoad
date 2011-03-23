#include "nb_index_out_op_algorithm.hpp"

namespace stream_process
{

nb_index_out_op_algorithm::nb_index_out_op_algorithm()
    : _nb_indices( "nb_indices" )
    , _neighbors( "neighbors" )
    , _nb_count( "nb_count" )
    , _point_index( "point_index" )
{
    set_name( "nb index out" );
}


nb_index_out_op_algorithm::~nb_index_out_op_algorithm()
{}


void
nb_index_out_op_algorithm::_compute( stream_point* point )
{
    const uint32_t nb_count = point->get( _nb_count );
    neighbor* nb            = point->get_ptr( _neighbors );
    neighbor* nb_end        = nb + nb_count;
    
    uint32_t* nb_indices    = point->get_ptr( _nb_indices );
    
    for( size_t i = 0; nb != nb_end; ++nb, ++i )
    {
        nb_indices[ i ] = nb->get_point()->get( _point_index );
    }

}


void
nb_index_out_op_algorithm::setup_stage_1()
{
    var_map::iterator it = _config->find( "nb-count" );
    if( it != _config->end() )
        _max_neighbors = (*it).second.as< size_t >();
    else
        throw exception( "nb_count member required for nb_indices op", 
            SPROCESS_HERE );
        
    _reserve_array( _nb_indices, SP_UINT_32, _max_neighbors, IO_WRITE_TO_OUTPUT );
}

} // namespace stream_process

