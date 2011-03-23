#ifndef __STREAM_PROCESS__SPHERE_GENERATOR__HPP__
#define __STREAM_PROCESS__SPHERE_GENERATOR__HPP__

#include <stream_process/VMMLibIncludes.h>
#include <stream_process/random.hpp>

#include <vector>

namespace stream_process
{

class sphere_generator
{

public:
    // since we generate a unit sphere, positions are normals are positions.. :)
    void generate( 
        size_t number_of_points,
        std::vector< vec3f >& positions_normals, 
        std::vector< vec3f >& axes,
        float& radius,
        float& axis_ratio, 
        float& axis_length
        );
        
    template< typename float_t >
    void generate_regular_points_on_sphere( 
        std::vector< vmml::vector< 3, float_t > >& points, 
        size_t target_number_of_points, 
        float_t radius = 1.0
        );
        
}; // class sphere_generator



template< typename float_t >
void
sphere_generator::generate_regular_points_on_sphere( 
    std::vector< vmml::vector< 3, float_t > >& points, 
    size_t target_number_of_points, 
    float_t radius
    )
{
    vmml::vector< 3, float_t > point;
    size_t actual_number_of_points = 0;
    
    float_t pi = M_PI;
    float_t target_n = static_cast< float_t >( target_number_of_points );
    
    float_t a = pi * 4.0 * radius * radius / target_n;
    float_t d = sqrt( a );
    
    // +0.5 for rounding to int
    size_t m_theta  = static_cast< size_t >( ( pi / d ) + 0.5 );
    float_t d_theta = pi / m_theta;
    float_t d_phi   = a / d_theta;
    
    for( size_t m = 0; m < m_theta; ++m )
    {
        float_t theta   = pi * ( 0.5 + m ) / m_theta;
        float_t sin_theta = sin( theta );
        
        // +0.5 for rounding to int
        size_t m_phi    = static_cast< size_t >( 
            ( pi * 2.0 * sin_theta / d_phi ) + 0.5
             );
        for( size_t n = 0; n < m_phi; ++n )
        {
            float_t phi = pi * 2.0 * n / m_phi;
            
            point.set(
                sin_theta * cos( phi ),
                sin_theta * sin( phi ),
                cos( theta )
                );
            point *= radius;
            
            points.push_back( point );
            ++actual_number_of_points;
        }
    }

    std::cout
        << "generated sphere with "
        << actual_number_of_points
        << " points, target was "
        << target_number_of_points << "." 
        << std::endl;
}


} // namespace stream_process

#endif

