#include <stream_process/point_set_generator.hpp>
#include <stream_process/sphere_generator.hpp>

#include <stream_process/data_set_header.hpp>
#include <stream_process/file_suffix_helper.hpp>

#include <fstream>

namespace stream_process
{

void
point_set_generator::
generate_unit_sphere( const std::string& filename_base, size_t points )
{
    std::vector< vec3f >    positions_normals;
    std::vector< vec3f >    ellipse_axes;
    float                   radius;
    float                   axis_ratio;
    float                   axis_length;

    // generate sphere
    sphere_generator sgen;
    sgen.generate( points, positions_normals, ellipse_axes, radius,
        axis_ratio, axis_length );
    
    // setup point set
    data_set_header header;
    data_element& vertices  = header.get_vertex_element();
    stream_structure& vs    = vertices.get_structure();
    
    vs.create_attribute( "position",    SP_FLOAT_32, 3 );
    vs.create_attribute( "normal",      SP_FLOAT_32, 3 );
    vs.create_attribute( "color",       SP_UINT_8, 4 );
    vs.create_attribute( "radius",      SP_FLOAT_32, 1 );
    vs.create_attribute( "axis",        SP_FLOAT_32, 3 );
    vs.create_attribute( "axis_ratio",  SP_FLOAT_32, 1 );
    vs.create_attribute( "axis_length", SP_FLOAT_32, 1 );
    
    size_t offset = 0;
    stream_structure::iterator it = vs.begin(), it_end = vs.end();
    for( ; it != it_end; ++it )
    {
        attribute& attr = **it;
        attr.set_is_output( true );
        attr.set_offset( offset );
        offset += attr.get_size_in_bytes();
    }
    
    vertices.update();
    
    size_t point_size_in_bytes = vertices.get_size_in_bytes();
    
    std::string suffix = file_suffix_helper::get_vertex_suffix();
    
    std::ofstream data_file( std::string( filename_base + suffix ).c_str() );
    if ( ! data_file.is_open() )
    {
        throw exception( "data file could not be opened.", SPROCESS_HERE );
    }

    const size_t num_points = positions_normals.size();

    vertices.set_size( num_points );

    vec4ub color( 0, 0, 0, 255 );

    for( size_t index = 0; index < num_points; ++index )
    {
        const vec3f& pn = positions_normals[ index ];
        // pos
        data_file.write( (char*) &pn, sizeof( vec3f ) );
        // normal
        data_file.write( (char*) &pn, sizeof( vec3f ) );
        // color
        color.x() = static_cast< uint8_t >( ( pn.x() + 1.0 ) * 127.5 );
        color.y() = static_cast< uint8_t >( ( pn.y() + 1.0 ) * 127.5 );
        color.z() = static_cast< uint8_t >( ( pn.z() + 1.0 ) * 127.5 );

        data_file.write( (char*) &color, sizeof( vec4ub ) );
        // radius
        data_file.write( (char*) &radius, sizeof( float ) );
        // axis
        data_file.write( (char*) &ellipse_axes[ index ], sizeof( vec3f ) );
        // ratio
        data_file.write( (char*) &axis_ratio, sizeof( float ) );
        // length
        data_file.write( (char*) &axis_length, sizeof( float ) );
    }
    data_file.close();
    
    header.set_aabb_min( vec3f( -1, -1, -1 ) );
    header.set_aabb_max( vec3f( 1, 1, 1 ) );
    
    header.write_to_file( filename_base );
}


} // namespace stream_process

