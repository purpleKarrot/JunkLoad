#ifndef __STREAM_PROCESS__NB_SHARED_DATA__HPP__
#define __STREAM_PROCESS__NB_SHARED_DATA__HPP__

#include <stream_process/neighbors.hpp>
#include <stream_process/VMMLibIncludes.h>

#include <limits>

namespace stream_process
{

template< typename sp_types_t >
class nb_shared_data : public sp_types_t
{
public:
    typedef typename sp_types_t::sp_float_type      sp_float_type;
    typedef neighbors< sp_types_t >                 neighbors_type;
    typedef vmml::vector< 3, sp_float_type >        vec3;
    typedef attribute_accessor< vec3 >              vec3_accessor_type;
    typedef attribute_accessor< neighbors_type >    nb_accessor_type;

    nb_shared_data( const vec3_accessor_type& get_pos,
        nb_accessor_type& get_nb );
    
    inline sp_float_type&          get_max_z();
    inline const sp_float_type&    get_max_z() const;
    
    const vec3_accessor_type&   get_position_accessor();
    nb_accessor_type&           get_neighbors_accessor();
	
	size_t						_max_bucket_size;
	size_t						_k;
    
    void set_world_aabb( const vec3& min_, const vec3& max_ );
    const vec3& get_aabb_min() const;
    const vec3& get_aabb_max() const;
    
protected:
    sp_float_type               _max_z;
    
    const vec3_accessor_type&   _get_position;
    nb_accessor_type&           _get_neighbors;
    
    vec3                        _aabb_min;
    vec3                        _aabb_max;
    
}; // class nb_shared_data


template< typename sp_types_t >
nb_shared_data< sp_types_t >::
nb_shared_data( const attribute_accessor< vec3 >& get_pos, 
    attribute_accessor< neighbors_type >& get_nb )
    : _get_position( get_pos )
    , _get_neighbors( get_nb )
    , _max_z( - std::numeric_limits< sp_float_type >::max() )
{}


template< typename sp_types_t >
typename nb_shared_data< sp_types_t >::sp_float_type&
nb_shared_data< sp_types_t >::
get_max_z()
{
    return _max_z;
}


template< typename sp_types_t >
const typename nb_shared_data< sp_types_t >::sp_float_type&
nb_shared_data< sp_types_t >::
get_max_z() const
{
    return _max_z;
}



template< typename sp_types_t >
const typename nb_shared_data< sp_types_t >::vec3_accessor_type&
nb_shared_data< sp_types_t >::
get_position_accessor()
{
    return _get_position;
}


template< typename sp_types_t >
typename nb_shared_data< sp_types_t >::nb_accessor_type&
nb_shared_data< sp_types_t >::
get_neighbors_accessor()
{
    return _get_neighbors;
}


template< typename sp_types_t >
const typename nb_shared_data< sp_types_t >::vec3&
nb_shared_data< sp_types_t >::
get_aabb_min() const
{
    return _aabb_min;
}


template< typename sp_types_t >
const typename nb_shared_data< sp_types_t >::vec3&
nb_shared_data< sp_types_t >::
get_aabb_max() const
{
    return _aabb_max;
}


template< typename sp_types_t >
void
nb_shared_data< sp_types_t >::
set_world_aabb( const vec3& min_, const vec3& max_ )
{
    _aabb_min = min_;
    _aabb_max = max_;
}



} // namespace stream_process

#endif

