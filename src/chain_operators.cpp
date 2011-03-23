#include "chain_operators.hpp"

#include "active_set_op.h"

namespace stream_process
{


chain_operators::chain_operators()
    : _points_in_active_set( 0 )
{}



void
chain_operators::insert( stream_point* point )
{
    ++_points_in_active_set;
    for( iterator it = begin(), it_end = end(); it != it_end; ++it )
    {
		(*it)->start_timer();
        (*it)->insert( point );
		(*it)->stop_timer();
    }
}



void
chain_operators::remove( stream_point* point )
{
    for( iterator it = begin(), it_end = end(); it != it_end; ++it )
    {
		(*it)->start_timer();
        (*it)->remove( point );
		(*it)->stop_timer();
    }
    --_points_in_active_set;
}



size_t
chain_operators::get_active_set_size()
{
    return _points_in_active_set;
}



} // namespace stream_process

