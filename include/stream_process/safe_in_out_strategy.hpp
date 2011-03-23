#ifndef __STREAM_PROCESS__SAFE_IN_OUT_STRATEGY__HPP__
#define __STREAM_PROCESS__SAFE_IN_OUT_STRATEGY__HPP__

/* 
* @brief safe in_out strategy

* only processes elements after all referenced elements have run through the previous operator
*
*
* @author jonas boesch
*/

#include <stream_process/in_out_strategy.hpp>

namespace stream_process
{

template< typename integer_t >
class safe_in_out_strategy : public in_out_strategy< integer_t >
{
public:
    virtual ~safe_in_out_strategy();
    virtual void push_back( stream_point* point );
    
protected:
    typedef in_out_strategy< integer_t > super; // for readability

    std::deque< stream_point* > _tmp_queue;
    
}; // class safe_in_out_strategy

template< typename integer_t >
safe_in_out_strategy< integer_t >::~safe_in_out_strategy()
{}



template< typename integer_t >
void
safe_in_out_strategy< integer_t >::push_back( stream_point* point )
{
    ++super::_points_in_op;
    
    typename super::index_reference_t zref;
    // update priority queue that maintains smallest referenced index from heap
    zref.first  = point->get( super::_min_ref_index );	// smallest referenced index
    zref.second = point->get( super::_point_index );			// from element
    super::_index_queue.push( zref );

    _tmp_queue.push_back( point );
    
    while( ! _tmp_queue.empty() )
    {
        stream_point* p = _tmp_queue.front();
        const integer_t max_ref_index = p->get( super::_max_ref_index );
        if ( max_ref_index < super::_previous_op->smallest_element() )
        {
            _tmp_queue.pop_front();
            super::_in_queue.push_back( p );
        }
        else
            break;
    }
    
}

} // namespace stream_process

#endif

