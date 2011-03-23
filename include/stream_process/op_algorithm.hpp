#ifndef __STREAM_PROCESS__OP_ALGORITHM__HPP__
#define __STREAM_PROCESS__OP_ALGORITHM__HPP__


#include "stream_point.h"
#include "rt_struct_member.h"
#include "data_types.hpp"

#include "in_out_strategy.hpp"
#include "job_range.hpp"

#include "rt_struct_user.h"
#include "chain_config.h"
#include "options.h"

#include <queue>

namespace stream_process
{
class chain_manager;

class op_algorithm : public rt_struct_user
{
public:
    op_algorithm();
	virtual ~op_algorithm();
    
    virtual op_algorithm* clone() = 0;
	
	virtual void run( stream_point* point );
	virtual void run( job_range& range_ );

	// TODO replace with list< string >& get_outputs()
	// and do check in stream_operator class
	virtual bool does_provide( const std::string& name, data_type_id type_ );
	// TODO replace with list< string >& get_requirements()
	// and do check in stream_operator class
	virtual void check_requirements() const;

	virtual void setup_stage_0();
	virtual void setup_stage_1();     
	virtual void setup_stage_2();

	virtual void prepare_processing();
	virtual void prepare_shutdown();
    
    virtual bool is_multi_threadable();
    
    virtual std::string about() const;

    static chain_config*            _config;
    static chain_manager*           _chain_manager;
    
protected:
	virtual void _compute( stream_point* point );

	in_out_strategy< uint32_t >*	_in_out_strategy;
    
}; // class op_algorithm

} // namespace stream_process

#endif

