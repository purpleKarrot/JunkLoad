#ifndef __STREAM_PROCESS__CURVE__HPP__
#define __STREAM_PROCESS__CURVE__HPP__

#include <stream_process/stream_process_types.hpp>

namespace stream_process
{

template< typename sp_types_t >
class curve : public sp_types_t
{
public:
    STREAM_PROCESS_TYPES
    
    struct angle_less_ptr
    {
        bool operator()( const curve* const a, const curve* const b )
        {
            return a->angle < b->angle;
        }
    };
       
    // pointers to the neighboring points
    const neighbor_type*    vi;
    const neighbor_type*    vj;

    sp_float_type           t;
    vec3                    tangent;
    vec3                    p_double_prime;

    vec3                    k;
    sp_float_type           kn;
    
    sp_float_type           angle;
    
    vec2                    proj_tangent;
    
    sp_float_type           vi_distance;
    sp_float_type           vj_distance;

}; // class curve

} // namespace stream_process

#endif

