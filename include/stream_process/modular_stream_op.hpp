#ifndef __STREAM_PROCESS__MODULAR_STREAM_OPERATOR__HPP__
#define __STREAM_PROCESS__MODULAR_STREAM_OPERATOR__HPP__

#include "op_algorithm.hpp"
#include "in_out_strategy.hpp"
#include "data_types.hpp"

#include "stream_op.h"

#include <string>

#include <boost/thread.hpp>

namespace stream_process
{

struct job
{
    job_range*          range;
    op_algorithm*       instance;
    void operator()()   { instance->run( *range ); };
};

class modular_stream_op : public stream_op
{
public:
    modular_stream_op( op_algorithm* op_algorithm_, 
        in_out_strategy< uint32_t>* in_out_strategy_ );
	virtual ~modular_stream_op();
  
	virtual void pull_push();
	virtual void clear_stage();

	virtual void attach_to( stream_op* prev_ );

	// out strategy
	virtual stream_point* front();
	virtual void pop_front();
	// smallest unprocessed element in this or previous stages
    virtual inline size_t smallest_element();	
    // smallest reference of any unprocessed element in this or previous stages
    virtual inline size_t smallest_reference();	

	virtual size_t size() const;


	// op_algorithm 
	virtual bool does_provide( const std::string& name, data_type_id type_ );
	virtual void check_requirements() const;

	virtual void setup_stage_0();
	virtual void setup_stage_1();     
	virtual void setup_stage_2();

	virtual void prepare_processing();
	virtual void prepare_shutdown();

    virtual std::string about() const;

protected:
    virtual void _run_threads();
    virtual void _setup_threading();
    //stream_operator*			_previous_op;
	
	op_algorithm*				_op_algorithm;
	in_out_strategy< uint32_t>*	_in_out_strategy;
	
    bool                        _single_threaded;
    
    size_t                      _thread_count;
    
    std::vector< boost::thread* >   _threads;
    std::vector< job_range >        _ranges;
    std::vector< job >              _jobs;

}; // class modular_stream_op

} // namespace stream_process

#endif

