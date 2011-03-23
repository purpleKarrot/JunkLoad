#ifndef __STREAM_PROCESS__CHAIN_OPERATOR__HPP__
#define __STREAM_PROCESS__CHAIN_OPERATOR__HPP__

namespace stream_process
{

class stream_config;
class point_batch;

template< typename operator_base_t >
class chain_operator : public operator_base_t
{
public:
	typedef typename operator_base_t::slice_type	slice_type;

    chain_operator() : operator_base_t() {}
	
    virtual ~chain_operator() {}

    virtual void insert( slice_type* data_slice_ )	= 0;
    virtual void remove( slice_type* data_slice_ )	= 0;

protected:

}; // class chain_operator

} // namespace stream_process

#endif

