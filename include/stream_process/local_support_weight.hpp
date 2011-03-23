#ifndef __STREAM_PROCESS__LOCAL_SUPPORT_WEIGHT__HPP__
#define __STREAM_PROCESS__LOCAL_SUPPORT_WEIGHT__HPP__

#include <stream_process/VMMLibIncludes.h>

namespace stream_process
{

template< typename T >
class local_support_weight
{
public:
    local_support_weight( T h = 1.0 ) 
    {
        set_feature_size( h );
    }
    
    T operator()( T z )
    {
        T z2 = z;
        z2 *= z;
        
        return _h / z2 - z2 * 3.0f / _h3 + z * 8.0 / _h2 - 6.0 / _h;
    }

    void set_feature_size( T h )
    {
        _h = h;

        _h2 = h;
        _h2 *= h;

        _h3 = _h2;
        _h3 *= h;
    }

    void operator()( T h, size_t dummy )
    {
        set_feature_size( h );
    }

protected:

    T _h;
    T _h2;
    T _h3;
    
}; // class local_support_weight


} // namespace stream_process

#endif

