#include "converter.hpp"

#include "mapped_point_data.hpp"

namespace stream_process
{

void
converter::convert_to_ascii_pnt( const std::string& base_filename )
{
    mapped_point_data points( base_filename + ".points",
        base_filename + ".ph" );

    point_info& info = points.get_point_info();
    
    rt_struct_member< vec3f > position
        = info.get_accessor_for_attribute< vec3f >( "position" );
    rt_struct_member< vec3f > normal
        = info.get_accessor_for_attribute< vec3f >( "normal" );
    rt_struct_member< float > radius
        = info.get_accessor_for_attribute< float >( "radius" );

    rt_struct_member< vec3ub > color;
    bool input_has_color = false;
    try
    {
        color = info.get_accessor_for_attribute< vec3ub >( "color" );
        input_has_color = true;
    }
    catch(...) {}
        
    const size_t point_count    = info.get_point_count();
    const size_t point_size     = info.get_point_size_in_bytes(); 
    std::ofstream pntfile( std::string( base_filename + ".pnt" ).c_str() );
    
    if ( ! pntfile.is_open() )
    {
        throw exception( "failed to open output file.", SPROCESS_HERE );
    }
    
    pntfile << point_count << std::endl;
    
    char* point_ptr = points.get_data_ptr();
    
    vec3ub default_color( 128, 128, 128 );
    
    for( size_t index = 0; index < point_count; ++index, point_ptr += point_size )
    {
        stream_point* point = reinterpret_cast< stream_point* >( point_ptr );
        
        const vec3f& p  = point->get( position );
        const vec3f& n  = point->get( normal );
        const float& r  = point->get( radius );
        
        if (
            std::isnan( p.x() ) ||
            std::isnan( p.y() ) ||
            std::isnan( p.z() ) ||
            std::isnan( n.x() ) ||
            std::isnan( n.y() ) ||
            std::isnan( n.z() ) ||
            std::isnan( r ) 
            )
        {
            std::cout
                << "index " << index 
                << p 
                << n 
                << r 
                << std::endl;
            throw exception( "encountered nan while converting model.", 
                SPROCESS_HERE );
        }
        
        pntfile
            << p.x() << " " 
            << p.y() << " " 
            << p.z() << " " 
            << n.x() << " " 
            << n.y() << " " 
            << n.z() << " ";
        if ( input_has_color )
        {
            const vec3ub& c = point->get( color );
            pntfile
                << (size_t) c.x() << " " 
                << (size_t) c.y() << " " 
                << (size_t) c.z() << " ";
        }
        else
        {
            pntfile 
                << (size_t) default_color.x() << " "
                << (size_t) default_color.y() << " "
                << (size_t) default_color.z() << " ";
        }
        pntfile
            << r << " "
            << std::endl;
    }

    pntfile.close();
}

} // namespace stream_process

