#ifndef __STREAM_PROCESS__STREAM_PROCESS_OPTIONS__HPP__
#define __STREAM_PROCESS__STREAM_PROCESS_OPTIONS__HPP__

#include <stream_process/options_base.hpp>
#include <map>
#include <string>

namespace stream_process
{

class stream_options : public options_base
{
public:
    stream_options();
    
    void finalize();

}; // class stream_options

} // namespace stream_process

#endif

