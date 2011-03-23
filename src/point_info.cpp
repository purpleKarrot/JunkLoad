#include "point_info.hpp"

#include "data_types.hpp"

#include <sstream>
#include <iomanip>

namespace stream_process
{
point_info::point_info()
    : rt_struct_info()
    , _point_count( 0 )
    , _min( 0, 0, 0 )
    , _max( 0, 0, 0 )
#ifdef  __BIG_ENDIAN__
    , _input_is_big_endian( true )
    , _output_is_big_endian( true )
#else
    , _input_is_big_endian( false )
    , _output_is_big_endian( false )
#endif
    , _has_transform( false )
    , _transform( mat4f::IDENTITY )
{

}



size_t
point_info::get_point_count() const
{
    return _point_count;
}



const vec3f&
point_info::get_min() const
{
    return _min;
}



const vec3f&
point_info::get_max() const
{
    return _max;
}



bool
point_info::requires_endian_conversion() const
{
    return _input_is_big_endian != _output_is_big_endian;
}



void
point_info::set_point_count( size_t point_count )
{
	_point_count = point_count;
}



void
point_info::set_min( const vec3f& min )
{
	_min = min;
}



void
point_info::set_max( const vec3f& max )
{
	_max = max;
}



size_t
point_info::get_point_set_size_in_bytes() const
{
    return get_point_size_in_bytes() * get_point_count();
}



std::string
point_info::_serialize( bool with_comments, bool only_out ) const
{
    std::stringstream pi;
    pi.precision( 8 * sizeof( void* ) - 1 );
    data_type_helper& helper = data_type_helper::get_singleton();

    if ( with_comments )
        pi  << "# point header generated by stream_process\n\n";
    
    pi  << "point_count " << _point_count << "\n\n" 
    #ifdef SPROCESS_OLD_VMMLIB
        << "min "   << _min.x << " " 
                    << _min.y << " " 
                    << _min.z << "\n"
        << "max "   << _max.x << " "
                    << _max.y << " "
                    << _max.z << "\n\n"
    #else
        << "min "   << _min.x() << " " 
                    << _min.y() << " " 
                    << _min.z() << "\n"
        << "max "   << _max.x() << " "
                    << _max.y() << " "
                    << _max.z() << "\n\n"
    #endif
        << "endian ";
    
    if ( _input_is_big_endian )
        pi << "big\n\n";
    else
        pi << "little\n\n";

    if ( _has_transform )
    {
        std::string transform_;
        _transform.getString( transform_ );
        pi << "transform " << transform_ << "\n\n";
    }

    if ( with_comments )
        pi << "#        name               data_type   array_size\n";

    const_iterator it     = begin();
    const_iterator it_end = end();
    
    for( ; it != it_end; ++it )
    {
        const rt_struct_member_info& info = *it;
        if ( ! only_out || info.is_output )
        {
            pi  << "data " << std::setw( 12 ) << info.name;
            try 
            {
                pi << std::setw( 16 ) << helper.get_default_name( info.type );
            }
            catch( exception& e )
            {
                pi << std::setw( 16 ) << "custom";
            }    
            pi << std::setw( 8 )  << info.array_size << "\n";
        }
    }
    
    return pi.str();
}



void
point_info::set_transform( const mat4f& transform_ )
{
    _transform = transform_;
    _has_transform = true;
}



const mat4f&
point_info::get_transform() const
{
    return _transform;
}



bool
point_info::has_transform()
{
    return _has_transform;
}



} // namespace stream_process

