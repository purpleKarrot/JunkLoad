#ifndef __STREAM_PROCESS__RT_STRUCT_INFO__HPP__
#define __STREAM_PROCESS__RT_STRUCT_INFO__HPP__

#include "rt_struct_member_info2.hpp"
#include "data_types.hpp"

#include <vector>
#include <iostream>
#include <functional>
#include <iomanip>

namespace stream_process
{

class rt_struct_info
{

public:
    typedef std::vector< rt_struct_member_info >::iterator iterator;
    typedef std::vector< rt_struct_member_info >::const_iterator const_iterator;
    
    void add_attribute( const rt_struct_member_info& info );
    bool has_attribute( const std::string& name ) const;
    bool has_attribute_of_type( const std::string& name, data_type_id type_ ) const;
	
    size_t get_attribute_count() const;

    // WARNING: sorting will invalidate this information.
    size_t get_index_of_attribute( const std::string& name ) const;

    size_t get_point_size_in_bytes() const;
    // DEPRECATED. this is the point size in bytes.
	size_t get_size_in_bytes() const;

    size_t get_offset_to_attribute( const std::string& name ) const;

    // this sorts the info as follows:
    // first all input attributes in the original order
    // then all output attributes 
    // then any other attributes
    void sort();

	// same as find, but throws exception if no attribute 'name' exists.
	rt_struct_member_info& get_attribute_info( const std::string& name );
	const rt_struct_member_info& 
        get_attribute_info( const std::string& name ) const;
        
    // sets in, stream, and out-offsets
    //void set_offsets_for_attribute( const std::string& name, size_t offset );

    iterator        find( const std::string& name );
    const_iterator  find( const std::string& name ) const;
    
    iterator        begin()         { return _members.begin(); };
    iterator        end()           { return _members.end();   };
    const_iterator  begin() const   { return _members.begin(); };
    const_iterator  end() const     { return _members.end();   };
    
    friend std::ostream& operator<<( std::ostream& os, const rt_struct_info& info )
    {
        size_t offset = 0;
        data_type_helper& helper = data_type_helper::get_singleton();
        const_iterator it     = info.begin();
        const_iterator it_end = info.end();
        for( ; it != it_end; ++it )
        {
            const rt_struct_member_info& info = (*it);
            os << std::setw( 16 ) << info.name << " ";

            if ( info.is_input )
                os << std::setw( 4 ) << "in";
            else 
                os << std::setw( 4 ) << "";

            if ( info.is_output )
                os << std::setw( 4 ) << "out";        
            else
                os << std::setw( 4 ) << "";
            
            try 
            {
                os << std::setw( 8 ) << helper.get_default_name( info.type )
                    << " " << std::setw( 8 ) << info.array_size
                    << std::setw( 16 ) << " "
                    << offset
                    << "\n";
            }
            catch( exception& e )
            {
                os  << std::setw( 8 ) << "custom"
                    << " " << std::setw( 8 ) << info.array_size 
                    << " *" << std::setw( 4 ) << info.element_size_in_bytes 
                    << " bytes    " 
                    << offset
                    << "\n";
            }    
            offset += info.array_size * info.element_size_in_bytes;
        }
        os << std::endl;
        return os;
    }
    
    void update();
    
protected:
    void _register( const rt_struct_member_info& info );
    void _update();
	void _compute_and_set_offsets();
    
    std::vector< rt_struct_member_info >   _members;
    // offset into _members vector
    std::map< std::string, size_t >        _members_by_name;
    
}; // class rt_struct_info



template< typename base_compare_t >
struct rt_struct_info_compare 
    : std::binary_function< 
        rt_struct_member_info const&, 
        rt_struct_member_info const&,
        bool >
{
    bool operator()( const rt_struct_member_info& first,
        const rt_struct_member_info& second )
    {
        return compare( first.get_size_in_bytes(), 
            second.get_size_in_bytes() );
    }
    
    base_compare_t  compare;
}; // struct rt_struct_info_sort

} // namespace stream_process

#endif

