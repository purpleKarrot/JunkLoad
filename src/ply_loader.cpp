#include "ply_loader.hpp"

#include "point_info.hpp"
#include "mapped_point_data.hpp"

namespace stream_process
{

ply_loader::ply_loader()
{}



void
ply_loader::load(
    const std::string& ply_filename, 
    const std::string& output_filename, 
    mapped_point_data& mapped_point_data_
)
{
    _mapped_point_data  = &mapped_point_data_;
    _output_filename    = output_filename;
    
    // open file
    _ply_file = ply_open_for_reading( 
        const_cast<char *>( ply_filename.c_str() ), &_element_count, 
            &_element_names, &_file_type, &_version );

    if( ! _ply_file )
    {
        std::string msg = "opening file ";
        msg += ply_filename;
        msg += " failed.";
        throw exception( msg.c_str(), SPROCESS_HERE );
    }

    // check for vertex element ( MUST HAVE )
    bool has_vertex_element = false;
    for ( ssize_t index = 0; index < _element_count; ++index )
    {
        if ( std::string( _element_names[ index ] ) == "vertex" )
            has_vertex_element = true;
    }
    if ( ! has_vertex_element )
    {
        throw exception( "no vertex element in ply file.", 
            SPROCESS_HERE );
    }
    
    _read_vertex_meta_data();
    _read_vertex_data();

    //_debug_print_points();
}



void 
ply_loader::_read_vertex_meta_data()
{
    _mapped_point_data->setup_empty_point_info();
    point_info& point_info_ = _mapped_point_data->get_point_info();

    // get vertex information
	_vertex_properties = ply_get_element_description( _ply_file, "vertex",
		&_vertex_count, &_vertex_property_count );
    
    point_info_.set_point_count( _vertex_count );

    size_t position_component_count = 0;
    size_t color_component_count    = 0;
    size_t normal_component_count   = 0;

    for( ssize_t index = 0; index < _vertex_property_count; ++index )
    {
        std::string name = _vertex_properties[ index ]->name;

        // position
        if ( name == "x" || name == "y" || name == "z" )
        {
            ++position_component_count;
        }
        // color
        else if ( name == "red" || name == "green" || name == "blue" 
            || name == "alpha" )
        {
            ++color_component_count;
        }
        // normal
        else if ( name == "nx" || name == "ny" || name == "nz" )
        {
            ++normal_component_count;
        }
        else _other_properties.push_back( name );
    }
 
    // FIXME allow 2d and homogenous
    if ( position_component_count != 3 )
    {
        throw exception( "ply file does not contain 3d vertices.", 
            SPROCESS_HERE );
    }
    else
    {
        rt_struct_member_info info( "position", SP_FLOAT_32, 
            position_component_count, true, true );
        
        point_info_.add_attribute( info );
    }
    
    if ( color_component_count > 0 )
    {
        rt_struct_member_info info( "color", SP_UINT_8, 
            color_component_count, true, true );
        
        point_info_.add_attribute( info );
    }

    if ( normal_component_count > 0 )
    {
        rt_struct_member_info info( "normal", SP_FLOAT_32, 
            normal_component_count, true, true );
        
        point_info_.add_attribute( info );
    }
    
    std::vector< std::string >::const_iterator it       = 
        _other_properties.begin();
    std::vector< std::string >::const_iterator it_end   = 
        _other_properties.end();
    
    for( ; it != it_end; ++it )
    {
        rt_struct_member_info info( *it, SP_FLOAT_32, 1, true, true );
        point_info_.add_attribute( info );
    }
    
    point_info_.update();
}


void
ply_loader::_read_vertex_data()
{
    point_info& point_info_ = _mapped_point_data->get_point_info();

    // setup the target mmapped file
    // FIXME
    _mapped_point_data->setup_empty_mmap( _output_filename + ".points" );

    // we have to build the required PlyProperties that allow 
    // data extraction from the ply 
    std::vector< PlyProperty > ply_properties;

    size_t offset = 0;

    std::vector< const char* > names;

    ply_properties.push_back( _create_ply_property( "x", PLY_FLOAT, offset ));
    offset += sizeof( float );

    ply_properties.push_back( _create_ply_property( "y", PLY_FLOAT, offset ));
    offset += sizeof( float );

    ply_properties.push_back( _create_ply_property( "z", PLY_FLOAT, offset ));
    offset += sizeof( float );
    
    if ( point_info_.has_attribute( "color" ) )
    {
        ply_properties.push_back( _create_ply_property( "red", PLY_UCHAR, 
            offset ));
        offset += sizeof( uint8_t );
        
        ply_properties.push_back( _create_ply_property( "green", PLY_UCHAR, 
            offset ));
        offset += sizeof( uint8_t );
        
        ply_properties.push_back( _create_ply_property( "blue", PLY_UCHAR, 
            offset ));
        offset += sizeof( uint8_t );
        
        const rt_struct_member_info& info 
            = point_info_.get_attribute_info( "color" );
        
        if ( info.array_size == 4 )
        {
            ply_properties.push_back( _create_ply_property( "alpha", 
                PLY_UCHAR, offset ));
            offset += sizeof( uint8_t );
        }
        
    }

    if ( point_info_.has_attribute( "normal" ) )
    {
        ply_properties.push_back( _create_ply_property( "nx", PLY_FLOAT, 
            offset ));
        offset += sizeof( float );
        
        ply_properties.push_back( _create_ply_property( "ny", PLY_FLOAT, 
            offset ));
        offset += sizeof( float );
        
        ply_properties.push_back( _create_ply_property( "nz", PLY_FLOAT, 
            offset ));
        offset += sizeof( float );
    }
    
    std::vector< std::string >::const_iterator it       = 
        _other_properties.begin();
    std::vector< std::string >::const_iterator it_end   = 
        _other_properties.end();
    
    for( ; it != it_end; ++it )
    {
        ply_properties.push_back( _create_ply_property( *it, PLY_FLOAT, 
            offset ));
        offset += sizeof( float );
    }
      
    std::vector< PlyProperty >::iterator property_it = 
        ply_properties.begin();
    std::vector< PlyProperty >::iterator property_it_end = 
        ply_properties.end();

	for( ; property_it != property_it_end; ++property_it )
	{
        // register the properties that interest us with the ply reader
        //PlyProperty& prop = *property_it;
        ply_get_property( _ply_file, "vertex", &(*property_it) );
	}

    char* target_data_ptr       = _mapped_point_data->get_data_ptr();
    size_t point_size_in_bytes  = point_info_.get_point_size_in_bytes();
    size_t point_count          = point_info_.get_point_count();
    
    for( size_t i = 0; i < point_count; ++i )
    {
        // read object data into memory map
        ply_get_element( _ply_file, target_data_ptr );
        target_data_ptr += point_size_in_bytes;
    }
    
    _find_min_max();

    // clean up (delete c-string char arrays)
    property_it = ply_properties.begin();
	for( ; property_it != property_it_end; ++property_it )
	{
        delete[] (*property_it).name;
    }
}



PlyProperty
ply_loader::_create_ply_property( const std::string& name, int internal_type, 
    int offset, int count_internal, int count_offset )
{
    PlyProperty ply_property;
    ply_property.name = new char[ name.size() ];
    strcpy( ply_property.name, name.c_str() );
    ply_property.internal_type     = internal_type;
    ply_property.offset            = offset;
    ply_property.count_internal    = count_internal;
    ply_property.count_offset      = count_offset;
    return ply_property;
}



void
ply_loader::_debug_print_points()
{
    point_info& point_info_     = _mapped_point_data->get_point_info();
    size_t point_count          = point_info_.get_point_count();

    rt_struct_member< vec3f > position 
        = point_info_.get_rt_struct_member_for_attribute< vec3f >( "position" );
        
    stream_point* point = _mapped_point_data->get_point( 0 );
    std::cout 
        << point->get( position ) 
        << std::endl;

    point = _mapped_point_data->get_point( point_count - 1 );
    std::cout 
        << point->get( position ) 
        << std::endl;
}



void
ply_loader::_debug_check_point( char* point )
{
    stream_point* p = reinterpret_cast< stream_point* > ( point );

    point_info& point_info_     = _mapped_point_data->get_point_info();
    rt_struct_member< vec3f > position 
        = point_info_.get_rt_struct_member_for_attribute< vec3f >( "position" );

    vec3f& pos = p->get( position );
    std::cout << pos << std::endl;

}


} // namespace stream_process

