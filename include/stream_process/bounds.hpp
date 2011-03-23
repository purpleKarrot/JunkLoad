#ifndef __STREAM_PROCESS__BOUNDS__HPP__
#define __STREAM_PROCESS__BOUNDS__HPP__

#include <limits>
#include <iostream>
#include <cassert>

namespace stream_process
{

template< typename float_t >
class bounds_1d
{
public:
    bounds_1d();

    inline bool ring_one_min_safe( const bounds_1d& other_bounds ) const;
    inline bool ring_one_max_safe( const bounds_1d& other_bounds ) const;
    
    inline void set( float_t min_z, float_t max_z, 
        float_t ring_one_min_z, float_t ring_one_max_z );
        
    inline void set( const bounds_1d& bounds_ );
        
    inline void update( float_t min_, float_t max_ );
    inline void update_ring_one( float_t ring_one_min_, float_t ring_one_max_ );

    float_t&    get_min();
    float_t&    get_max();
    float_t&    get_ring_one_min();
    float_t&    get_ring_one_max();

    const float_t&    get_min() const ;
    const float_t&    get_max() const ;
    const float_t&    get_ring_one_min() const ;
    const float_t&    get_ring_one_max() const ;
    
    
    void        set_max_index( size_t max_index );
    size_t&     get_max_index();
    
    void print() const
    {
        std::cout << "bounds [ " << _ring_one_min << " [ " << _min << " | "
            << _max << " ] " << _ring_one_max << " ] ";
    }

protected:
    float_t     _min;
    float_t     _max;
    float_t     _ring_one_min;
    float_t     _ring_one_max;
    
    size_t      _max_index;
    
}; // class bounds



template< typename float_t >
bounds_1d< float_t >::
bounds_1d()
	: _min( 0 )
	, _max( 1 )
	, _ring_one_min( -1 )
	, _ring_one_max( 0 )
    , _max_index( std::numeric_limits< size_t >::max() )
{}


template< typename float_t >
void
bounds_1d< float_t >::
set( float_t min_, float_t max_, float_t ring_one_min_,
    float_t ring_one_max_ )
{
    _min = min_;
    _max = max_;
    
    _ring_one_min = ring_one_min_;
    _ring_one_max = ring_one_max_;
}



template< typename float_t >
void
bounds_1d< float_t >::
update( float_t min_, float_t max_ )
{
    assert( min_ <= max_ );

    if ( min_ < _min )
        _min = min_;
    if ( max_ > _max )
        _max = max_;
}



template< typename float_t >
void
bounds_1d< float_t >::
update_ring_one( float_t ring_one_min_, float_t ring_one_max_ )
{
    if ( ring_one_min_ < _ring_one_min )
        _ring_one_min = ring_one_min_;
    if ( ring_one_max_ > _ring_one_max )
        _ring_one_max = ring_one_max_;
}



template< typename float_t >
bool
bounds_1d< float_t >::
ring_one_min_safe( const bounds_1d& bounds_ ) const
{
#if 1
    return _ring_one_min > bounds_._ring_one_max ? true : false;
#else
	const float_t& omin = bounds_._min;
	const float_t& omax = bounds_._max;

    if ( _ring_one_min > omax  )
        return true;
    return false;
#endif
}



template< typename float_t >
bool
bounds_1d< float_t >::
ring_one_max_safe( const bounds_1d& bounds_ ) const
{
    assert( _ring_one_min <= _min );
    assert( _min <= _max );
    assert( _max <= _ring_one_max );
    
#if 1
    return _ring_one_max < bounds_._ring_one_min ? true : false;
    
#else
	const float_t& omin = bounds_._min;
	const float_t& omax = bounds_._max;

    if ( _ring_one_max < omin )
        return true;
    return false;
#endif
}


template< typename float_t >
inline void
bounds_1d< float_t >::
set( const bounds_1d& bounds_ )
{
    _min = bounds_._min;
    _max = bounds_._max;
    _ring_one_min = bounds_._ring_one_min;
    _ring_one_max = bounds_._ring_one_max;
    
    _max_index = bounds_._max_index;
}


template< typename float_t >
float_t&
bounds_1d< float_t >::
get_min()
{
    return _min;
}



template< typename float_t >
float_t&
bounds_1d< float_t >::
get_max()
{
    return _max;
}


template< typename float_t >
float_t&
bounds_1d< float_t >::
get_ring_one_min()
{
    return _ring_one_min;
}



template< typename float_t >
float_t&
bounds_1d< float_t >::
get_ring_one_max()
{
    return _ring_one_max;
}



template< typename float_t >
const float_t&
bounds_1d< float_t >::
get_min() const
{
    return _min;
}



template< typename float_t >
const float_t&
bounds_1d< float_t >::
get_max() const
{
    return _max;
}


template< typename float_t >
const float_t&
bounds_1d< float_t >::
get_ring_one_min() const
{
    return _ring_one_min;
}



template< typename float_t >
const float_t&
bounds_1d< float_t >::
get_ring_one_max() const
{
    return _ring_one_max;
}



template< typename float_t >
void
bounds_1d< float_t >::
set_max_index( size_t max_index_ )
{
    _max_index = max_index_;
}



template< typename float_t >
size_t&
bounds_1d< float_t >::
get_max_index()
{
    return _max_index;
}



} // namespace stream_process

#endif

