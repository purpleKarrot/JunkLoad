#ifndef __STREAM_PROCESS__SLICE_TRACKER__HPP__
#define __STREAM_PROCESS__SLICE_TRACKER__HPP__

#include <stream_process/stream_process_types.hpp>
#include <stream_process/mt_queue.hpp>
#include <stream_process/priority_queue.hpp>

#include <queue>
#include <vector>
#include <utility>
#include <functional>

namespace stream_process
{

/*
*   @brief a class that tracks minimum dependencies of slices using a pq
*   
*   PRE: all slices must be removed in order of their indices.
*/
template< typename sp_types_t >
class slice_tracker : public sp_types_t
{
public:
    STREAM_PROCESS_TYPES
    
    // minimal z referenced, slice index
    typedef std::pair< sp_float_type, size_t >  z_index_ref;
    typedef std::vector< z_index_ref >          container_type;
    typedef std::greater< z_index_ref >         cmp_type;

    typedef priority_queue< z_index_ref, container_type, cmp_type > pq_type;
    typedef mt_queue< z_index_ref, pq_type >    mt_pq_type;
    
    void insert( slice_type* slice );
    void remove( slice_type* slice ); 

    // before a call to is_safe_to_stream_out can possibly succeed, 
    // the slice has to be removed already... ( obviously :) )
    inline bool is_safe( slice_type* slice );
    inline bool is_safe( const sp_float_type& z );

protected:
    mt_pq_type  _reference_queue;

}; // class slice_tracker



template< typename sp_types_t >
void
slice_tracker< sp_types_t >::
insert( slice_type* slice )
{
    assert( slice );
    z_index_ref new_ref( slice->get_ring_one_min(), slice->get_slice_number() );
    _reference_queue.push( new_ref );
}



template< typename sp_types_t >
void
slice_tracker< sp_types_t >::
remove( slice_type* slice )
{
    assert( slice );
    
    size_t slice_number = slice->get_slice_number();
    #if 0
    while( ! _reference_queue.empty() && _reference_queue.top().second <= slice_number )
    {
        _reference_queue.pop();
    }
    #else
    z_index_ref ref;
    while( _reference_queue.try_pop( ref ) )
    {
        if ( ref.second <= slice_number )
        {
            // it's popped already :)
        }
        else
        {
            _reference_queue.push( ref );
            break;
        }
    }
    
    
    #endif
}



template< typename sp_types_t >
bool
slice_tracker< sp_types_t >::
is_safe( slice_type* slice )
{
    assert( slice );
    //if ( _reference_queue.empty() )
    //    return true;

    //assert( _reference_queue.top().second > slice->get_slice_number() );
    //return slice->get_max() < _reference_queue.top().first;
    
    bool safe = true;
    z_index_ref ref;
    if ( _reference_queue.try_pop( ref ) )
    {
        safe = slice->get_ring_one_max() < ref.first;
        _reference_queue.push( ref );
    }
    return safe;
}


template< typename sp_types_t >
bool
slice_tracker< sp_types_t >::
is_safe( const sp_float_type& z )
{
    bool safe = true;
    z_index_ref ref;
    if ( _reference_queue.try_pop( ref ) )
    {
        safe = z < ref.first;
        _reference_queue.push( ref );
    }
    return safe;
}


} // namespace stream_process

#endif

