#ifndef __STREAM_PROCESS__RT_STRUCT_MEMBER__H__
#define __STREAM_PROCESS__RT_STRUCT_MEMBER__H__

#include "rt_struct.h"
#include "rt_struct_member_base.h"

#include <functional>
#include <string>
#include <map>
#include <utility>
#include <sys/types.h>

#include <cassert>


/**
*
* @brief accessor-class for point attributes
*
* @author jonas boesch
*/

namespace stream_process
{

// TODO should be rts_accessor or so

    //#define DYNAMIC_ELEMENT_DEBUG_OUT
template< typename T >
struct rt_struct_member : public rt_struct_member_base
{
    rt_struct_member();
    rt_struct_member( const std::string& name );
    
    inline void set_offset( size_t offset_ ); 
    inline const rt_struct_member& operator=( const rt_struct_member& other ); 


    // DEPRECATED START -> use rt_struct::get(...), set(...)    
        // gets the *this element out of the parameter point
        inline T& operator<<( rt_struct* point ) const;
        inline const T& operator<<( const rt_struct* point ) const;
        inline T& get( rt_struct* point ) const;
        inline const T& get( const rt_struct* point ) const;
        
        // neighbors are std::pair< float, rt_struct >.. convenience accessor
        template< typename pair_first_t >
        inline const T& get( std::pair< pair_first_t, rt_struct* >& pair_ ) const;
        template< typename pair_first_t >
        inline T& get( std::pair< pair_first_t, rt_struct* >& pair_ ) const;

        //inline T& operator<<( const std::pair< rt_struct*, size_t >& array_element_ ) const;
        inline T& get( rt_struct* point, size_t index ) const;
        
        inline void set( rt_struct* point, const T& data ) const;
        inline void set( rt_struct* point, const char* data ) const;
    // DEPRECATED END

};


template< typename T >
rt_struct_member< T >::rt_struct_member()
{}



template< typename T >
rt_struct_member< T >::rt_struct_member( const std::string& name )
    : rt_struct_member_base( name )
{
    register_member( _name );
}


template< typename T >
inline T& 
rt_struct_member< T >::operator<<( rt_struct* point ) const
{
    assert( point );
    return (T&) point[ _offset ];    
}


template< typename T >
inline const T& 
rt_struct_member< T >::operator<<( const rt_struct* point ) const
{
    assert( point );
    return (T&) point[ _offset ];    
}



template< typename T >
inline T& 
rt_struct_member< T >::get( rt_struct* point ) const
{
    assert( point );
    return (T&) point[ _offset ];    
}



template< typename T >
inline const T& 
rt_struct_member< T >::get( const rt_struct* point ) const
{
    assert( point );
    return (T&) point[ _offset ];    
}



template< typename T >
template< typename pair_first_t >
inline const T& 
rt_struct_member< T >::get( std::pair< pair_first_t, rt_struct* >& pair_ ) const
{
    assert( pair_.second );
    return (T&) pair_.second[ _offset ];    
}


template< typename T >
template< typename pair_first_t >
inline T& 
rt_struct_member< T >::get( std::pair< pair_first_t, rt_struct* >& pair_ ) const
{
    assert( pair_.second );
    return (T&) pair_.second[ _offset ];    
}


#if 0
template< typename T >
inline T& 
rt_struct_member< T >
    ::operator<<( const std::pair< rt_struct*, size_t >& ae ) const
{
    assert( ae.first );
    return (T&) ae.first[ _offset + sizeof( T ) * ae.second ];    
}
#endif


template< typename T >
inline T& 
rt_struct_member< T >
    ::get( rt_struct* point, size_t index ) const
{
    assert( point );
    return (T&) point[ _offset + sizeof( T ) * index ];    
}




template< typename T >
inline void 
rt_struct_member< T >
    ::set( rt_struct* point, const T& data ) const
{
    memcpy( &point[ _offset ], &data, sizeof( T ) );
}



template< typename T >
inline void 
rt_struct_member< T >
    ::set( rt_struct* point, const char* data ) const
{
    assert( point );
    memcpy( &point[ _offset ], data, sizeof( T ) );    
}



template< typename T >
inline void 
rt_struct_member< T >::set_offset( size_t offset_ )
{
    _offset = offset_;
}



template< typename T >
inline const rt_struct_member< T >&
rt_struct_member< T >::operator=( const rt_struct_member& other )
{
    _offset = other._offset;
    return *this;
}


} // namespace stream_process

#endif

