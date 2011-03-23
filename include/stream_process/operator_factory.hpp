#ifndef __STREAM_PROCESS__OPERATOR_FACTORY__HPP__
#define __STREAM_PROCESS__OPERATOR_FACTORY__HPP__

#include "op_options.h" 

namespace stream_process
{

//abstract factory base class
class stream_op;
class active_set_op;

class stream_op_factory
{
public:
	virtual ~stream_op_factory() {};
    virtual stream_op* create() = 0;
    virtual op_options& get_options() = 0;

};


class chain_op_factory
{
public:
	virtual ~chain_op_factory() {};
    virtual active_set_op* create() = 0;
    virtual op_options& get_options() = 0;

};



//default stream_op factory
template< 
    typename operator_t, 
    typename op_options_t = op_options
    >
class default_stream_op_factory: public stream_op_factory
{
public:
    virtual ~default_stream_op_factory() {}
    virtual stream_op* create();
    virtual op_options& get_options();
protected:
    op_options_t _op_options;
};



template< 
    typename operator_t, 
    typename op_options_t 
    >
stream_op*
default_stream_op_factory< operator_t, op_options_t >::create()
{
    return new operator_t();
}


template< 
    typename operator_t, 
    typename op_options_t 
    >
op_options&
default_stream_op_factory< operator_t, op_options_t >::get_options()
{
    return _op_options;
}



//default chain_op factory
template< 
    typename operator_t, 
    typename op_options_t = op_options
    >
class default_chain_op_factory : public chain_op_factory
{
public:
    virtual ~default_chain_op_factory() {}
    virtual active_set_op* create();
    virtual op_options& get_options();
protected:
    op_options_t _op_options;
};



template< 
    typename operator_t, 
    typename op_options_t 
    >
active_set_op*
default_chain_op_factory< operator_t, op_options_t >::create()
{
    return new operator_t();
}


template< 
    typename operator_t, 
    typename op_options_t 
    >
op_options&
default_chain_op_factory< operator_t, op_options_t >::get_options()
{
    return _op_options;
}


} // namespace stream_process

#endif

