#ifndef __STREAM_PROCESS__STREAM_POINT_CONFIGURATOR__HPP__
#define __STREAM_PROCESS__STREAM_POINT_CONFIGURATOR__HPP__

#include <stream_process/point_structure.hpp>

#include <list>

namespace stream_process
{

// FIXME - refactor into data_set_header and data_set_header_merger ?

class stream_structure;
class data_set_header;

class stream_point_configurator
{
public:
    stream_point_configurator( data_set_header& header_ );

    void setup_input( data_set_header& input_header_ );
    
    bool has_attribute( const std::string& name, data_type_id data_type_id_,
        size_t array_size = 1 );

    attribute& add_attribute( const std::string& name, data_type_id id_, 
        size_t array_size = 1, bool write_to_output = true );

    attribute& add_custom_attribute( const std::string& name,
        size_t size_in_bytes, bool write_to_output = false );

    void finalize();

    void print() const;
    
    void set_types( data_type_id sp_ft, data_type_id hp_ft );
          
protected:
    void _merge_inputs( const stream_structure& inputs, 
        stream_structure& in_stream );


    data_type_id        _sp_float_type;
    data_type_id        _hp_float_type;
    
    data_set_header&    _header;

    point_structure&    _stream_structure;
    
}; // class stream_point_configurator

} // namespace stream_process

#endif

