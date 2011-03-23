#include <stream_process/point_structure.hpp>

#include <stream_process/exception.hpp>

namespace stream_process
{
point_structure::point_structure( size_t content_type_ )
    : _content_type( content_type_ )
{}


point_structure::point_structure( const point_structure& point_structure_ )
{
    *this = point_structure_;
}


void
point_structure::set_is_face_structure()
{
    _content_type = 2;
}


void
point_structure::set_is_point_structure()
{
    _content_type = 1;
}


const point_structure&
point_structure::operator=( const point_structure& point_structure_ )
{
    clear();
    const_iterator
        it      = point_structure_.begin(),
        it_end  = point_structure_.end();
    for( ; it != it_end; ++it )
    {
        const attribute& attr = **it;
        create_attribute( attr );
    }
    return *this;
}


void
point_structure::clear()
{
    iterator it = begin(), it_end = end();
    for( ; it != it_end; ++it )
    {
        delete *it;
    }
    attr_container::clear();
    _by_name.clear();
}



bool
point_structure::has_attribute( const std::string& name ) const
{
    return _by_name.find( name ) != _by_name.end();   
}



bool
point_structure::has_attribute( const std::string& name,
    data_type_id id_, size_t array_size ) const
{
    attribute* attr_ptr = find( name );
    
    if ( ! attr_ptr )
        return false;
    
    if ( attr_ptr->get_data_type_id() == id_ 
        && attr_ptr->get_number_of_elements() == array_size )
    {
        return true;
    }
    return false;
}


attribute&
point_structure::get_attribute( const std::string& name )
{
    named_iterator it = _by_name.find( name );
    if ( it == _by_name.end() )
    {
        throw exception(
            std::string( "could not find attribute with name " ) + name + ".",
            SPROCESS_HERE
            ); 
    }

    return *(it->second);
}



const attribute&
point_structure::get_attribute( const std::string& name ) const
{
    const_named_iterator it = _by_name.find( name );
    if ( it == _by_name.end() )
    {
        throw exception(
            std::string( "could not find attribute with name " ) + name + ".",
            SPROCESS_HERE
            ); 
    }

    return *(it->second);
}



// creates an empty attribute with the specified name
attribute&
point_structure::create_attribute( const std::string& name,
    data_type_id data_type_id_, size_t array_size )
{
    attribute* new_attr = new attribute( name, data_type_id_, array_size );

    try
    {
        _add_attribute( *new_attr );
    }
    catch( exception& e)
    {
        delete new_attr;
        throw e;
    }

    return *new_attr;
}



// creates an empty attribute with the specified name
attribute&
point_structure::create_custom_attribute( const std::string& name, 
    size_t element_size_in_bytes, size_t number_of_elements )
{
    attribute* new_attr = new attribute( name, element_size_in_bytes,
        number_of_elements, true );

    try
    {
        _add_attribute( *new_attr );
    }
    catch( exception& e )
    {
        delete new_attr;
        throw e;
    }

    return *new_attr;
}




void
point_structure::create_attribute( const attribute& attr )
{
    if( attr.get_name() == "uninitialized" )
	{
        throw exception(
			std::string( "attempt to add uninitialized attribute to " )
			+ " point structure failed.",
            SPROCESS_HERE );
	}

    attribute* new_attr = new attribute( attr );

    try
    {
        _add_attribute( *new_attr );
    }
    catch( exception& e )
    {
        delete new_attr;
        throw e;
    }
}



void
point_structure::_add_attribute( attribute& attr )
{
    const std::string& name = attr.get_name();
    
    if ( has_attribute( name ) ) 
    {
        throw exception(
            std::string( "could not create attribute with name " ) + name
                + " - name is already taken.",
            SPROCESS_HERE
            ); 
    }

    named_container::value_type new_element( name, &attr );
    std::pair< named_iterator, bool > result = _by_name.insert( new_element );

    if( result.second != true )
    {
        throw exception(
            std::string( "could not create attribute with name " ) + name
                + ".",
            SPROCESS_HERE
            ); 
    }
    
    push_back( &attr );
}



attribute*
point_structure::find( const std::string& name ) const
{
    const_named_iterator it = _by_name.find( name );
    if ( it != _by_name.end() )
        return it->second;
    else
        return 0;
}



point_structure::~point_structure()
{
    iterator
        it      = begin(), 
        it_end  = end();
    for( ; it != it_end; ++it )
    {
        delete *it;
    }
}



size_t
point_structure::compute_size_in_bytes() const
{
    size_t size_in_bytes = 0;
    const_iterator
        it      = begin(),
        it_end  = end();
    for( ; it != it_end; ++it )
    {
        size_in_bytes += (*it)->get_size_in_bytes();
    }

    return size_in_bytes;
}



size_t
point_structure::compute_out_size_in_bytes() const
{
    size_t size_in_bytes = 0;
    const_iterator
        it      = begin(),
        it_end  = end();
    for( ; it != it_end; ++it )
    {
        const attribute& attr = **it;
        if ( attr.is_output() )
            size_in_bytes += (*it)->get_size_in_bytes();
    }

    return size_in_bytes;
}


std::string
point_structure::to_string() const
{
    assert( _by_name.size() == size() );

    std::string structure_string;
    if ( _content_type == 1 )
    {
        for( const_iterator it = begin(), it_end = end(); it != it_end; ++it )
        {
            const attribute& attr = **it;
            structure_string += attr.to_string();
        }
    }
    else if ( _content_type == 2 )
    {
        std::string face_prefix = "face_";
        for( const_iterator it = begin(), it_end = end(); it != it_end; ++it )
        {
            const attribute& attr = **it;
            structure_string += face_prefix + attr.to_string();
        }
    }

    return structure_string;
}


std::string
point_structure::to_header_string() const
{
    assert( _by_name.size() == size() );

    std::string structure_string= "# point structure: ";
    structure_string += "attribute-identifier, name, type, array_size";
    structure_string += ", size_in_bytes, flags\n";

    if ( _content_type == 1 )
    {
        for( const_iterator it = begin(), it_end = end(); it != it_end; ++it )
        {
            const attribute& attr = **it;
            if ( ! attr.is_output() )
                continue;
            structure_string += attr.to_header_string();
        }
    }
    else if ( _content_type == 2 )
    {
        std::string face_prefix = "face_";
        for( const_iterator it = begin(), it_end = end(); it != it_end; ++it )
        {
            const attribute& attr = **it;
            if ( ! attr.is_output() )
                continue;
            structure_string += face_prefix + attr.to_header_string();
        }
    }

    return structure_string;
}


void
point_structure::print( std::ostream& os ) const
{
    os << to_string() << std::endl;
    
}

} // namespace stream_process

