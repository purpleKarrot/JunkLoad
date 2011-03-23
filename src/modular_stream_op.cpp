#include "modular_stream_op.hpp"

namespace stream_process
{

modular_stream_op::modular_stream_op( op_algorithm* op_algorithm_, 
    in_out_strategy< uint32_t>* in_out_strategy_ )
    : _op_algorithm(    op_algorithm_ )
    , _in_out_strategy( in_out_strategy_ )
    , _single_threaded( false )
    , _thread_count( 8 )
{
    assert( _op_algorithm );
    assert( _in_out_strategy );
    set_name( _op_algorithm->get_name() );
        
    // _single_threaded = global_single ? true : _op_algorithm->is_multi_threadable();
    _single_threaded = ! _op_algorithm->is_multi_threadable();    
}


modular_stream_op::~modular_stream_op()
{
    // delete cloned op_algo instances
    std::vector< job >::iterator it     = _jobs.begin();
    std::vector< job >::iterator it_end = _jobs.end();
    for( ; it != it_end; ++it )
    {
        delete (*it).instance;
    }

    delete _op_algorithm;
    delete _in_out_strategy;
}



void
modular_stream_op::pull_push()
{
	stream_point* point;
	while( point = prev->front() )
	{
		_in_out_strategy->push_back( point );
        prev->pop_front();
	}
    if ( _single_threaded )
    {
        while( point = _in_out_strategy->op_top() )
        {
            _in_out_strategy->op_pop();
            _op_algorithm->run( point );
            _in_out_strategy->op_push( point );
        }
    }
    else
	{
        if ( _in_out_strategy->in_size() > 2048 )
            _run_threads();
    }
}


void
modular_stream_op::clear_stage()
{
    if ( _in_out_strategy->in_size() )
    {
        if ( ! _single_threaded )
        {
            _run_threads();
        }
        else
        {
            throw exception( "TODO, impl clear stage in in_out_strategy", 
            SPROCESS_HERE );
        }
    }
}



void
modular_stream_op::_run_threads()
{

    _in_out_strategy->get_batch( _ranges );

#if 1
    std::vector< boost::thread* > threads;
    threads.reserve( _thread_count );

    for( size_t index = 0; index < _thread_count; ++index )
    {
        threads.push_back( new boost::thread( _jobs[ index ] ) );
    }
    for( ssize_t index = _thread_count - 1; index > -1; --index )
    {
        threads[ index ]->join();
        delete threads[ index ];
        //_jobs[ index].instance->print_debug();
    }
#else

    for( size_t i = 0; i < _ranges.size(); ++i )
    {
        _op_algorithm->run( _ranges[ i ] );
    }
#endif    
    _in_out_strategy->batch_complete( _ranges );

}



size_t
modular_stream_op::size() const
{
    return _in_out_strategy->size();
}


void
modular_stream_op::attach_to( stream_op* prev_ )
{
    prev = prev_;
    assert( _in_out_strategy );
    _in_out_strategy->set_previous_op( prev );
}


// out strategy
stream_point*
modular_stream_op::front()
{
	return _in_out_strategy->front();
}


void
modular_stream_op::pop_front()
{
	_in_out_strategy->pop_front();
}


// smallest unprocessed element in this or previous stages
size_t
modular_stream_op::smallest_element()
{
    size_t smallest = static_cast< size_t >( _in_out_strategy->smallest_element() );
    return prev ? std::min( smallest, prev->smallest_element() ) : smallest;
}


// smallest reference of any unprocessed element in this or previous stages
size_t
modular_stream_op::smallest_reference()
{
    size_t smallest = static_cast< size_t >( _in_out_strategy->smallest_reference() );
    return prev ? std::min( smallest, prev->smallest_reference() ) : smallest;
}



// op_algorithm 
bool
modular_stream_op::does_provide( const std::string& name, data_type_id type_ )
{
	bool does_provide_ = _op_algorithm->does_provide( name, type_ );
    if ( ! does_provide_ && prev )
        return prev->does_provide( name, type_ );
    else
        return does_provide_;
}



void
modular_stream_op::check_requirements() const
{
	_op_algorithm->check_requirements();
}


void
modular_stream_op::setup_stage_0()
{
	_op_algorithm->setup_stage_0();
}


void
modular_stream_op::setup_stage_1()
{
	_op_algorithm->setup_stage_1();
}


void
modular_stream_op::setup_stage_2()
{
	_op_algorithm->setup_stage_2();

    if ( ! _single_threaded )
        _setup_threading();
}


void
modular_stream_op::prepare_processing()
{
	_op_algorithm->prepare_processing();

}


void 
modular_stream_op::prepare_shutdown()
{
	_op_algorithm->prepare_shutdown();
}



void
modular_stream_op::_setup_threading()
{
    _threads.resize( _thread_count );
    _jobs.resize( _thread_count );
    _ranges.resize( _thread_count );
    
    for( size_t index = 0; index < _thread_count; ++index )
    {
        _jobs[ index ].instance     = _op_algorithm->clone();
        _jobs[ index ].range        = &_ranges[ index ];
    }

}



std::string
modular_stream_op::about() const
{
    return _op_algorithm->about();
}



} // namespace stream_process

