#ifndef __STREAM_PROCESS__SLICE_BUFFER2__HPP__
#define __STREAM_PROCESS__SLICE_BUFFER2__HPP__


#include <cstddef>

#include <set>
#include <queue>
#include <utility>
#include <functional>

namespace stream_process
{

template< typename sp_types_t >
struct get_slice_min
{
    STREAM_PROCESS_TYPES
    
    sp_float_type operator()( const slice_type* slice_ ) const
    {
        return slice_->get_min();
    }
    
};


template< typename sp_types_t >
struct get_slice_max
{
    STREAM_PROCESS_TYPES
    
    sp_float_type operator()( const slice_type* slice_ ) const
    {
        return slice_->get_max();
    }
    
};


template< typename sp_types_t >
struct get_slice_ring_one_min
{
    STREAM_PROCESS_TYPES
    
    sp_float_type operator()( const slice_type* slice_ ) const
    {
        return slice_->get_ring_one_min();
    }
    
};


template< typename sp_types_t >
struct get_slice_ring_one_max
{
    STREAM_PROCESS_TYPES
    
    sp_float_type operator()( const slice_type* slice_ ) const
    {
        return slice_->get_min();
    }
    
};


template< typename T, typename compare_t, typename get_t >
struct composite_compare
{
    inline bool operator()( const T* a, const T* b )
    {
        return compare_t()( get_t()( a ), get_t()( b ) );
    }
};



template< typename sp_types_t >
class slice_buffer2
{
public:
    STREAM_PROCESS_TYPES

    typedef std::deque< slice_type* >                   io_buffer_type;
    
    typedef std::less< sp_float_type >                  less_type;
    typedef std::greater< sp_float_type >               greater_type;
    
    typedef composite_compare< slice_type, less_type, 
        get_slice_ring_one_min< sp_types_t > > min_compare_type;

    typedef std::set< slice_type*, min_compare_type >   min_buffer_type;


    slice_buffer2();
    ~slice_buffer2();
    
    // adds a new slice to the buffer
    void push_newest( slice_type* t_ );
    
    // returns the oldest slice or 0
    slice_type* oldest();
    // removes the oldest slice from this buffer
    void pop_oldest();
    
    
    // if a slice is ready to be processed, returns it, otherwise 0.
    slice_type* ready();

    // clear stage, force the remaining slices, do not check bounds
    slice_type* force_ready();
    slice_type* force_oldest();

    // removes the ready slice from the in_queue. does NOT remove it
    // from the min-max buffers.
    void pop_ready();
    
    // adds a processed slice to the out_queue. 
    void push_completed( slice_type* top_ );
    
    


protected:
    // since we are streaming in z order, the oldest slice generally has the 
    // smallest min/max values. 
    min_buffer_type     _min_buffer;

    io_buffer_type      _in_queue;
    io_buffer_type      _out_queue;
    
}; // class slice_buffer2


#define SP_TEMPLATE_TYPES   template< typename sp_types_t >
#define SP_CLASS_NAME       slice_buffer2< sp_types_t >

SP_TEMPLATE_TYPES
SP_CLASS_NAME::slice_buffer2()
    : _min_buffer()
    , _in_queue()
    , _out_queue()
{}


SP_TEMPLATE_TYPES
SP_CLASS_NAME::~slice_buffer2()
{
    assert( _in_queue.empty() );
    assert( _out_queue.empty() );
    assert( _min_buffer.empty() );
}


// adds a new slice to the buffer
SP_TEMPLATE_TYPES
void
SP_CLASS_NAME::push_newest( slice_type* newest_ )
{
    _in_queue.push_front( newest_ );
    _min_buffer.insert( newest_ );
}



// returns the oldest slice or 0
SP_TEMPLATE_TYPES
typename SP_CLASS_NAME::slice_type*
SP_CLASS_NAME::oldest()
{
    if ( _out_queue.size() < 2 )
        return 0;

    slice_type* oldest      = _out_queue.back();
    slice_type* min_slice   = *_min_buffer.begin();
    
    assert( oldest != min_slice );
    
    if ( 0 )
    {
        (*_min_buffer.begin())->DEBUGprint( "min " );
        (*_min_buffer.rbegin())->DEBUGprint( "min_max " );
        oldest->DEBUGprint( "oldest" );
    
    }
    assert( min_slice->get_ring_one_min() <= (*_min_buffer.rbegin())->get_ring_one_min() );
    
    if ( oldest->get_max() < min_slice->get_ring_one_min() )
    {
        return oldest;
    }
    
    return 0;
}


SP_TEMPLATE_TYPES
// removes the oldest slice from this buffer
void
SP_CLASS_NAME::pop_oldest()
{
    assert( !_out_queue.empty() );
    _out_queue.pop_back();
}

     
// if a slice is ready to be processed, returns it, otherwise 0.
SP_TEMPLATE_TYPES
typename SP_CLASS_NAME::slice_type*
SP_CLASS_NAME::ready()
{
    if ( _in_queue.size() < 2 )
        return 0;

    slice_type* ready_slice = _in_queue.back();
    
    // a slice is ready to be processed if all elements it references are
    // fully contained in the buffer.
    // This means that the following conditions have to be fulfilled:
    // - its r1_min is larger than the smallest_min in the buffer
    // - its r1_max is smaller than the largest max in the buffer
    
    assert( ! _in_queue.empty() );
    if( ! _in_queue.empty() )
    {
        if ( _in_queue.front()->get_max() < ready_slice->get_ring_one_max() )
        {
            return 0;
        }
    }
    
    #if 0
    assert( ready_slice->get_slice_number() == 0 || ! _out_queue.empty() );
    if ( ! _out_queue.empty() )
    {
        if ( _out_queue.back()->get_min() > ready_slice->get_ring_one_min() )
        {
            return 0;
        }
    }
    #endif
    return ready_slice;
}



SP_TEMPLATE_TYPES
typename SP_CLASS_NAME::slice_type*
SP_CLASS_NAME::force_ready()
{
    return _in_queue.empty() ? 0 : _in_queue.back();
}



SP_TEMPLATE_TYPES
typename SP_CLASS_NAME::slice_type*
SP_CLASS_NAME::force_oldest()
{
    assert( _in_queue.empty() );
    return _out_queue.empty() ? 0 : _out_queue.back();
}




// removes the ready slice from the in_queue. does NOT remove it
// from the min-max buffers.
SP_TEMPLATE_TYPES
void
SP_CLASS_NAME::pop_ready()
{
    assert( !_in_queue.empty() );
    _in_queue.pop_back();
}


// adds a processed slice to the out_queue. 
SP_TEMPLATE_TYPES
void
SP_CLASS_NAME::push_completed( slice_type* ready_ )
{
    _out_queue.push_front( ready_ );
    _min_buffer.erase( ready_ );
}




#undef SP_TEMPLATE_TYPES
#undef SP_CLASS_NAME

} // namespace stream_process

#endif

