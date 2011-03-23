#ifndef __STREAM_PROCESS__NEIGHBOR_INFO__HPP__
#define __STREAM_PROCESS__NEIGHBOR_INFO__HPP__

#include <stream_process/stream_data.hpp>

namespace stream_process
{

template< typename sp_types_t >
struct neighbor_info : public sp_types_t
{
    typedef typename sp_types_t::sp_float_type  sp_float_type;

    inline stream_data*         get_ptr();
    inline const stream_data*   get_ptr() const;
    
    // important: this is the square distance!!!
    inline sp_float_type        get_distance() const;

    inline void set( sp_float_type distance_, stream_data* ptr_ );
    
    inline bool operator<( const neighbor_info& other_nb ) const;
    inline bool operator>( const neighbor_info& other_nb ) const;
    
protected:
    sp_float_type   _distance;
    stream_data*    _ptr;

}; // class neighbor_info


template< typename sp_types_t >
inline stream_data*
neighbor_info< sp_types_t >::
get_ptr()
{
    return _ptr;
}



template< typename sp_types_t >
inline const stream_data*
neighbor_info< sp_types_t >::
get_ptr() const
{
    return _ptr;
}



template< typename sp_types_t >
inline typename neighbor_info< sp_types_t >::sp_float_type
neighbor_info< sp_types_t >::
get_distance() const
{
    return _distance;
}



template< typename sp_types_t >
inline void
neighbor_info< sp_types_t >::
set( sp_float_type distance_, stream_data* ptr_ )
{
    assert( ptr_ );

    _distance   = distance_;
    _ptr        = ptr_;
}



template< typename sp_types_t >
inline bool
neighbor_info< sp_types_t >::operator<( const neighbor_info& other_nb ) const
{
    return _distance < other_nb._distance;
}

    

template< typename sp_types_t >
inline bool
neighbor_info< sp_types_t >::operator>( const neighbor_info& other_nb ) const
{
    return _distance > other_nb._distance;
}


} // namespace stream_process

#endif

