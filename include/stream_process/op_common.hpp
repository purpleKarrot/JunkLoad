#ifndef __STREAM_PROCESS__OP_COMMON__HPP__
#define __STREAM_PROCESS__OP_COMMON__HPP__

#include "rt_struct_user.h"
#include "chain_config.h"
#include "multi_timer.hpp"

namespace stream_process
{

class chain_manager;
class active_set;
class point_info;
class mapped_point_data;

/**
*   @brief common base class for stream ops and active_set ops
*/

class op_common : public rt_struct_user
{
public:
    static void set_chain_manager( chain_manager* chain_manager_ );
    static void set_chain_config( chain_config* chain_config_ );
    static void set_active_set( active_set* active_set_ );
    static void set_point_info( point_info* point_info_ );

    inline void start_timer();
    inline void stop_timer();
    const multi_timer& get_timer();
    
    std::string get_timer_string() const;
    
protected:
    multi_timer             _multi_timer;
    static chain_manager*   _chain_manager;
    static chain_config*    _config;
    static active_set*      _active_set;
    static point_info*      _point_info;

}; // class op_common



inline void
op_common::start_timer()
{
    _multi_timer.start();
}



inline void
op_common::stop_timer()
{
    _multi_timer.stop();
}

} // namespace stream_process

#endif

