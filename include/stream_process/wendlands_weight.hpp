#ifndef __STREAM_PROCESS__WENDLANDS_WEIGHT__HPP__
#define __STREAM_PROCESS__WENDLANDS_WEIGHT__HPP__

#include <stream_process/stream_process_types.hpp>

namespace stream_process
{

// implementation of a wendlands weight functor [wendland 1995]

template< typename T >
class wendlands_weight
{
public:
    wendlands_weight( T h = 1.0 ) : _h ( h ) {}
    
    T operator()( T r ) const
    {
        if ( r > _h )
            return 0;

        T r_h = r / _h;
        
        T t0 = - r_h + 1.0;
        
        t0 *= t0;
        t0 *= t0;
        t0 *= t0;

        return t0 * ( ( r_h * 4.0 ) + 1.0 );
    }

    void operator()( T h, size_t dummy )
    {
        set_feature_size( h );
    }

    void set_feature_size( T h )
    {
        _h = h;
    }

private:
    T _h;

}; // class wendlands_weight

} // namespace stream_process

#endif

