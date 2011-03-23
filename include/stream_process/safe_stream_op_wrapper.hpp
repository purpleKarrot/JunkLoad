#ifndef __STREAM_PROCESS__SAFE_STREAM_OP_WRAPPER__HPP__
#define __STREAM_PROCESS__SAFE_STREAM_OP_WRAPPER__HPP__

#include <stream_process/stream_process_types.hpp>

#include <stream_process/simple_slice_tracker.hpp>
#include <stream_process/slice_buffer2.hpp>

namespace stream_process
{

template< typename sp_types_t >
class safe_stream_op_wrapper
{
public:
    STREAM_PROCESS_TYPES

    typedef simple_slice_tracker< sp_types_t >      tracker_type;
    typedef slice_buffer2< sp_types_t >             buffer_type;
    
    typedef priority_queue< slice_type*, std::deque< slice_type* >, 
        ptr_greater< slice_type* > > pq_type;
    
    safe_stream_op_wrapper( stream_op_type& op_ );
    ~safe_stream_op_wrapper();
    

    void            push( slice_type* slice_ );
    slice_type*     top();
    void            pop();
    
    void            run_ready();
    void            run_clear_stage();
    
    stream_op_type& get_op();
    
    const std::string& get_op_name() const { return _op.get_op_name(); };
    
    void            enable_clear_stage();
    
protected:
    stream_op_type&         _op;
    buffer_type             _buffer;
    bool                    _clear_stage;

    
    
}; // class safe_stream_op_wrapper

#define SP_TEMPLATE_TYPES   template< typename sp_types_t >
#define SP_CLASS_NAME       safe_stream_op_wrapper< sp_types_t >

SP_TEMPLATE_TYPES
SP_CLASS_NAME::safe_stream_op_wrapper( stream_op_type& op_ )
    : _op( op_ )
    , _clear_stage( false )
{
    //assert( _op.needs_bounds_checking() );
}



SP_TEMPLATE_TYPES
SP_CLASS_NAME::~safe_stream_op_wrapper()
{
    //std::cout << "dtor of wrapper for " << _op.get_op_name() << std::endl;
}



SP_TEMPLATE_TYPES
void
SP_CLASS_NAME::push( slice_type* slice_ )
{
    _buffer.push_newest( slice_ );

    _clear_stage ? run_clear_stage() : run_ready();
}



SP_TEMPLATE_TYPES
typename SP_CLASS_NAME::slice_type*
SP_CLASS_NAME::top()
{
    return _clear_stage ? _buffer.force_oldest() : _buffer.oldest();
}



SP_TEMPLATE_TYPES
void
SP_CLASS_NAME::pop()
{
    _buffer.pop_oldest();
}



SP_TEMPLATE_TYPES
void
SP_CLASS_NAME::run_ready()
{
    while( slice_type* slice_ = _buffer.ready() )
    {
        _buffer.pop_ready();
        _op.push( slice_ );
    }

    while( slice_type* processed_ = _op.top() )
    {
        _op.pop();
        _buffer.push_completed( processed_ );
    }
}



SP_TEMPLATE_TYPES
void
SP_CLASS_NAME::run_clear_stage()
{
    while( slice_type* slice_ = _buffer.force_ready() )
    {
        _buffer.pop_ready();
        _op.push( slice_ );
    }

    while( slice_type* processed_ = _op.top() )
    {
        _op.pop();
        _buffer.push_completed( processed_ );
    }
}



SP_TEMPLATE_TYPES
typename SP_CLASS_NAME::stream_op_type&
SP_CLASS_NAME::get_op()
{
    return _op;
}



SP_TEMPLATE_TYPES
void
SP_CLASS_NAME::enable_clear_stage()
{
    _clear_stage = true;
}




#undef  SP_TEMPLATE_TYPES
#undef  SP_CLASS_NAME

} // namespace stream_process

#endif

