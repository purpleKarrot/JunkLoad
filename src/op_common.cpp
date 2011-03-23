#include "op_common.hpp"

#include <sstream>

namespace stream_process
{

chain_manager*       op_common::_chain_manager = 0;
chain_config*        op_common::_config = 0;
active_set*          op_common::_active_set = 0;
point_info*          op_common::_point_info = 0;


const multi_timer&
op_common::get_timer()
{
    return _multi_timer;
}


void
op_common::set_point_info( point_info* point_info_ )
{
    _point_info = point_info_;
}



void
op_common::set_chain_manager( chain_manager* chain_manager_ )
{
    _chain_manager = chain_manager_;
}


void 
op_common::set_chain_config( chain_config* chain_config_ )
{
    _config = chain_config_;
}



void
op_common::set_active_set( active_set* active_set_ )
{
    _active_set = active_set_;
}



std::string
op_common::get_timer_string() const
{
    std::stringstream stream;
    stream << std::setw( 24 ) << _name + ": " << _multi_timer.get_elapsed_time();
    return stream.str();
}



} // namespace stream_process

