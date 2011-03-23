#ifndef __STREAM_PROCESS__STREAM_CONFIG__HPP__
#define __STREAM_PROCESS__STREAM_CONFIG__HPP__

#include <stream_process/point_set_header.hpp>
#include <stream_process/stream_point_configurator.hpp>
#include <stream_process/stream_options.hpp>
#include <stream_process/attribute_accessor.hpp>

#include <stream_process/data_set.hpp>

#include <cstddef>

namespace stream_process
{

// contains static config data (static as in, will not change once processing started )

class stream_config
{
public:
    stream_config( stream_options& options );
        
    void setup_input();
    void setup_point();
    
    #if 0
    bool has_attribute( const std::string& name, data_type_id data_type_id_,
        size_t array_size = 1 );

    attribute& add_attribute( const std::string& name, data_type_id id_, 
        size_t array_size = 1, bool write_to_output = false );

    attribute&  add_custom_attribute( const std::string& name, 
        size_t element_size_in_bytes, size_t number_of_elements = 1 );

    template< typename T >
    void add_attribute( const std::string& name, size_t array_size = 1,
        bool write_to_output = false );
    #endif
    
    mapped_data_set&    setup_input_data_set( const std::string& base_filename );

    mapped_data_set&    get_input_data_set();
    data_set_header&    get_input_header();
       
    stream_options&     get_options();
    
    data_set_header&    get_header();
    stream_structure&   get_vertex_structure();
    stream_structure&   get_face_structure();
    
    data_type_id        get_sp_float_type() const;
    data_type_id        get_hp_float_type() const;

    void set_types( data_type_id sp_, data_type_id hp );
    
    void print_structures( std::ostream& os = std::cout ) const;

protected:
        
    stream_options&             _options;

    data_type_id                _sp_float_type;
    data_type_id                _hp_float_type;

    data_set_header             _header;
    stream_structure&           _vertex_structure;
    stream_structure&           _face_structure;
    stream_point_configurator   _configurator;
   
    mapped_data_set*            _input_data_set;
    point_set_header*           _input_header;
        
}; // class stream_config


#if 0
template< typename T >
void
stream_config::add_attribute( const std::string& name, size_t array_size,
    bool write_to_output )
{
    get_data_type_id_from_type< T > get_id;
    add_attribute( name, get_id(), array_size, write_to_output );
}
#endif

} // namespace stream_process

#endif

