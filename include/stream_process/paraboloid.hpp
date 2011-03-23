#ifndef __STREAM_PROCESS__PARABOLOID__HPP__
#define __STREAM_PROCESS__PARABOLOID__HPP__

#include <stream_process/stream_process_types.hpp>
#include <stream_process/exception.hpp>

#include <boost/function.hpp>

namespace stream_process
{

template< typename T >
class paraboloid
{
public:
    paraboloid();
    
    void initialize( T a2_, T b2_ );
    
    T       get_a2() const;
    void    set_a2( T a2_ );
    
    T       get_b2() const;
    void    set_b2( T b2_ );
    
    inline T compute_z_hyperbolic( T x, T y ) const;
    inline T compute_z_hyperbolic( const vmml::vector< 3, T >& point ) const;
    
    inline T compute_z_elliptic( T x, T y ) const;
    inline T compute_z_elliptic( const vmml::vector< 3, T >& point ) const;
    
protected:
    T   _a2;
    T   _b2;
    

}; // class paraboloid


template< typename T >
paraboloid< T >::
paraboloid()
    : _a2( 1.0 )
    , _b2( 1.0 ) 
{}


template< typename T >
void
paraboloid< T >::
initialize( T a2_, T b2_ )
{
    if ( a2_ == 0 || b2_ == 0 )
    {
        ERROR( "attempted to set paraboloid parameters to 0." );
    }

    _a2 = a2_;
    _b2 = b2_; 
}



template< typename T >
T
paraboloid< T >::
get_a2() const
{
    return _a2;
}



template< typename T >
void
paraboloid< T >::
set_a2( T a2_ )
{
    _a2 = a2_;
}



template< typename T >
T
paraboloid< T >::
get_b2() const
{
    return _b2;
}



template< typename T >
void
paraboloid< T >::
set_b2( T b2_ )
{
    _b2 = b2_;
}



template< typename T >
T
paraboloid< T >::
compute_z_hyperbolic( T x, T y ) const
{
    return ( ( x * x ) / _a2 ) - ( ( y * y ) / _b2 );
}



template< typename T >
T
paraboloid< T >::
compute_z_hyperbolic( const vmml::vector< 3, T >& point ) const
{
    return compute_z_hyperbolic( point.x(), point.y() );
}



template< typename T >
T
paraboloid< T >::
compute_z_elliptic( T x, T y ) const
{
    return ( ( x * x ) / _a2 ) + ( ( y * y ) / _b2 );
}



template< typename T >
T
paraboloid< T >::
compute_z_elliptic( const vmml::vector< 3, T >& point ) const
{
    return compute_z_elliptic( point.x(), point.y() );
}


} // namespace stream_process
#endif

