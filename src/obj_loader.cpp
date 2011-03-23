#include "obj_loader.hpp"

#include "mapped_point_data.hpp"
#include "string_utils.h"

namespace stream_process
{

void
obj_loader::load( 
        const std::string& obj_file_name_, 
        const std::string& output_filename,
		mapped_point_data& mapped_point_data_ )
{
	_mapped_point_data  = &mapped_point_data_;
    _output_filename    = output_filename; 

	_file.open( obj_file_name_.c_str() );
	if ( ! _file.is_open() )
	{
		std::string msg( "file ");
		msg += obj_file_name_;
		msg += " not found.";
		throw exception( msg.c_str(), SPROCESS_HERE );
	}
	
	// since obj doesn't have a header, we have to scan the file first 
	_scan_file();
	
    // setup mapped point data instance
	_mapped_point_data->setup_empty_point_info();
    point_info& point_info_ = _mapped_point_data->get_point_info();

    // setup point_info
	point_info_.set_point_count( _vertex_count );
	point_info_.add_attribute( rt_struct_member_info( "position", SP_FLOAT_32,
		 sizeof(float), 3 , true, true) );

    if ( _has_normals )
    {
        point_info_.add_attribute( rt_struct_member_info( "normal", 
            SP_FLOAT_32, sizeof(float), 3 , true, true) );
	}

    if ( _has_texcoords )
    {
        point_info_.add_attribute( rt_struct_member_info( "texcoords", 
            SP_FLOAT_32, sizeof(float), 2 , true, true) );
	}
    point_info_.update();

    // create empty file
    _mapped_point_data->setup_empty_mmap( _output_filename );


    // read in the data from the obj
    size_t point_size_in_bytes  = point_info_.get_point_size_in_bytes();
    char* mmap_begin            = _mapped_point_data->get_data_ptr();
    char* mmap_end              = mmap_begin + _vertex_count * point_size_in_bytes;
    
    
    char* position_ptr = mmap_begin + 
        point_info_.get_offset_to_attribute( "position" );
    
    char* normal_ptr = mmap_begin;
    if ( _has_normals )
        normal_ptr += point_info_.get_offset_to_attribute( "normal" );

    char* texcoord_ptr = mmap_begin;
    if ( _has_texcoords )
        texcoord_ptr += point_info_.get_offset_to_attribute( "texcoords" );
    
	std::string line;
    std::vector< std::string > tokens;
	while( ! _file.eof() )
	{
        tokens.clear();
		
        std::getline( _file, line );
        // remove comments
		if ( line.find_first_of( '#' ) != std::string::npos )
        {
            string_utils::split_string( line, tokens, '#' );
            if ( ! tokens.empty() && ! tokens[ 0 ].empty() )
            {
                line = tokens[ 0 ];
                tokens.clear();
            }
        }

        // split line
        string_utils::split_string( line, tokens );
        if ( line.size() > 1 )
        {
            if ( line[ 0 ] == 'v' )
            {
                if ( line[ 1 ] == ' ' )
                {
                    if ( tokens.size() != 4 )
                    {
                        std::string msg( "invalid vertex found in ");
                        msg += obj_file_name_;
                        msg += ". ";
                        throw exception( msg.c_str(), SPROCESS_HERE );
                    }
                    vec3f& p = *reinterpret_cast< vec3f* >( position_ptr );
                    #ifdef SPROCESS_OLD_VMMLIB
                    p.x = string_utils::from_string< float >( tokens[ 1 ] );
                    p.y = string_utils::from_string< float >( tokens[ 2 ] );
                    p.z = string_utils::from_string< float >( tokens[ 3 ] );
                    #else
                    p.x() = string_utils::from_string< float >( tokens[ 1 ] );
                    p.y() = string_utils::from_string< float >( tokens[ 2 ] );
                    p.z() = string_utils::from_string< float >( tokens[ 3 ] );
                    #endif
                    
                    position_ptr += point_size_in_bytes;
                }
                else if ( line[ 1 ] == 't' )
                {
                    if ( tokens.size() != 3 )
                    {
                        std::string msg( "invalid vertex texcoord found in ");
                        msg += obj_file_name_;
                        msg += ". ";
                        throw exception( msg.c_str(), SPROCESS_HERE );
                    }
                    vec2f& tc = *reinterpret_cast< vec2f* >( texcoord_ptr );
                    #ifdef SPROCESS_OLD_VMMLIB
                    tc.x = string_utils::from_string< float >( tokens[ 1 ] );
                    tc.y = string_utils::from_string< float >( tokens[ 2 ] );
                    #else
                    tc.x() = string_utils::from_string< float >( tokens[ 1 ] );
                    tc.y() = string_utils::from_string< float >( tokens[ 2 ] );
                    #endif
                    texcoord_ptr += point_size_in_bytes;

                }
                else if ( line[ 1 ] == 'n' )
                {
                    if ( tokens.size() != 4 )
                    {
                        std::string msg( "invalid vertex normal found in ");
                        msg += obj_file_name_;
                        msg += ". ";
                        throw exception( msg.c_str(), SPROCESS_HERE );
                    }
                    vec3f& n = *reinterpret_cast< vec3f* >( normal_ptr );
                    #ifdef SPROCESS_OLD_VMMLIB
                    n.x = string_utils::from_string< float >( tokens[ 1 ] );
                    n.y = string_utils::from_string< float >( tokens[ 2 ] );
                    n.z = string_utils::from_string< float >( tokens[ 3 ] );
                    #else
                    n.x() = string_utils::from_string< float >( tokens[ 1 ] );
                    n.y() = string_utils::from_string< float >( tokens[ 2 ] );
                    n.z() = string_utils::from_string< float >( tokens[ 3 ] );
                    #endif
                    normal_ptr += point_size_in_bytes;
                }
            }
        }
	}

    //std::cout << "vertex count " << _vertex_count << std::endl;

    if ( position_ptr < mmap_end )
    {
        throw exception( "reading vertex positions failed.", SPROCESS_HERE );
    }
    
    if ( _has_normals && normal_ptr < mmap_end )
    {
        throw exception( "reading vertex normals failed.", SPROCESS_HERE );
    }
    
    if ( _has_texcoords && texcoord_ptr < mmap_end )
    {
        throw exception( "reading texture coordinats failed.", SPROCESS_HERE );
    }


    // determine bounding box
    file_loader::_find_min_max();

}



void
obj_loader::_scan_file()
{
	bool has_vertices	= false;
	_has_normals		= false;
	_has_texcoords		= false;

    _vertex_count           = 0;
    size_t normal_count     = 0;
    size_t texcoord_count   = 0;
	
	std::string line;
	while( ! _file.eof() )
	{
		std::getline( _file, line );
		string_utils::trim( line );
		if ( line.size() > 1 )
		{
			if ( line[ 0 ] == 'v' )
			{
				if ( line[ 1 ] == ' ' )
				{
					has_vertices = true;
					++_vertex_count;
				}
				else if ( line[ 1 ] == 't' )
				{
					_has_texcoords = true;
                    ++texcoord_count;
                }
				else if ( line[ 1 ] == 'n' )
				{
					_has_normals = true;
                    ++normal_count;
                }
			}
			
		}
	}
    
	if ( ! has_vertices )
	{
		throw exception( "obj file does not contain vertex information.", 
			SPROCESS_HERE );
	}

    if ( _has_normals && normal_count != _vertex_count )
    {
        throw exception( "normal and vertex counts in obj file don't match.",
            SPROCESS_HERE );
    }

    if ( _has_texcoords && texcoord_count != _vertex_count )
    {
        throw exception( "texcoord and vertex counts in obj file don't match.",
            SPROCESS_HERE );
    }

    // clear eof flag (since we just scanned the whole file)
    _file.clear();
    // reset the file's get_ptr to beginning of the file
    _file.seekp( 0 );


}


} // namespace stream_process

