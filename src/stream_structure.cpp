#include <stream_process/stream_structure.hpp>

#include <stream_process/exception.hpp>

namespace stream_process
{

stream_structure::stream_structure( const std::string& attribute_identifier_ )
    : _attribute_identifier( attribute_identifier_ )
{}


stream_structure::stream_structure( const stream_structure& src )
    : _attribute_identifier( src._attribute_identifier )
{
    for( const_iterator it = src.begin(), it_end = src.end(); it != it_end; ++it )
    {
        const attribute& attr = **it;
        create_attribute( attr );
    }
}


const stream_structure&
stream_structure::operator=( const stream_structure& stream_structure_ )
{
    clear();
    const_iterator
        it      = stream_structure_.begin(),
        it_end  = stream_structure_.end();
    for( ; it != it_end; ++it )
    {
        const attribute& attr = **it;
        create_attribute( attr );
    }
    return *this;
}


void
stream_structure::clear()
{
    iterator it = begin(), it_end = end();
    for( ; it != it_end; ++it )
    {
        delete *it;
    }
    super::clear();
    _by_name.clear();
}



bool
stream_structure::has_attribute( const std::string& name ) const
{
    return _by_name.find( name ) != _by_name.end();   
}



bool
stream_structure::has_attribute( const std::string& name,
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
stream_structure::get_attribute( const std::string& name )
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
stream_structure::get_attribute( const std::string& name ) const
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
stream_structure::create_attribute( const std::string& name,
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
stream_structure::create_custom_attribute( const std::string& name, 
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
stream_structure::create_attribute( const attribute& attr )
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
stream_structure::_add_attribute( attribute& attr )
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
stream_structure::find( const std::string& name ) const
{
    const_named_iterator it = _by_name.find( name );
    if ( it != _by_name.end() )
        return it->second;
    else
        return 0;
}



stream_structure::~stream_structure()
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
stream_structure::compute_size_in_bytes() const
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
stream_structure::compute_out_size_in_bytes() const
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



void
stream_structure::compute_offsets()
{
    size_t offset = 0;
    for( iterator it = begin(), it_end = end(); it != it_end; ++it )
    {
        attribute& attr = **it;
        attr.set_offset( offset );
        offset += attr.get_size_in_bytes();
    }
}



std::string
stream_structure::to_string() const
{
    assert( _by_name.size() == size() );

    std::string structure_string;
    for( const_iterator it = begin(), it_end = end(); it != it_end; ++it )
    {
        const attribute& attr = **it;
        structure_string += attr.to_string();
    }
    return structure_string;
}



std::string
stream_structure::to_header_string() const
{
    assert( _by_name.size() == size() );

    std::string structure_string = "";
    structure_string += "# element, attribute-identifier, name, type, ";
    structure_string += " array_size, size_in_bytes, flags\n";

    for( const_iterator it = begin(), it_end = end(); it != it_end; ++it )
    {
        const attribute& attr = **it;
        if ( attr.is_output() )
            structure_string += attr.to_header_string( _attribute_identifier );
    }
    return structure_string;
}


void
stream_structure::print( std::ostream& os ) const
{
    os << to_string() << std::endl;
    
}


const std::string&
stream_structure::get_name() const
{
    return _attribute_identifier;
}



void
stream_structure::merge_input( const stream_structure& input_structure )
{
    stream_structure::const_iterator
        in_it       = input_structure.begin(), 
        in_it_end   = input_structure.end();
    for( ; in_it != in_it_end; ++in_it )
    {
        const attribute& in_attr    = **in_it;

        data_type_id in_type        = in_attr.get_data_type_id();
        size_t number_of_elements   = in_attr.get_number_of_elements();
        bool is_hp                  = in_attr.is_high_precision();

        data_type_id stream_type    = in_type;

#if 0
        if ( in_type == SP_FLOAT_32 || in_type == SP_FLOAT_64 )
        {
            if ( is_hp )
                stream_type = hp_float_type_;
            else
                stream_type = sp_float_type_;
        }
#endif
               
        attribute* out_attr = 0;
        if ( has_attribute( in_attr.get_name() ) )
        {
            out_attr = & get_attribute( in_attr.get_name() );
            if ( out_attr->get_data_type_id() != stream_type 
                || out_attr->get_number_of_elements() != number_of_elements )
            {
                throw exception( "invalid request for attribute.",
                    SPROCESS_HERE );
            }
        }
        else
            out_attr = & create_attribute(
                in_attr.get_name(),
                stream_type,
                number_of_elements 
            );
        
        assert( out_attr->get_name() != "uninitialized" );

        out_attr->set_is_high_precision( is_hp );

        // all input attributes are output attributes by default
        out_attr->set_is_output( true );
    }

}



void
stream_structure::delete_attribute( const std::string& attr_name_ )
{
    named_iterator it = _by_name.find( attr_name_ );
    if ( it == _by_name.end() )
    {
        throw exception(
            std::string( "could not find attribute with name " ) + attr_name_ + ".",
            SPROCESS_HERE
            ); 
    }

    attribute* attr = it->second;
    _by_name.erase( it );

    iterator ait = begin(), ait_end = end();
    for( ; ait != ait_end; ++ait )
    {
        attribute* a = *ait;
        if ( a == attr )
        {
            erase( ait );
            break;
        }
    }
}



size_t
stream_structure::get_number_of_attributes() const
{
    return super::size();
}





} // namespace stream_process
