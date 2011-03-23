#ifndef __STREAM_PROCESS__POINT_HEADER__HPP__
#define __STREAM_PROCESS__POINT_HEADER__HPP__

#include "point_info.hpp"

#include <boost/lexical_cast.hpp>

namespace stream_process
{

class point_header
{
public:
    point_header( point_info& info );

    void read( const std::string& filename );
    void write( const std::string& filename );

protected:
    void _parse_line( const std::string& line );
    void _parse_element( const std::vector< std::string >& tokens );
	
	template< typename T >
	T _cast( const std::vector< std::string >& tokens, size_t index );
	
	point_info&     _point_info;

}; // class point_header


template< typename T >
T point_header::_cast( const std::vector< std::string >& tokens, size_t index )
{
	T casted_value; 
	try
	{
		casted_value = boost::lexical_cast< T >( tokens[ index ] );
	}
	catch (std::exception& e)
	{
		std::cerr << "\nError at token " << index << " of line: ";
		std::vector< std::string >::const_iterator it = tokens.begin();
		std::vector< std::string >::const_iterator it_end = tokens.end();
		for( ; it != it_end; ++it )
		{
			std::cerr << *it << " ";
		}
		std::cerr << std::endl;
		throw exception( "casting value failed. ", SPROCESS_HERE ); 
	}
	return casted_value;
}

} // namespace stream_process

#endif

