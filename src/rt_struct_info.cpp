#include "rt_struct_info.hpp"

#include <algorithm>

namespace stream_process
{

rt_struct_member_info&
rt_struct_info::get_attribute_info( const std::string& name )
{
    assert( !name.empty() );
    std::map< std::string, size_t >::iterator it 
        = _members_by_name.find( name );
    if ( it == _members_by_name.end() )
	{
        std::string error_msg = 
            std::string( "could not find the requested member " ) 
                + name 
                + ".";
		throw exception( error_msg.c_str(), 
			SPROCESS_HERE );
	}
	return _members[ (*it).second ];
}




const rt_struct_member_info&
rt_struct_info::get_attribute_info( const std::string& name ) const
{
    assert( !name.empty() );
    std::map< std::string, size_t >::const_iterator it 
        = _members_by_name.find( name );
    if ( it == _members_by_name.end() )
	{
        std::string error_msg = 
            std::string( "could not find the requested member " ) 
                + name 
                + ".";
		throw exception( error_msg.c_str(), 
			SPROCESS_HERE );
	}
	return _members[ (*it).second ];
}



void
rt_struct_info::add_attribute( const rt_struct_member_info& info )
{
    std::map< std::string, size_t >::const_iterator it 
        = _members_by_name.find( info.name );
    if ( it != _members_by_name.end() )
    {   // a member with same name is already registered
        if ( info == _members[ (*it).second ] )
        {
            // everything is ok, it's the same member
            return;
        }
        else
        {
            throw exception( 
                std::string( "adding attribute " )
                + info.name 
                + " failed, the attribute-name is already registered.",
                SPROCESS_HERE );
        }
    }
    else
    {
        _register( info );
    }
}



void 
rt_struct_info::_register( const rt_struct_member_info& info )
{
    size_t index = _members.size();
    _members.push_back( info );
    _members_by_name[ info.name ] = index; 
}



bool
rt_struct_info::has_attribute( const std::string& name ) const
{
    std::map< std::string, size_t >::const_iterator it 
        = _members_by_name.find( name );
    return it != _members_by_name.end();
}



bool
rt_struct_info::has_attribute_of_type( const std::string& name, 
    data_type_id type_ ) const
{
    std::map< std::string, size_t >::const_iterator it 
        = _members_by_name.find( name );
    if ( it == _members_by_name.end() )
        return false;
    return ( _members[ (*it).second ].type == type_ );
}



size_t
rt_struct_info::get_index_of_attribute( const std::string& name ) const
{
    std::map< std::string, size_t >::const_iterator it 
        = _members_by_name.find( name );
    if ( it != _members_by_name.end() )
        return (*it).second;
    throw exception( "requested index of unknown member", SPROCESS_HERE );
    
}



rt_struct_info::iterator
rt_struct_info::find( const std::string& name )
{
    std::map< std::string, size_t >::const_iterator it 
        = _members_by_name.find( name );
    if ( it == _members_by_name.end() )
    {
        return _members.end();
    }
    return _members.begin() + (*it).second;
}



rt_struct_info::const_iterator
rt_struct_info::find( const std::string& name ) const
{
    std::map< std::string, size_t >::const_iterator it 
        = _members_by_name.find( name );
    if ( it == _members_by_name.end() )
    {
        return _members.end();
    }
    return _members.begin() + (*it).second;
}



void 
rt_struct_info::sort()
{
    iterator it         = begin();
    iterator it_end     = end();
    iterator swap_pos   = begin();

    // move input and output to the beginning
    for( ; it != it_end; ++it )
    {
        if ( (*it).is_input )
        {
            swap_pos = it + 1;
        }
        else if ( (*it).is_output )
        {
            if ( it != swap_pos )
                std::iter_swap( it, swap_pos );            
            ++swap_pos;
        }
    }
    // sort the rest of the members according to size
    for( it = begin(); it != it_end; ++it )
    {
        if ( ! (*it).is_input && ! (*it).is_output )
        {
            break;
        }
    }
    rt_struct_info_compare< std::greater< size_t > > compare_functor;
    std::sort( it, it_end, compare_functor );
	
    _update();
}



void
rt_struct_info::_update()
{
    _members_by_name.clear();
    size_t member_count = _members.size();
    for( size_t index = 0; index < member_count; ++index )
    {
        _members_by_name[ _members[ index ].name ] = index;
        //std::cout << _members[ index ].name << " at index " << index << std::endl;
    }

	_compute_and_set_offsets();
}



size_t 
rt_struct_info::get_attribute_count() const
{
    return _members.size();
}



size_t
rt_struct_info::get_size_in_bytes() const
{
    return get_point_size_in_bytes();
}



size_t
rt_struct_info::get_point_size_in_bytes() const
{
	size_t size_in_bytes = 0;
	for( const_iterator it = begin(), it_end = end(); it != it_end; ++it )
	{
		size_in_bytes += (*it).get_size_in_bytes();
	}
	return size_in_bytes;
}



void
rt_struct_info::_compute_and_set_offsets()
{
	size_t offset = 0;
	for( iterator it = begin(), it_end = end(); it != it_end; ++it )
	{
		rt_struct_member_info& info = *it;
		info.input_offset = offset;
		info.stream_offset = offset;
		offset += info.get_size_in_bytes();
	}
}


void
rt_struct_info::update()
{
    _update();
}



size_t
rt_struct_info::get_offset_to_attribute( const std::string& name ) const
{
    return get_attribute_info( name ).stream_offset;
}



} // namespace stream_process

