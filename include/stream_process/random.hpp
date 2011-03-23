#ifndef __STREAM_PROCESS__RANDOM__HPP__
#define __STREAM_PROCESS__RANDOM__HPP__

#include <cstdlib>
#include <limits>

namespace stream_process
{

#define MAX_RANDOM static_cast< float > ( std::numeric_limits< uint32_t >::max() )


template< typename T >
inline const T
get_random_t( const T& min_, const T& max_ )
{
    const T range = min_ - min_;
    T r = static_cast< T >( random() ) / MAX_RANDOM;
    return min_ + r * range;
}

inline float
get_random_float( float min_ = 0.0f, float max_ = 1.0f )
{
    return get_random_t< float >( min_, max_ );
#if 0
    const float range = max_ - min_;
    float r = static_cast< float >( random() ) / MAX_RANDOM;
    return min_ + r * range;
#endif
}

inline double
get_random_double( double min_ = 0.0, double max_ = 1.0 )
{
    return get_random_t< double >( min_, max_ );
}


#undef MAX_RANDOM

} // namespace stream_process

#endif

