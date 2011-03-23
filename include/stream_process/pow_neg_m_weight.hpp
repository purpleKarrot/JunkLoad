#ifndef __STREAM_PROCESS__POW_NEG_WEIGHT__HPP__
#define __STREAM_PROCESS__POW_NEG_WEIGHT__HPP__

#include <stream_process/VMMLibIncludes.h>

namespace stream_process
{

template< typename T >
class pow_neg_m_weight
{
public:
    pow_neg_m_weight( T m = 2.0 ) : _m( m ) {}
    
    T operator()( T z )
    {
        return powf( z, -_m );
    }
    
    void operator()( T h, size_t dummy )
    {
    }

    void set_feature_size( T h )
    {}
    
protected:

    T _m;

}; // class pow_neg_weight


template<>
double
pow_neg_m_weight< double >::operator()( double z )
{
    return pow( z, -_m );
}


} // namespace stream_process

#endif

