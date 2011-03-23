#ifndef __STREAM_PROCESS__NORMAL_DEBUG_HELPER__HPP__
#define __STREAM_PROCESS__NORMAL_DEBUG_HELPER__HPP__

#include "stream_op.h"
#include "rt_struct_member.h"
#include "neighbor.h"

#include <iostream>

namespace stream_process
{

class normal_debug_helper
{
public:
    normal_debug_helper();
    ~normal_debug_helper();
    void setup_old( 
        const rt_struct_member< vec3f >& position, 
        const rt_struct_member< vec3f >& normal, 
        const rt_struct_member< uint32_t >& point_index, 
        //const rt_struct_member< uint32_t >& nb_count, 
        const rt_struct_member< stream_point* >& neighbors,
        const rt_struct_member< float >& nb_distances
        );
        
    void setup_new( 
        const rt_struct_member< vec3f >& position, 
        const rt_struct_member< vec3f >& normal, 
        const rt_struct_member< uint32_t >& point_index, 
        //const rt_struct_member< uint32_t >& nb_count, 
        const rt_struct_member< neighbor >& neighbors
        );
        
    void write_debug_info_new( stream_point* point );
    void write_debug_info_old( stream_point* point );
    
protected:
    rt_struct_member< vec3f >           _position;
    rt_struct_member< vec3f >           _normal;
    rt_struct_member< uint32_t >        _point_index;
    //rt_struct_member< uint32_t >        _nb_count;
    rt_struct_member< neighbor >        _neighbors;
    rt_struct_member< stream_point* >   _nb_ptrs;
    rt_struct_member< float >           _nb_distances;
    
    std::ofstream _debug_info;
    
}; // class normal_debug_helper

} // namespace stream_process

#endif

