#include "op_algorithm.hpp"

namespace stream_process
{

chain_config*   op_algorithm::_config           = 0;
chain_manager*  op_algorithm::_chain_manager    = 0;

op_algorithm::op_algorithm()
{
    set_name( "yet unnamed op algorithm" );
}


op_algorithm::~op_algorithm()
{}


void
op_algorithm::run( stream_point* point )
{
	_compute( point );
}



void
op_algorithm::run( job_range& range_ )
{
    stream_point** point    = range_.begin;
    stream_point** end      = range_.end;

    for( ; point != end; ++point )
    {
        _compute( *point );
    }
}



void
op_algorithm::_compute( stream_point* point )
{
	//implement operator functionality here.
    std::cout << "please implement op_algorithm functionality." << std::endl;
    assert( 0 );
}


bool
op_algorithm::does_provide( const std::string& name, data_type_id type_ )
{
	return false;
}

void
op_algorithm::check_requirements() const
{}


void
op_algorithm::setup_stage_0()
{}

void
op_algorithm::setup_stage_1()
{}

void
op_algorithm::setup_stage_2()
{}

void
op_algorithm::prepare_processing()
{}

void 
op_algorithm::prepare_shutdown()
{}


bool
op_algorithm::is_multi_threadable()
{
    return false;
}


std::string
op_algorithm::about() const
{
    return "no description.";
}

} // namespace stream_process

