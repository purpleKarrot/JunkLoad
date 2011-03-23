#ifndef __STREAM_PROCESS__STREAM_OPERATOR__HPP__
#define __STREAM_PROCESS__STREAM_OPERATOR__HPP__

#include <stream_process/stream_data.hpp>

#include <queue>

namespace stream_process
{

class data_structure;
class stream_config;

template< typename operator_base_t >
class stream_operator : public operator_base_t
{
public:
	typedef typename operator_base_t::slice_type	slice_type;
	
    stream_operator() : _op_number( 255 ) {}
	
	virtual ~stream_operator() {}

    virtual void            push( slice_type* data_slice_ );
    virtual slice_type*     top();
    virtual void            pop();
    
    virtual size_t          size() const { return _out_buffer.size(); }

    virtual bool needs_bounds_checking() const { return true; };

    virtual stream_operator* clone() { return 0; };
		
	virtual void set_op_number( size_t op_number_ );
	virtual size_t get_op_number() const;
    
protected:
	size_t                          _op_number;
    std::deque< slice_type* >       _out_buffer;

}; // class stream_operator

#define SP_TEMPLATE_STRING      template< typename operator_base_t >
#define SP_CLASS_NAME           stream_operator< operator_base_t >


SP_TEMPLATE_STRING
void
SP_CLASS_NAME::push( slice_type* slice_ )
{
    _out_buffer.push_back( slice_ );
}


SP_TEMPLATE_STRING
typename SP_CLASS_NAME::slice_type*
SP_CLASS_NAME::top()
{
    return _out_buffer.empty() ? 0 : _out_buffer.front();
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::pop()
{
    assert( ! _out_buffer.empty() );
    _out_buffer.pop_front();
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
set_op_number( size_t op_number_ )
{
    _op_number = op_number_;
}



SP_TEMPLATE_STRING
size_t
SP_CLASS_NAME::
get_op_number() const
{
    return _op_number;
}


#undef SP_TEMPLATE_STRING
#undef SP_CLASS_NAME


} // namespace stream_process

#endif

