#ifndef __STREAM_PROCESS__RT_STRUCT_MEMBER_DECLARATION__HPP__
#define __STREAM_PROCESS__RT_STRUCT_MEMBER_DECLARATION__HPP__

#include <string>

#include "data_types.hpp"
#include "exception.hpp"

#include "rt_struct_member.h"

namespace stream_process
{

// TODO should be rts_attribute_info or rts_attribute_metadata or so
struct rt_struct_member_info
{
    rt_struct_member_info( const std::string& name_, data_type_id type_, 
        size_t array_size_, bool is_input_ = false, bool is_output_ = false
            );
            
    rt_struct_member_info( const std::string& name_, data_type_id type_, 
        size_t element_size_in_bytes_, size_t array_size_, 
        bool is_input_ = false, bool is_output_ = false
            );
            
    size_t get_size_in_bytes() const;
    bool operator==( const rt_struct_member_info& other ) const;
    bool operator!=( const rt_struct_member_info& other ) const;

    // simple system for storing any kind of additional info on the attribute
    bool has_setting( const std::string& key ) const;
    const std::string& get_setting( const std::string& key ) const;
    void set_setting( const std::string& key, const std::string& value );

    // FIXME protected + accessors

    std::string     name;
    data_type_id    type;
    size_t          array_size;
    size_t          element_size_in_bytes;

    bool            is_input;
    ssize_t         input_offset;

    ssize_t         stream_offset;
    
    bool            is_output;
    ssize_t         output_offset;
    
    std::map< std::string, std::string > other_settings;
       
}; // struct rt_struct_member_info

} // namespace stream_process

#endif

