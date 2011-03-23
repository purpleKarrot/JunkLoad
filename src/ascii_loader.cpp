#include "ascii_loader.hpp"

#include "mapped_point_data.hpp"
#include "string_utils.h"

#include "exception.hpp"

#include "ascii_to_round_point.hpp"
#include "ascii_to_elliptic_point.hpp"

namespace stream_process
{

ascii_loader::ascii_loader()
    : _ascii_to_point( 0 )
    , _vertex_count( 0 )
    , _format( 0 )
    , _current_point( 0 )
{}


ascii_loader::~ascii_loader()
{
    if ( _ascii_to_point )
        delete _ascii_to_point;
}




void
ascii_loader::load(    
    const std::string& filename, 
    const std::string& output_filename, 
    mapped_point_data& mapped_point_data_
    )
{
	_mapped_point_data  = &mapped_point_data_;
    _output_filename    = output_filename; 

	_file.open( filename.c_str() );
	if ( ! _file.is_open() )
	{
		std::string msg( "file ");
		msg += filename;
		msg += " not found.";
		throw exception( msg.c_str(), SPROCESS_HERE );
	}

    std::getline( _file, _line );
    string_utils::split_string( _line, _tokens );
    if ( _tokens.size() == 0 )
    {
        throw exception(
            std::string( "invalid header line in ascii file " ) + filename, 
                SPROCESS_HERE );
    }

    _vertex_count = string_utils::from_string< size_t >( _tokens[ 0 ] );


    const size_t data_start = _file.tellg();
    
    _extract_tokens_from_next_line();
    
    if ( _tokens.size() == 10 )
    {
        _ascii_to_point = new ascii_to_round_point();
    }
    else if ( _tokens.size() == 15 )
    {
        _ascii_to_point = new ascii_to_elliptic_point();
    }
    else
    {
        throw exception( "invalid or unrecognized format.", SPROCESS_HERE );
    }
        
	_mapped_point_data->setup_empty_point_info();
    point_info& point_info_ = _mapped_point_data->get_point_info();
    point_info_.set_point_count( _vertex_count );
    _ascii_to_point->setup_point_info( point_info_ );
       
    // create empty file
    _mapped_point_data->setup_empty_mmap( _output_filename );

    size_t point_size_in_bytes  = point_info_.get_point_size_in_bytes();
    char* mmap_begin            = _mapped_point_data->get_data_ptr();
    char* mmap_end              = mmap_begin + _vertex_count * point_size_in_bytes;
    
    _current_point = mmap_begin;

    _file.seekg( data_start );

	while( ! _file.eof() && _extract_tokens_from_next_line() )
	{
        //std::cout << "cur " << (size_t) _current_point << ", " << _line << std::endl;
        _current_point = _ascii_to_point->write_point( _current_point, _tokens );
    }

    if ( _current_point != mmap_end )
    {
        throw exception( "reading file seems to have failed.", SPROCESS_HERE );
    }

    //std::cout << "vertex count " << _vertex_count << std::endl;

    // determine bounding box
    file_loader::_find_min_max();

    std::cout << "header of newly created point data file: " << std::endl;
    std::cout << point_info_ << std::endl;

}



bool
ascii_loader::_extract_tokens_from_next_line()
{
    _tokens.clear();
    while( ! _file.eof() && _tokens.empty() )
    {
        _line.clear();
        
        std::getline( _file, _line );

        // remove comments
        if ( _line.find_first_of( '#' ) != std::string::npos )
        {
            string_utils::split_string( _line, _tokens, '#' );
            if ( ! _tokens.empty() && ! _tokens[ 0 ].empty() )
            {
                _line = _tokens[ 0 ];
                _tokens.clear();
            }
        }

        // split line (delimiter: space)
        string_utils::split_string( _line, _tokens );
    }
    return ! _file.eof();
}



std::string
ascii_loader::_get_target_filename( const std::string& filename )
{
    return _output_filename  + ".points";
    #if 0
    std::string tmp_filename;
    
    try
    {
        tmp_filename 
            = string_utils::replace_file_suffix( filename, ".tmp.points");
	}
    catch( std::exception& e )
    {
        tmp_filename = filename + ".tmp.points";
    }
    
    return tmp_filename;
    #endif
}


} // namespace stream_process

