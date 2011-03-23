#ifndef __STREAM_PROCESS__NAN_INF_TEST__HPP__
#define __STREAM_PROCESS__NAN_INF_TEST__HPP__

#include <cmath>

#include <stream_process/exception.hpp>

namespace stream_process
{


template< typename T >
bool
is_nan_or_inf( const T& t_, bool except_on_nan_inf = true )
{
    bool ok = true;
    if ( std::isnan( t_ ) || std::isinf( t_ ) )
    {
        ok = false;
    }
    
    if ( ! ok )
    {
        std::cout << "invalid float value: " << t_ << std::endl;
        if ( except_on_nan_inf )
        {
            throw exception( "nan/inf found. aborting...", SPROCESS_HERE );
        }
    }

    return !ok;

}


template< typename T >
bool
contains_nan_or_inf( const T& t_, bool except_on_nan_inf = true )
{
    bool ok = true;

    typename T::const_iterator it = t_.begin(), it_end = t_.end();
    for( ; it != it_end; ++it )
    {
        //typename const T::value_type& v = *it;
        
        if ( std::isnan( *it ) || std::isinf( *it ) )
        {
            ok = false;
            break;
        }
    }
    
    if ( ! ok )
    {
        std::cout << "invalid float value: " << t_ << std::endl;
        if ( except_on_nan_inf )
        {
            throw exception( "nan/inf found. aborting...", SPROCESS_HERE );
        }
    }

    return !ok;
}


} // namespace stream_process

#endif

