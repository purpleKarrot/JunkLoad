#ifndef __STREAM_PROCESS__ASCII_TO_POINT__HPP__
#define __STREAM_PROCESS__ASCII_TO_POINT__HPP__

#include "VMMLibIncludes.h"
#include "string_utils.h"

#include <vector>
#include <string>

namespace stream_process
{
class point_info;

class ascii_to_point
{
public:
    virtual ~ascii_to_point() {};

    virtual void setup_point_info( point_info& point_info_ )    = 0;
    virtual char* write_point( char* point_begin,
        const std::vector< std::string >& tokens ) = 0;

    template< typename T, size_t size >
    char* read_next_point( char*& current,
        std::vector< std::string >::const_iterator& it );

    template< size_t size >
    char* read_next_point_convert( char*& current,
        std::vector< std::string >::const_iterator& it );

protected:

}; // class ascii_to_point




template< typename T, size_t size >
char*
ascii_to_point::read_next_point( char*& current,
    std::vector< std::string >::const_iterator& it )
{
    vmml::vector< size, T >& new_data =
        *reinterpret_cast< vmml::vector< size, T >* >( current );
    
    for( size_t index = 0; index < size; ++index, ++it )
    {
        new_data( index )   = string_utils::from_string< T >( *it );
        
        //std::cout << "read " << index << ": " << new_data( index ) << std::endl;
    }
    current += sizeof( vmml::vector< size, T > );
    return current;
}



template< size_t size >
char*
ascii_to_point::read_next_point_convert( char*& current,
    std::vector< std::string >::const_iterator& it )
{
    vmml::vector< size, uint8_t >& new_data =
        *reinterpret_cast< vmml::vector< size, uint8_t >* >( current );
    
    for( size_t index = 0; index < size; ++index, ++it )
    {
        const size_t value = string_utils::from_string< size_t >( *it );
        new_data( index ) = static_cast< uint8_t >( value );

        //std::cout << "read " << index << ": " << value << std::endl;
    }

    current += sizeof( vmml::vector< size, uint8_t > );
    return current;
}





} // namespace stream_process

#endif

