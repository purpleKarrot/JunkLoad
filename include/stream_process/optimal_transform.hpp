#ifndef __STREAM_PROCESS__OPTIMAL_TRANSFORM__HPP__
#define __STREAM_PROCESS__OPTIMAL_TRANSFORM__HPP__

#include "mapped_point_data.hpp"

#include "VMMLibIncludes.h"

/**
*
*   this class computes the transformation that is required to align the model
*   optimally for stream processing.
*
*/

namespace stream_process
{

class optimal_transform
{
public:
    // computes the optimal transform
    optimal_transform( mapped_point_data& mapped_point_data_ );

    // does not compute the transform but uses the specified m4 instead.
    optimal_transform( mapped_point_data& mapped_point_data_, 
        const mat4f& transform );

    // transform the model for stream processing
    void apply_optimal_transform();
    // transform the model back to the original coordinates
    void apply_inverse_optimal_transform();
    
    const mat4f& get_transform() const;
    
    const vec3f& get_translation() const;
    const mat3f& get_orientation() const;
    
protected:
    void _determine_optimal_transform();
    
    mat3f _get_covariance_3x3();

    mapped_point_data&  _mapped_point_data;
    
    mat4f         _transform_4x4;

    mat3f         _transform; // FIXME orientation
    vec3f         _translation;
    vec3f         _center;
	

}; // class optimal_transform

} // namespace stream_process

#endif

