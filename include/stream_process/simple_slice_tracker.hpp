#ifndef __STREAM_PROCESS__SIMPLE_SLICE_TRACKER__HPP__
#define __STREAM_PROCESS__SIMPLE_SLICE_TRACKER__HPP__

#include <stream_process/stream_process_types.hpp>
#include <stream_process/mt_queue.hpp>
#include <stream_process/priority_queue.hpp>

#include <queue>
#include <vector>
#include <utility>
#include <functional>

namespace stream_process
{

template< typename sp_types_t >
class simple_slice_tracker
{
public:
    STREAM_PROCESS_TYPES

    // CONCEPTUALLY WRONG, NEED TO ORDER SLICES ACCORDING TO SLICE NUM
    // AND THEN RELEASE WHEN SAFE

    // minimal z referenced, slice index
    typedef std::pair< sp_float_type, size_t >      z_index_ref;
    typedef std::vector< z_index_ref >              container_type;
    typedef std::greater< z_index_ref >             cmp_type;

    typedef priority_queue< z_index_ref, container_type, cmp_type > pq_type;

    void insert( slice_type* slice );
    void remove( slice_type* slice ); 

    // before a call to is_safe_to_stream_out can possibly succeed, 
    // the slice has to be removed already... ( obviously :) )
    inline bool is_safe( slice_type* slice );
    inline bool is_safe( const sp_float_type& z );

protected:
    pq_type     _reference_queue;

}; // class simple_slice_tracker

#define SP_TEMPLATE_TYPES   template< typename sp_types_t >
#define SP_CLASS_NAME       simple_slice_tracker< sp_types_t >


SP_TEMPLATE_TYPES
void
SP_CLASS_NAME::insert( slice_type* slice )
{
    assert( slice );
    z_index_ref new_ref( slice->get_ring_one_min(), slice->get_slice_number() );
    _reference_queue.push( new_ref );
}



SP_TEMPLATE_TYPES
void
SP_CLASS_NAME::remove( slice_type* slice )
{
    assert( slice );
    
    size_t slice_number = slice->get_slice_number();
    while( ! _reference_queue.empty() && _reference_queue.top().second <= slice_number )
    {
        _reference_queue.pop();
    }
}




SP_TEMPLATE_TYPES
inline bool
SP_CLASS_NAME::is_safe( slice_type* slice )
{
    if ( _reference_queue.empty() )
        return true;

    assert( _reference_queue.top().second >= slice->get_slice_number() );
    return slice->get_max() < _reference_queue.top().first;
    //return slice->get_max() < _reference_queue.top().first;
}



SP_TEMPLATE_TYPES
inline bool
SP_CLASS_NAME::is_safe( const sp_float_type& z )
{
    return false;
}


#undef SP_TEMPLATE_TYPES
#undef SP_CLASS_NAME

} // namespace stream_process

#endif

