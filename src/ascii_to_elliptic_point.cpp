#include "ascii_to_elliptic_point.hpp"

#include "point_info.hpp"
#include "string_utils.h"

namespace stream_process
{

void
ascii_to_elliptic_point::setup_point_info( point_info& point_info_ )
{
    // setup point_info
	point_info_.add_attribute( rt_struct_member_info( "position", SP_FLOAT_32,
		 sizeof(float), 3 , true, true) );
	point_info_.add_attribute( rt_struct_member_info( "color", SP_UINT_8,
		 sizeof( uint8_t ), 3 , true, true) );
	point_info_.add_attribute( rt_struct_member_info( "normal", SP_FLOAT_32,
		 sizeof(float), 3 , true, true) );
	point_info_.add_attribute( rt_struct_member_info( "axis", SP_FLOAT_32,
		 sizeof(float), 3 , true, true) );
	point_info_.add_attribute( rt_struct_member_info( "radius", SP_FLOAT_32,
		 sizeof( float ), 1 , true, true) );
	point_info_.add_attribute( rt_struct_member_info( "length", SP_FLOAT_32,
		 sizeof( float ), 1 , true, true) );
	point_info_.add_attribute( rt_struct_member_info( "ratio", SP_FLOAT_32,
		 sizeof( float ), 1 , true, true) );

    point_info_.update();
}


char*
ascii_to_elliptic_point::write_point( char* current,
    const std::vector< std::string >& tokens )
{
    if ( tokens.size() != 15 )
    {
        std::string msg( "encountered invalid vertex.");
        throw exception( msg.c_str(), SPROCESS_HERE );
    }
    
    std::vector< std::string >::const_iterator it = tokens.begin();
    
    // position
    read_next_point< float, 3 >( current, it );
    // color
    read_next_point_convert< 3 >( current, it );
    // normal
    read_next_point< float, 3 >( current, it );
    // axis
    read_next_point< float, 3 >( current, it );
    // radius
    read_next_point< float, 1 >( current, it );
    // radius
    read_next_point< float, 1 >( current, it );
    // radius
    read_next_point< float, 1 >( current, it );
    
    return current;
}


} // namespace stream_process

