#include "rt_struct_member_info2.hpp"

namespace stream_process
{

rt_struct_member_info::rt_struct_member_info( const std::string& name_, 
    data_type_id type_, size_t array_size_, bool is_input_, bool is_output_ )
    : name( name_ )
    , type( type_ )
    , array_size( array_size_ )
    , is_input( is_input_ )
    , input_offset( -1 )
    , stream_offset( -1 )
    , is_output( is_output_ )
    , output_offset( -1 )
{
    element_size_in_bytes 
        = data_type_helper::get_singleton().get_size_in_bytes( type );
}



rt_struct_member_info::rt_struct_member_info( const std::string& name_, 
    data_type_id type_, size_t element_size_in_bytes_, size_t array_size_, 
    bool is_input_, bool is_output_ )
    : name( name_ )
    , type( type_ )
    , array_size( array_size_ )
    , element_size_in_bytes( element_size_in_bytes_ )
    , is_input( is_input_ )
    , input_offset( -1 )
    , stream_offset( -1 )
    , is_output( is_output_ )
    , output_offset( -1 )
{}  



size_t 
rt_struct_member_info::get_size_in_bytes() const
{
    return element_size_in_bytes * array_size;
}



bool
rt_struct_member_info::operator==( const rt_struct_member_info& other ) const
{
    return name         == other.name 
        && type         == other.type
        && array_size   == other.array_size;
}



bool 
rt_struct_member_info::operator!=( const rt_struct_member_info& other ) const
{
    return !operator==( other );
}



bool
rt_struct_member_info::has_setting( const std::string& key ) const
{
    std::map< std::string, std::string >::const_iterator it
        = other_settings.find( key );
    return it != other_settings.end();
}



const std::string&
rt_struct_member_info::get_setting( const std::string& key ) const
{
    std::map< std::string, std::string >::const_iterator it 
        = other_settings.find( key );
    if ( it != other_settings.end() )
    {
        return (*it).second;
    }
    std::string msg( "could not find setting with key " );
    msg += key;
    msg += ". ";
    throw exception( msg.c_str(), SPROCESS_HERE );
}



void
rt_struct_member_info::set_setting( const std::string& key,
    const std::string& value )
{
    other_settings[ key ] = value;
}

} // namespace stream_process

