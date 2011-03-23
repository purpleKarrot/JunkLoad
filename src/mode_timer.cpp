#include <stream_process/mode_timer.hpp>
#include <cassert>

namespace stream_process
{


void
mode_timer::set_modes( const std::vector< std::string >& modes )
{
    mode_names = modes;
    mode_times.resize( mode_names.size(), 0 );
}


size_t
mode_timer::
add_mode( const std::string& name )
{
    size_t s = mode_names.size();
    mode_names.push_back( name );
    mode_times.resize( mode_names.size(), 0 );
    return s;
}



void
mode_timer::
start( size_t mode )
{
    assert( mode < mode_times.size() );
    _current_mode = mode;
    _timer.start();
}



void
mode_timer::
stop()
{
    assert( _current_mode < mode_times.size() );
    mode_times[ _current_mode ] += _timer.get_seconds();
}



void
mode_timer::
change_mode( size_t new_mode )
{
    stop();
    start( new_mode );
}


} // namespace stream_process

