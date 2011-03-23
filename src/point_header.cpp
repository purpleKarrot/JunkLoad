#include <stream_process/point_header.hpp>

#include <stream_process/string_utils.h>
#include <stream_process/exception.hpp>
#include <stream_process/data_types.hpp>

#include <fstream>

#include <fstream>

namespace stream_process
{

point_header::point_header( point_info& info )
    : _point_info( info )
{}


void
point_header::read( const std::string& filename )
{
    std::ifstream in_stream;
    in_stream.open( filename.c_str() );
    if ( ! in_stream.is_open() )
    {
        throw exception( "opening point header file failed.", 
            SPROCESS_HERE );
    }
    
    std::string line;
    size_t offset;
    while( std::getline( in_stream, line ) )
    {
        // check for comments
        offset = line.find_first_of( '#' );
        line = line.substr( 0, offset );
        string_utils::trim( line );
        if ( ! line.empty() )
        {
            _parse_line( line );
        }
    }

    in_stream.close();
	
	_point_info._compute_and_set_offsets();
}



void
point_header::_parse_line( const std::string& line )
{
    // TODO REFACTOR!

    std::vector< std::string > tokens;
    string_utils::split_string( line, tokens, ' ' );

    std::vector< std::string >::iterator it     = tokens.begin();
    std::vector< std::string >::iterator it_end = tokens.end();
    for( ; it != it_end; ++it )
    {
        string_utils::to_lower_case( *it );
    }
    
    if ( tokens.empty() )
        return;
    if ( tokens.size() == 1 )
	{
		std::vector< std::string >::iterator it		= tokens.begin();
		std::vector< std::string >::iterator it_end = tokens.end();
		std::cerr << "line : " << "\n";
		for( ; it != it_end; ++it )
		{
			std::cerr << *it << " ";
		}
		std::cerr << std::endl;
		throw exception( "invalid line in point header.", SPROCESS_HERE );
	}
	if ( tokens[ 0 ] == "data" || tokens[0] == "attribute" )
    {
        _parse_element( tokens );
    }
    else if ( tokens[ 0 ] == "point_count" )
    {
        if ( tokens.size() != 2 )
        {
            std::cerr << ">> " << line << std::endl;
            throw exception( "invalid line in point header.", SPROCESS_HERE );
        }
        _point_info._point_count = _cast< size_t >( tokens, 1 );
    }
    else if ( tokens[ 0 ] == "endian" )
    {
        if ( tokens.size() != 2 )
        {
            std::cerr << ">> " << line << std::endl;
            throw exception( "invalid line in point header.", SPROCESS_HERE );
        }
        _point_info._input_is_big_endian = ( tokens[ 1 ] == "big" );
    }
    else if ( tokens[ 0 ] == "min" )
    {
        if ( tokens.size() != 4 )
        {
            std::cerr << ">> " << line << std::endl;
            throw exception( "invalid line in point header.", SPROCESS_HERE );
        }
        #ifdef SPROCESS_OLD_VMMLIB
        _point_info._min.x = _cast< double >( tokens, 1 );
        _point_info._min.y = _cast< double >( tokens, 2 );
        _point_info._min.z = _cast< double >( tokens, 3 );
        #else
        _point_info._min.x() = _cast< double >( tokens, 1 );
        _point_info._min.y() = _cast< double >( tokens, 2 );
        _point_info._min.z() = _cast< double >( tokens, 3 );
        #endif
    }
    else if ( tokens[ 0 ] == "max" )
    {
        if ( tokens.size() != 4 )
        {
            std::cerr << ">> " << line << std::endl;
            throw exception( "invalid line in point header.", SPROCESS_HERE );
        }
        #ifdef SPROCESS_OLD_VMMLIB
		_point_info._max.x = _cast< double >( tokens, 1 );
		_point_info._max.y = _cast< double >( tokens, 2 );
		_point_info._max.z = _cast< double >( tokens, 3 );
        #else
		_point_info._max.x() = _cast< double >( tokens, 1 );
		_point_info._max.y() = _cast< double >( tokens, 2 );
		_point_info._max.z() = _cast< double >( tokens, 3 );
        #endif
    }
    else if ( tokens[ 0 ] == "transform" )
    {
        if ( tokens.size() >= 17 )
        {
            std::vector< std::string > values;
            values.resize( tokens.size() - 1 );
            
            std::vector< std::string >::iterator input_it = tokens.begin();

            ++input_it; // we don't want the 'transform' token
            std::copy( input_it, tokens.end(), values.begin() );
            
            mat4f transform_;
            if ( transform_.set( values ) )
            {
                _point_info.set_transform( transform_ );
            }
            else
                throw exception( "parsing transform matrix4 failed.", 
                    SPROCESS_HERE );
        }
        else
            throw exception( "invalid transform information", SPROCESS_HERE );
    }

}



void
point_header::_parse_element( const std::vector< std::string >& tokens )
{
    data_type_helper& helper = data_type_helper::get_singleton();

    if ( tokens.size() == 3 )
    {
        rt_struct_member_info new_info( 
            tokens[ 1 ], 
            helper.get_data_type_id( tokens[ 2 ] ),
            1,
            true
        );
        new_info.is_input   = true;
        new_info.is_output  = true;
        _point_info.add_attribute( new_info );
    }
    else if ( tokens.size() == 4 || tokens.size() == 5 || tokens.size() == 6 )
    {
        rt_struct_member_info new_info( 
            tokens[ 1 ], 
            helper.get_data_type_id( tokens[ 2 ] ),
            _cast< size_t >( tokens, 3 ),
            true
        );
        new_info.is_input   = true;
        new_info.is_output  = true;
        _point_info.add_attribute( new_info );
    }
    else
    {
        throw exception( "invalid line in point header.", SPROCESS_HERE );
    }
}



void
point_header::write( const std::string& filename )
{
    std::ofstream out_stream;
    out_stream.open( filename.c_str() );
    if ( ! out_stream.is_open() || ! out_stream.good() )
    {
        throw exception( "opening point header file failed.", 
            SPROCESS_HERE );
    }
    
    out_stream << _point_info._serialize( true, true ) << std::endl;

    out_stream.close();
}



} // namespace stream_process

