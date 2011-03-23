#ifndef __STREAM_PROCESS__SORT_MODE__HPP__
#define __STREAM_PROCESS__SORT_MODE__HPP__

#include <stream_process/options_base.hpp>

namespace stream_process
{

class sort_mode
{
public:
    sort_mode( const options_map& options_base );
    
    void print_usage();

protected:
    const options_map&      _options;

}; // class sort_mode

} // namespace stream_process

#endif

