#ifndef __STREAM_PROCESS__MODULAR_OPERATOR_FACTORY__HPP__
#define __STREAM_PROCESS__MODULAR_OPERATOR_FACTORY__HPP__

#include "operator_factory.hpp"
#include "modular_stream_op.hpp"
#include "op_options.h" 

namespace stream_process
{
template< 
    typename op_algorithm_t,
    typename in_out_strategy_t = default_in_out_strategy,
    typename op_options_t = op_options
    >
class modular_stream_operator_factory : public stream_op_factory
{

public:
	modular_stream_operator_factory(){};
	virtual ~modular_stream_operator_factory(){};
    virtual stream_op* create();
    virtual op_options& get_options();

protected:
    op_options_t    _op_options;

}; // class modular_stream_operator_factory

template< 
    typename op_algorithm_t,
    typename in_out_strategy_t,
    typename op_options_t
    >
stream_op*
modular_stream_operator_factory< 
    op_algorithm_t,
    in_out_strategy_t,
    op_options_t
    >::create()
{
    modular_stream_op* op = new modular_stream_op
        (
            new op_algorithm_t(),
            new in_out_strategy_t()
        );
    return op;
}



template< 
    typename op_algorithm_t,
    typename in_out_strategy_t,
    typename op_options_t
    >
op_options&
modular_stream_operator_factory< 
    op_algorithm_t,
    in_out_strategy_t,
    op_options_t
    >::get_options()
{
    return _op_options;
}

} // namespace stream_process

#endif

