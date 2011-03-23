#include <stream_process/multi_timer.hpp>

#include <iostream>

namespace stream_process
{
#ifdef __APPLE__
mach_timebase_info_data_t   multi_timer::_timebase_info;
double                      multi_timer::_time_factor;
#endif

multi_timer::multi_timer()
    : _total_time( 0 )
{

#ifdef __APPLE__
    if ( _timebase_info.denom == 0 )
    {
        mach_timebase_info( &_timebase_info );
        _time_factor = (double) _timebase_info.numer / _timebase_info.denom;
        _time_factor /= 1e9;
    }
#endif
}



void
multi_timer::reset()
{
    _total_time = 0;
}


double
multi_timer::get_elapsed_time() const
{
    return _total_time;
}



double
multi_timer::get_current_time()
{
    #ifdef __LINUX__
        return (double) clock() / CLOCKS_PER_SEC;
    #else
        #ifdef __APPLE__
            return static_cast< double > ( mach_absolute_time() * _time_factor );
        #else
            timeval now;
            gettimeofday( &now, 0 );
            return now.tv_sec + ( (double)now.tv_usec * 1e-6 );
        #endif
    #endif
}



double
multi_timer::get_seconds()
{
    #ifdef __APPLE__
        _t_end = mach_absolute_time();
        _tmp_time = static_cast< double > ( _t_end - _t_begin );
        _tmp_time *= _time_factor;
    #else
        #ifdef __LINUX__
            _tmp = clock();
            _tmp_time = static_cast< double >( _tmp - _t_begin );
            _tmp_time /= CLOCKS_PER_SEC;
        #else
            gettimeofday( &_t_end, 0 );
            
            _total_time += static_cast< double >( _t_end.tv_sec - _t_begin.tv_sec );
            _tmp_time = 1e-6 * static_cast< double >( _t_end.tv_usec - _t_begin.tv_usec  );
        #endif
    #endif
    return _tmp_time;
}


} // namespace stream_process

