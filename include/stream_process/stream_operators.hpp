#ifndef __STREAM_PROCESS__STREAM_OPERATORS__HPP__
#define __STREAM_PROCESS__STREAM_OPERATORS__HPP__

#include "op_container.hpp"

namespace stream_process
{

class stream_op;
class stream_operators : public op_container< stream_op >
{
public:
    stream_operators();
    ~stream_operators();
    
    // stream_op forwards
    void pull_push();
    void clear_stage();

    void check_requirements();

    stream_op* get_head();
    stream_op* get_tail();
	
	void push_back_operator( stream_op* op );
	

}; // class stream_op_chain

} // namespace stream_process

#endif

