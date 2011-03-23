#ifndef __VMML__STREAM_POINT_FUNCTORS__H__
#define __VMML__STREAM_POINT_FUNCTORS__H__

#include "stream_point.h"
#include "rt_struct_member.h"

namespace stream_process
{


struct stream_point_less 
    : std::binary_function< stream_point const *, stream_point const *, bool > 
{

    stream_point_less( const size_t offset, const size_t axis_ )
        : axis( axis_ )
    {
        position.set_offset( offset );
    }



    bool operator()( stream_point const* x, stream_point const* y ) const
    {
        return position.get( x )[ axis ] < position.get( y )[ axis ];
    }
    
    
    const size_t axis;
    rt_struct_member< vec3f > position;
};




struct stream_point_greater 
    : std::binary_function< stream_point const *, stream_point const *, bool > 
{

    stream_point_greater( const size_t offset, const size_t axis_ )
        : axis( axis_ )
    {
        position.set_offset( offset );
    }



    bool operator()( stream_point const* x, stream_point const* y ) const
    {
        return position.get( x )[ axis ] > position.get( y )[ axis ];
    }

    const size_t axis;
    rt_struct_member< vec3f > position;

}; // struct stream_point_greater 


} // namespace stream_process

#endif

