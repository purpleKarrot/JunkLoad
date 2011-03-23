#ifndef __STREAM_PROCESS__MODE_TIMER__HPP__
#define __STREAM_PROCESS__MODE_TIMER__HPP__

#include <stream_process/timer.hpp>

#include <vector>
#include <string>

namespace stream_process
{

class mode_timer
{
public:
    mode_timer() : _current_mode( 0 ) {}

    size_t add_mode( const std::string& name );

    void set_modes( const std::vector< std::string >& modes );
    
    void start( size_t mode = 0 );
    void stop();
    void change_mode( size_t new_mode );

    std::vector< double >       mode_times;
    std::vector< std::string >  mode_names;
    
protected:
    timer                       _timer;
    size_t                      _current_mode;
    
}; // class mode_timer




} // namespace stream_process

#endif

