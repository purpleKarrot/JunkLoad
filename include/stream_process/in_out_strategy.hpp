#ifndef __STREAM_PROCESS__IN_OUT_STRATEGY__HPP__
#define __STREAM_PROCESS__IN_OUT_STRATEGY__HPP__

#include "stream_point.h"
#include "job_range.hpp"

#include "stream_op.h"

#include "rt_struct_member.h"

#include <queue>

/* 
* @brief default in_out strategy, no retention
*
*
* @author jonas boesch
*/


namespace stream_process
{

template< typename integer_t >
class in_out_strategy
{
public:
    typedef std::pair< integer_t , integer_t > index_reference_t; 
    typedef std::priority_queue< 
                index_reference_t, 
                std::deque< index_reference_t >, 
                std::greater< index_reference_t > 
                > index_queue_t;


    in_out_strategy();
	virtual ~in_out_strategy() {};
    
    virtual void set_previous_op( stream_op* prev_ );

	// these methods implement the behavior related to other stream_operators
	virtual void push_back( stream_point* point );
	virtual stream_point* front();
	virtual void pop_front();
	// smallest unprocessed element in this or previous stages
	virtual integer_t smallest_element();	
    // smallest reference of any unprocessed element in this or previous stages
	virtual integer_t smallest_reference();	
	virtual integer_t size() const;
	virtual integer_t in_size() const;
    
	// these methods implement the behavior related to op_algorithm
	virtual stream_point* op_top();
	virtual void op_pop();
	virtual void op_push( stream_point* point );

    virtual void get_batch( std::vector< job_range >& ranges );
    virtual void batch_complete( std::vector< job_range >& ranges );

protected:
    stream_op*                  _previous_op;

    std::deque< stream_point* > _in_queue;
    std::deque< stream_point* > _out_queue;
    
    index_queue_t               _index_queue;
    
    point_batch                 _point_batch;
    
    integer_t                   _points_in_op;

    rt_struct_member< integer_t >    _point_index;
    rt_struct_member< integer_t >    _min_ref_index;
    rt_struct_member< integer_t >    _max_ref_index;

}; // class in_out_strategy


typedef in_out_strategy< uint32_t > default_in_out_strategy;


template< typename integer_t >
in_out_strategy< integer_t >::in_out_strategy()
    : _points_in_op( 0 )
    , _point_index(     "point_index" )
    , _min_ref_index(   "min_ref_index" )
    , _max_ref_index(   "max_ref_index" )
    , _previous_op( 0 )
{}



template< typename integer_t >
void
in_out_strategy< integer_t >::push_back( stream_point* point )
{
    ++_points_in_op;
    
    index_reference_t zref;
    // update priority queue that maintains smallest referenced index from heap
    zref.first  = point->get( _min_ref_index );	// smallest referenced index
    zref.second = point->get( _point_index );			// from element
    _index_queue.push( zref );

    _in_queue.push_back( point );
}



template< typename integer_t >
stream_point*
in_out_strategy< integer_t >::front()
{
    return _out_queue.empty() ? 0 : _out_queue.front();
}



template< typename integer_t >
void
in_out_strategy< integer_t >::pop_front()
{
    assert( ! _out_queue.empty() );

    integer_t index = _out_queue.front()->get( _point_index );

    // remove outdated references from index_queue
    while ( ! _index_queue.empty() && _index_queue.top().second < index )
    {
        _index_queue.pop();
    }

    --_points_in_op;
    _out_queue.pop_front();
}



template< typename integer_t >
stream_point*
in_out_strategy< integer_t >::op_top()
{
    return _in_queue.empty() ? 0 : _in_queue.front();
}



template< typename integer_t >
void
in_out_strategy< integer_t >::op_pop()
{
    assert( ! _in_queue.empty() );
    _in_queue.pop_front();
}



template< typename integer_t >
void
in_out_strategy< integer_t >::op_push( stream_point* point )
{
    _out_queue.push_back( point );
}



template< typename integer_t >
integer_t
in_out_strategy< integer_t >::smallest_element()
{
    if ( ! _in_queue.empty() )
        return _in_queue.front()->get( _point_index );
    else
        return std::numeric_limits< integer_t >::max();
}



template< typename integer_t >
integer_t
in_out_strategy< integer_t >::smallest_reference()
{
    if ( ! _index_queue.empty() )
        return _index_queue.top().first;
    else
        return std::numeric_limits< integer_t >::max();
}



template< typename integer_t >
void
in_out_strategy< integer_t >::get_batch( std::vector< job_range >& jobs )
{
    size_t points_per_job = ( _in_queue.size() / jobs.size() ) + 1;
    
    std::vector< job_range >::iterator job_iter     = jobs.begin();
    std::vector< job_range >::iterator job_iter_end = jobs.end();
    
    _point_batch.reserve( _in_queue.size() );
    
    stream_point** begin = &_point_batch[ 0 ];
    
    for( ; job_iter != job_iter_end; ++job_iter )
    {
        for( size_t points = 0; points < points_per_job && !_in_queue.empty(); ++points )
        {
            stream_point* point = _in_queue.front();
            _in_queue.pop_front();
            _point_batch.push_back( point );
        }
        (*job_iter).begin   = begin;
        (*job_iter).end     = begin     = &_point_batch.back(); 
    }

    // since the iterators are [x,y[, we need to increase the iterator
    // otherwise the last point would be ignored.
    jobs.back().end += 1;
}



template< typename integer_t >
void
in_out_strategy< integer_t >::batch_complete( std::vector< job_range >& ranges )
{
    point_batch::iterator it        = _point_batch.begin();
    point_batch::iterator it_end    = _point_batch.end();
    
    for ( ; it != it_end; ++it )
    {
        _out_queue.push_back( *it );
    }

    _point_batch.clear();
}



template< typename integer_t >
integer_t
in_out_strategy< integer_t >::size() const
{
    return _points_in_op;
}



template< typename integer_t >
integer_t
in_out_strategy< integer_t >::in_size() const
{
    return _in_queue.size();
}


template< typename integer_t >
void
in_out_strategy< integer_t >::set_previous_op( stream_op* prev_ )
{
    _previous_op = prev_;
}



} // namespace stream_process

#endif

