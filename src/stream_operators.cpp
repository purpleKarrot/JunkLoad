#include "stream_operators.hpp"

#include "stream_op.h"
#include "active_set.h"

#include "exception.hpp"
#include <typeinfo>
#include <functional>

namespace stream_process
{

stream_operators::stream_operators()
{}



stream_operators::~stream_operators()
{
    for( iterator it = begin(), it_end = end(); it != it_end; ++it )
    {
        delete *it;
    }
    _operators.clear();
}



void
stream_operators::check_requirements()
{
	for_each( begin(), end(), std::mem_fun( &stream_op::check_requirements ) );
}



void
stream_operators::pull_push()
{
    for( iterator it = begin(), it_end = end(); it != it_end; ++it )
    {
        (*it)->start_timer();
        (*it)->pull_push();
        (*it)->stop_timer();
    }   
}



void
stream_operators::clear_stage()
{
    for( iterator it = begin(), it_end = end(); it != it_end; ++it )
    {
        (*it)->start_timer();
        (*it)->pull_push();
        (*it)->clear_stage();
        (*it)->stop_timer();
    }
}



stream_op*
stream_operators::get_head()
{
    return _operators.empty() ? 0 : _operators.front();
}



stream_op*
stream_operators::get_tail()
{
    return _operators.empty() ? 0 : _operators.back();
}



void
stream_operators::push_back_operator( stream_op* op )
{
	 op->attach_to( _operators.empty() ? 0 : _operators.back() );
	 push_back( op );
}


} // namespace stream_process

