#include <stream_process/stream_point_configurator.hpp>
#include <stream_process/exception.hpp>
#include <stream_process/data_set_header.hpp>

namespace stream_process
{

stream_point_configurator::stream_point_configurator( data_set_header& header_ )
    : _sp_float_type( SP_FLOAT_32 )
    , _hp_float_type( SP_FLOAT_64 )
    , _header( header_ )
    , _stream_structure( _header.get_vertex_structure() )
{}



void
stream_point_configurator::_merge_inputs( const stream_structure& input_structure, 
    stream_structure& target_structure )
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

        if ( in_type == SP_FLOAT_32 || in_type == SP_FLOAT_64 )
        {
            if ( is_hp )
                stream_type = _hp_float_type;
            else
                stream_type = _sp_float_type;
        }
               
        attribute* out_attr = 0;
        if ( target_structure.has_attribute( in_attr.get_name() ) )
        {
            out_attr = & target_structure.get_attribute( in_attr.get_name() );
            if ( out_attr->get_data_type_id() != stream_type 
                || out_attr->get_number_of_elements() != number_of_elements )
            {
                throw exception( "invalid request for attribute.",
                    SPROCESS_HERE );
            }
        }
        else
            out_attr = & target_structure.create_attribute(
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
stream_point_configurator::setup_input( data_set_header& input_header_ )
{
    _merge_inputs( input_header_.get_vertex_structure(), 
        _header.get_vertex_structure() );

    std::cout << _header.get_vertex_structure() << std::endl;

    _merge_inputs( input_header_.get_face_structure(), 
        _header.get_face_structure() );
        
    std::cout << _header.get_face_structure() << std::endl;
        
    // FIXME proper triangle handling
    _header.set_number_of_faces( input_header_.get_number_of_faces() );
    
    _header.set_aabb_min< double >( input_header_.get_aabb_min< double >() );
    _header.set_aabb_max< double >( input_header_.get_aabb_max< double >() );
    _header.set_transform< double >( input_header_.get_transform< double >() );
}



bool
stream_point_configurator::has_attribute( const std::string& name,
    data_type_id data_type_id_, size_t array_size )
{
    // check if the attribute is registered in the stream point
    const attribute* attr_ptr = _stream_structure.find( name );
    if ( attr_ptr != 0 )
    {
        if ( attr_ptr->get_data_type_id() == data_type_id_ 
            &&  attr_ptr->get_number_of_elements() == array_size )
            return true;
    }
    return false;
}



attribute&
stream_point_configurator::add_attribute( const std::string& name,
    data_type_id data_type_id_, size_t array_size, bool write_to_output )
{
    attribute& attr = 
        _stream_structure.create_attribute( name, data_type_id_, array_size );
    if ( write_to_output )
        attr.set_is_output( true );
    return attr;
}



attribute&
stream_point_configurator::add_custom_attribute( const std::string& name,
    size_t size_in_bytes, bool write_to_output )
{
    attribute& attr = 
        _stream_structure.create_custom_attribute( name, size_in_bytes );
    if ( write_to_output )
        attr.set_is_output( true );
    return attr;
}



void
stream_point_configurator::print() const
{
    std::cout << _stream_structure << std::endl;

}


void
stream_point_configurator::finalize()
{
    // we sort the points according to
    // - if they are outputs (so the whole output data is one block)
    // - their offset (so inputs stay in order)
    
    _stream_structure.sort( attribute_ptr_outputs_first() );

    size_t offset = 0;
    
    point_structure::iterator
        it      = _stream_structure.begin(), 
        it_end  = _stream_structure.end();
    for( ; it != it_end; ++it )
    {
        attribute& attr = **it;
        attr.set_offset( offset );
        offset += attr.get_size_in_bytes();
    }
}


void
stream_point_configurator::
set_types( data_type_id sp_ft, data_type_id hp_ft )
{
    _sp_float_type = sp_ft;
    _hp_float_type = hp_ft;
}

} // namespace stream_process

