#ifndef __STREAM_PROCESS__PRINCIPAL_CURVATURE_ESTIMATE__HPP__
#define __STREAM_PROCESS__PRINCIPAL_CURVATURE_ESTIMATE__HPP__

#include <stream_process/stream_process_types.hpp>
#include <stream_process/curve.hpp>

namespace stream_process
{

template< typename sp_types_t >
class principal_curvature_estimate : public sp_types_t
{
public:
    STREAM_PROCESS_TYPES
    
    typedef curve< sp_types_t >     curve_type;

    const curve_type*   a;
    const curve_type*   b;
    const curve_type*   c;

    sp_float_type       k1;
    sp_float_type       k2;

    vec3                e1;
    vec3                e2;
    
    sp_float_type       theta;

    sp_float_type       quality;
    
    bool                valid;

protected:

}; // class principal_curvature_estimate

} // namespace stream_process

#endif

