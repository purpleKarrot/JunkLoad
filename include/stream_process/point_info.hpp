#ifndef __STREAM_PROCESS__POINT_INFO__HPP__
#define __STREAM_PROCESS__POINT_INFO__HPP__

#include "rt_struct_info.hpp"
#include "rt_struct_member.h"
#include "VMMLibIncludes.h"

namespace stream_process
{

class point_info : public rt_struct_info
{
public:
    point_info();

    size_t          get_point_count() const;
    const vec3f&    get_min() const;
    const vec3f&    get_max() const;

	void			set_point_count( size_t point_count );
	void			set_min( const vec3f& min );
	void			set_max( const vec3f& max );

    bool            requires_endian_conversion() const;
    
    size_t          get_point_set_size_in_bytes() const;
    
    void            set_transform( const mat4f& transform_ );
    const mat4f&    get_transform() const;
    bool            has_transform();

    template< typename T >
    rt_struct_member< T > 
    get_accessor_for_attribute( const std::string& name ) const;

    template< typename T >
    rt_struct_member< T > 
    get_rt_struct_member_for_attribute( const std::string& name ) const;

    friend std::ostream& operator<<( std::ostream& os, const point_info& info )
    {
        os << info._serialize() << std::endl;
        return os;
    }
    
protected:
    friend class point_header;

    std::string     _serialize( bool with_comments = true, 
        bool only_out = false ) const;
    
    size_t  _point_count;
    vec3f   _min;
    vec3f   _max;
    
    bool        _input_is_big_endian;
    const bool  _output_is_big_endian; // ouput is always machine endianess
    
    bool        _has_transform;
    mat4f       _transform;

}; // class point_info


template< typename T >
rt_struct_member< T > 
point_info::get_rt_struct_member_for_attribute( const std::string& name ) const
{
    return get_accessor_for_attribute< T >( name );
}



template< typename T >
rt_struct_member< T > 
point_info::get_accessor_for_attribute( const std::string& name ) const
{
    const rt_struct_member_info& info 
        = rt_struct_info::get_attribute_info( name );
    
    rt_struct_member< T > rs_member;
    rs_member.set_name( name );
    rs_member.set_offset( info.stream_offset );
    
    return rs_member;
}



} // namespace stream_process

#endif

