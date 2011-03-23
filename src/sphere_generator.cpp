#include <stream_process/sphere_generator.hpp>

#include <cmath>

namespace stream_process
{

void
sphere_generator::generate( 
    size_t number_of_points,
    std::vector< vec3f >& positions_normals, 
    std::vector< vec3f >& axes,
    float& radius,
    float& axis_ratio, 
    float& axis_length
    )
{
    generate_regular_points_on_sphere( positions_normals, number_of_points );

    const size_t n = positions_normals.size();
    axes.resize( n );

    const float default_point_radius = 1.0f / sqrtf( n );

    radius      = default_point_radius; 
    axis_ratio  = 0.666f;
    axis_length = default_point_radius * 1.25f;

    vec3f axis;
    vec3f tmp( 0.0f, 0.0f, 1.0f );
    
    for( size_t index = 0; index < n; ++index )
    {
        const vec3f& normal = positions_normals[ index ];
        
        axis.cross( tmp, normal );
        axes[ index ] = axis;
    }

}


} // namespace stream_process

