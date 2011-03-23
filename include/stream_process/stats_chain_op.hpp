#ifndef __STREAM_PROCESS__STATS_CHAIN_OP__HPP__
#define __STREAM_PROCESS__STATS_CHAIN_OP__HPP__

#include <stream_process/stream_process_types.hpp>

#include <stream_process/timer.hpp>

namespace stream_process
{

template< typename operator_base_t >
class stats_chain_op : public operator_base_t
{
public:
	typedef operator_base_t             super;
    typedef typename super::sp_types_t  sp_types_t;

    STREAM_PROCESS_TYPES
    
    stats_chain_op();

    virtual void insert( slice_type* slice );
    virtual void remove( slice_type* slice );
    
    virtual void prepare_shutdown();

protected:
    size_t          _number_of_slices;
    size_t          _max_number_of_slices;
    timer           _timer;

}; // class stats_chain_op

#define SP_TEMPLATE_STRING \
    template< typename operator_base_t >
    
#define SP_CLASS_NAME \
    stats_chain_op< operator_base_t >


SP_TEMPLATE_STRING
SP_CLASS_NAME::stats_chain_op()
    : _number_of_slices( 0 )
    , _max_number_of_slices( 0 )
{
    _timer.start();
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::insert( slice_type* slice )
{
    ++_number_of_slices;
    if ( _number_of_slices > _max_number_of_slices )
        _max_number_of_slices = _number_of_slices;
        
    if ( _timer.get_seconds() > 5.0 )
    {
        size_t batch_size = super::_stream_config->get_options().get("slice-size").get_int();

        std::cout
            << "in-slice: " << slice->get_slice_number()
            << ", active set: " << _number_of_slices
            << " slices, ~" << _number_of_slices * batch_size << " vertices"
            << ", max active set: " << _max_number_of_slices
            << " slices, ~" << _max_number_of_slices * batch_size << " vertices."
            << std::endl;

        _timer.start();
    }
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::remove( slice_type* slice )
{
    --_number_of_slices;
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::prepare_shutdown()
{
    size_t batch_size = super::_stream_config->get_options().get("slice-size").get_int();

    std::cout
        << "max active set: " << _max_number_of_slices
        << " slices  (~" << _max_number_of_slices * batch_size << " vertices)."
        << std::endl;
}


#undef SP_TEMPLATE_STRING
#undef SP_CLASS_NAME

} // namespace stream_process

#endif

