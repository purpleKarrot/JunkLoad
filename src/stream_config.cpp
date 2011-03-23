#include <stream_process/stream_config.hpp>

namespace stream_process
{

stream_config::stream_config( stream_options& options_ )
    : _options( options_ ) 
    , _sp_float_type( SP_FLOAT_32 )
    , _hp_float_type( SP_FLOAT_64 )
    , _header()
    , _vertex_structure( _header.get_vertex_structure() )
    , _face_structure( _header.get_face_structure() )
    , _configurator( _header )
    , _input_data_set( 0 )
    , _input_header( 0 )
{
    _configurator.set_types( _sp_float_type, _hp_float_type );
}



void
stream_config::setup_input()
{
    assert( _input_header );
  
    #if 1
    _header.merge_input( *_input_header );
    _header.update_float_precision( _sp_float_type, _hp_float_type );
    
    #else
    _configurator.setup_input( *_input_header );
    _header.set_aabb_min< double >( _input_header->get_aabb_min< double >() );
    _header.set_aabb_max< double >( _input_header->get_aabb_max< double >() );
    _header.set_transform< double >( _input_header->get_transform< double >() );

    #endif
}


void
stream_config::setup_point()
{
    _header.finalize_structures();
    //_configurator.finalize();
    //_header.finalize_structures();
}


stream_options&
stream_config::get_options()
{
    return _options;
}

#if 0
bool
stream_config::has_attribute( const std::string& name, data_type_id data_type_id_,
    size_t array_size )
{
    return _configurator.has_attribute( name, data_type_id_, array_size );
}


attribute&
stream_config::add_attribute( const std::string& name, data_type_id id_, 
    size_t array_size, bool write_to_output )
{
    return _configurator.add_attribute( name, id_, array_size, write_to_output );
}


attribute&
stream_config::add_custom_attribute( const std::string& name, 
    size_t element_size_in_bytes, size_t number_of_elements )
{
    return _configurator.add_custom_attribute( name, element_size_in_bytes,
        number_of_elements );
}
#endif


mapped_data_set&
stream_config::setup_input_data_set( const std::string& base_filename )
{
    _input_data_set = new data_set( base_filename );
    _input_header   = & _input_data_set->get_header();
    return *_input_data_set;
}



mapped_data_set&
stream_config::get_input_data_set()
{
    assert( _input_data_set );
    return *_input_data_set;
}



data_set_header&
stream_config::get_header()
{
    return _header;
}



data_set_header&
stream_config::get_input_header()
{
    assert( _input_header );
    return *_input_header;
}


stream_structure&
stream_config::get_vertex_structure()
{
    return _vertex_structure;
}



stream_structure&
stream_config::get_face_structure()
{
    return _face_structure;
}



data_type_id
stream_config::get_sp_float_type() const
{
    return _sp_float_type;
}



data_type_id
stream_config::get_hp_float_type() const
{
    return _hp_float_type;
}


void
stream_config::set_types( data_type_id sp_, data_type_id hp_ )
{
    _sp_float_type = sp_;
    _hp_float_type = hp_;
    _configurator.set_types( _sp_float_type, _hp_float_type );
}


void
stream_config::print_structures( std::ostream& os ) const
{
    data_set_header::const_iterator
        it = _header.begin(), it_end = _header.end();
    for( ; it != it_end; ++it )
    {
        const data_element& e       = **it;
        const stream_structure& s   = e.get_structure();
        
        os << "structure of element " << e.get_name() << ":\n";
        os << s;
        //os << "total size: " << s.compute_size_in_bytes() << " bytes."
        os << std::endl;
    }


}


} // namespace stream_process

