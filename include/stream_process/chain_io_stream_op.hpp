#ifndef __STREAM_PROCESS__CHAIN_IO_STREAM_OP__HPP__
#define __STREAM_PROCESS__CHAIN_IO_STREAM_OP__HPP__

#include <stream_process/stream_process_types.hpp>
#include <stream_process/attribute_accessor.hpp>

#include <vector>

namespace stream_process
{

template< typename operator_base_t >
class chain_io_stream_op : public operator_base_t
{
public:
	typedef operator_base_t				super;
	typedef typename super::sp_types_t	sp_types_t;
	
	STREAM_PROCESS_TYPES
	
	chain_io_stream_op( bool is_input_ );
	
	virtual void push( slice_type* slice );
	
	virtual void prepare_setup();
	
    virtual bool is_multithreadable() const		{ return false; }
    // chain-in doesn't need bounds checking, but chain-out does...
    virtual bool needs_bounds_checking() const	{ return ! _is_input; }
	
    virtual stream_op_type* clone();

protected:
	std::vector< chain_op_type* >*	_chain_ops;
	const bool						_is_input;
	
}; // class chain_io_stream_op



template< typename operator_base_t >
chain_io_stream_op< operator_base_t >::
chain_io_stream_op( bool is_input_ )
	: _chain_ops( 0 )
	, _is_input( is_input_ )
{}



template< typename operator_base_t >
void
chain_io_stream_op< operator_base_t >::
push( slice_type* slice )
{
    assert( slice );

	typename std::vector< chain_op_type* >::iterator
		it		= _chain_ops->begin(),
		it_end	= _chain_ops->end();
	
	if ( _is_input )
	{
		for( ; it != it_end; ++it )
		{
			(*it)->insert( slice );
		}
	}
	else
	{
		for( ; it != it_end; ++it )
		{
			(*it)->remove( slice );
		}
	}
    super::_out_buffer.push_back( slice );
}



template< typename operator_base_t >
void
chain_io_stream_op< operator_base_t >::
prepare_setup()
{
	_chain_ops = &super::_stream_manager->get_chain_ops();
	assert( _chain_ops );
}



template< typename operator_base_t >
typename chain_io_stream_op< operator_base_t >::stream_op_type* 
chain_io_stream_op< operator_base_t >::
clone()
{
	return 0;
}


} // namespace stream_process

#endif

