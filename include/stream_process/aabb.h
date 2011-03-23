#ifndef __VMML__AABB__H__
#define __VMML__AABB__H__

#include <iostream>

#include "VMMLibIncludes.h"

namespace stream_process
{

template< typename T,
          class vector_t,
          unsigned int dimensions
          >
class aabb
{
public:
    typedef vector_t vec;
    
    aabb();
    aabb( const vec& min, const vec& max );
    
    inline void set( const vec& min, const vec& max );
    inline const aabb& operator=( const aabb& aabb_ ); 

    const vec& get_min() const;
    const vec& get_max() const;
    
    inline const vec get_center() const;
    inline const T get_center_of_axis( size_t axis ) const;

    inline void merge( const aabb& aabb_ );
    
    // kd-tree split ( split into two aabbs )
    inline void split_kd( const size_t axis, const T split_value,
        aabb& child0, aabb& child1 ) const;

    inline void split_kd( const size_t axis, const T split_value,
        aabb& child_aabb, size_t child_number ) const;

    // quadtree split ( split into four aabbs )
    void split_qt( const vec& split, aabb& child0, aabb& child1,
        aabb& child2, aabb& child3 ) const;

    bool is_inside( const vec& point ) const;

    friend std::ostream& operator<<( std::ostream& o, 
        const aabb& box )
    {
        o << "aabb:" << box._min << " " << box._max << std::endl;
        return o;
    }
    
        
protected:
    vec _min;
    vec _max;
      
}; //class aabb

#ifdef SPROCESS_OLD_VMMLIB

typedef aabb< float,	vmml::Vector2< float >, 2 >     aabb2f;
typedef aabb< double,	vmml::Vector2< double >, 2 >	aabb2d;
typedef aabb< float,	vmml::Vector3< float >, 3 >     aabb3f;
typedef aabb< double,	vmml::Vector3< double >, 3 >	aabb3d;

#else

typedef aabb< float,	vmml::vector< 2, float >, 2 >   aabb2f;
typedef aabb< double,	vmml::vector< 2, double >, 2 >	aabb2d;
typedef aabb< float,	vmml::vector< 3, float >, 3 >   aabb3f;
typedef aabb< double,	vmml::vector< 3, double >, 3 >	aabb3d;

#endif

template< typename T, typename vector_t, unsigned int dimensions >
aabb< T, vector_t, dimensions >
    ::aabb()
{}



template< typename T, typename vector_t, unsigned int dimensions >
aabb< T, vector_t, dimensions >
    ::aabb( const vec& min, const vec& max )
    : _min( min )
    , _max( max )
{}



template< typename T, typename vector_t, unsigned int dimensions >
inline void aabb< T, vector_t, dimensions >
    ::set( const vec& min, const vec& max )
{
        memcpy( &_min, &min, sizeof( vec ) );
        memcpy( &_max, &max, sizeof( vec ) );
}



template< typename T, typename vector_t, unsigned int dimensions >
inline const aabb< T, vector_t, dimensions >& aabb< T, vector_t, dimensions >
    ::operator=( const aabb< T, vector_t, dimensions >& aabb_ )
{
        memcpy( this, &aabb_, sizeof( aabb ) );
        return *this;
}



template< typename T, typename vector_t, unsigned int dimensions >
inline const vector_t&  aabb< T, vector_t, dimensions >
    ::get_min() const
{
    return _min;
}



template< typename T, typename vector_t, unsigned int dimensions >
inline const vector_t&  aabb< T, vector_t, dimensions >
    ::get_max() const
{
    return _max;
}



template< typename T, typename vector_t, unsigned int dimensions >
inline const vector_t  aabb< T, vector_t, dimensions >
    ::get_center() const
{
    return _min + ( ( _max - _min ) * 0.5 );
}



template< typename T, typename vector_t, unsigned int dimensions >
inline const T aabb< T, vector_t, dimensions >
    ::get_center_of_axis( size_t axis ) const
{
    assert( axis < dimensions );
    return _min[ axis ] + ( ( _max[ axis ] - _min[ axis ] ) * 0.5 );
}




template< typename T, typename vector_t, unsigned int dimensions >
inline void  aabb< T, vector_t, dimensions >
    ::merge( const aabb< T, vector_t, dimensions >& aabb_ ) 
{
    _min.x = std::min( _min.x, aabb_._min.x );
    _min.y = std::min( _min.y, aabb_._min.y );
    _max.x = std::max( _max.x, aabb_._max.x );
    _max.y = std::max( _max.y, aabb_._max.y );
}



// kd-tree split
template< typename T, typename vector_t, unsigned int dimensions >
void  aabb< T, vector_t, dimensions >::split_kd( const size_t axis, 
    const T split_value, 
    aabb< T, vector_t, dimensions >& child0, 
    aabb< T, vector_t, dimensions >& child1 
    ) const
{
    assert( axis < dimensions );
    //assert( _min[ axis ] < split_value && _max[ axis ] > split_value ); 
    if ( ! ( _min[ axis ] < split_value && _max[ axis ] > split_value ) )
    {
        //std::cout << *this << std::endl;
        //std::cout << "split_value " << split_value << " axis " << axis << std::endl;
    };
   
    vector_t tmp( _max );
    tmp[ axis ] = split_value;
    child0.set( _min, tmp );

    tmp = _min;
    tmp[ axis ] = split_value;
    child1.set( tmp, _max );
    //std::cout << "axis " << axis << " split " << split_value << std::endl;
    //std::cout << *this;
    //std::cout << "c0 " << child0;
    //std::cout << "c1 " << child1 << std::endl;
    
}



template< typename T, typename vector_t, unsigned int dimensions >
void  aabb< T, vector_t, dimensions >::split_kd( const size_t axis, 
    const T split_value, aabb& child_aabb, const size_t child_number ) const
{
    assert( axis < dimensions );
    assert( _min[ axis ] < split_value && _max[ axis ] > split_value ); 
 
    if ( child_number == 0 )
    {
        vector_t tmp( _max );
        tmp[ axis ] = split_value;
        child_aabb.set( _min, tmp );
    }
    else
    {
        vector_t tmp( _min );
        tmp[ axis ] = split_value;
        child_aabb.set( tmp, _max );
    }
    //std::cout << "axis " << axis << " split " << split_value << std::endl;
    //std::cout << *this;
    //std::cout << "c0 " << child0;
    //std::cout << "c1 " << child1 << std::endl;
    
}



// quadtree split
template< typename T, typename vector_t, unsigned int dimensions >
void aabb< T, vector_t, dimensions >::split_qt( const vector_t& split, 
    aabb< T, vector_t, dimensions >& child0,
    aabb< T, vector_t, dimensions >& child1,
    aabb< T, vector_t, dimensions >& child2,
    aabb< T, vector_t, dimensions >& child3
    ) const
{
    child0.set( _min, split );
    child3.set( split, _max );

    vector_t tmp_min( _min );
    vector_t tmp_max( _max );
    tmp_min[ 0 ] = split[ 0 ];
    tmp_max[ 1 ] = split[ 1 ];
    
    child1.set( tmp_min, tmp_max );
    
    tmp_min = _min;
    tmp_max = _max;
    tmp_min[ 1 ] = split[ 1 ];
    tmp_max[ 0 ] = split[ 0 ];
    
    child2.set( tmp_min, tmp_max );

}



template< typename T, typename vector_t, unsigned int dimensions >
bool 
aabb< T, vector_t, dimensions >::is_inside( const vec& point ) const
{
	bool is_inside_ = true;
    
    for( size_t i = 0; i < dimensions; ++i )
    {
        if ( point[ i ] < _min[ i ] || point[ i ] >= _max[ i ] )
            is_inside_ = false; 
        
    }
    
    /*
	typename vector_t< T >::const_iterator it		= point.begin();
	typename vector_t< T >::const_iterator it_end	= point.end();
	for ( size_t index = 0; is_inside_ && it != it_end; ++it, ++index )
	{
		if ( *it < _min[ index ] )
			is_inside_ = false;
		if( *it >= _max[ index ] )
			is_inside_ = false;
	}
    */
	return is_inside_;
//    return //! ( point.smaller( _min ) | point.greater( _max ) );


}


} // namespace stream_process

#endif
