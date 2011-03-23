#ifndef __STREAM_PROCESS__ELLIPSOID__HPP__
#define __STREAM_PROCESS__ELLIPSOID__HPP__

#include <stream_process/stream_process_types.hpp>

namespace stream_process
{

template< typename sp_types_t >
class ellipsoid
{
public:
    STREAM_PROCESS_TYPES
    
    struct init_data
    {
        // pos + normal of the 'current' point
        vec3            position;
        vec3            normal;
        // principal curvature estimates 
        vec3            e1;
        vec3            e2;
        sp_float_type   k1;
        sp_float_type   k2;
    };

    void initialize( const init_data& init_data_ );

protected:

}; // class ellipsoid


template< typename sp_types_t >
void
ellipsoid< sp_types_t >::initialize( const init_data& init_data_ )
{
    const vec3& n   = init_data_.normal;
    const vec3& e1  = init_data_.e1;
    const vec3& e2  = init_data_.e2;
    
    const sp_float_type& k1 = init_data_.k1;
    const sp_float_type& k2 = init_data_.k2;


    sp_float_type radius1   = 1.0 / init_data_.k1;
    sp_float_type radius2   = 1.0 / init_data_.k2;
    
    vec3 e1_axis    = e1 * radius1; // 'x'
    vec3 e2_axis    = e2 * radius2; // 'y'

}


} // namespace stream_process

#endif

