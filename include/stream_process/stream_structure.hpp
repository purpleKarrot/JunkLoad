#ifndef __STREAM_PROCESS__STREAM_STRUCTURE__HPP__
#define __STREAM_PROCESS__STREAM_STRUCTURE__HPP__

#include <stream_process/attribute.hpp>
#include <stream_process/attribute_type.hpp>
#include <stream_process/exception.hpp>

#include <iostream>

#include <map>
#include <list>

namespace stream_process
{

class stream_structure : private std::list< attribute* > 
{
public:
    typedef std::list< attribute* >     super;

    stream_structure( const std::string& attribute_identifier_ );

    stream_structure( const stream_structure& stream_structure_ );
    ~stream_structure();

    const stream_structure& operator=( const stream_structure& point_structure_ );
    
    size_t get_number_of_attributes() const;
    
    void clear();
    
    bool has_attribute( const std::string& name ) const;
    bool has_attribute( const std::string& name, 
		data_type_id id_, size_t array_size ) const;

    attribute&          get_attribute( const std::string& name );
    const attribute&    get_attribute( const std::string& name ) const;

    void create_attribute( const attribute& attr );

	template< typename T >
	attribute& create_attribute( const std::string& name, size_t array_size = 0 );

    attribute&  create_attribute( const std::string& name, 
        data_type_id data_type_id_, size_t array_size = 1 );

    attribute&  create_custom_attribute( const std::string& name, 
        size_t element_size_in_bytes, size_t array_size = 1 );

    // WARNING: do not use unless you know exactly what you are doing.
    // stuff will break!
    void delete_attribute( const std::string& attr_name_ );

    // iterators
    using super::iterator;
    using super::const_iterator;
    using super::begin;
    using super::end;

    using super::sort;
    using super::size;
    using super::empty;
    
    typedef std::map< std::string, attribute* > named_container;
    typedef named_container::iterator           named_iterator;
    typedef named_container::const_iterator     const_named_iterator;

    attribute* find( const std::string& name ) const;

    // string stuff
    std::string to_string() const; 
    // creates the string in 'header-format'
    std::string to_header_string() const;

    void print( std::ostream& os ) const;
    friend std::ostream& operator<<( std::ostream& os, const stream_structure& ds )
    {
        os << ds.to_string() << std::endl;
        return os;
    }
    
    size_t compute_size_in_bytes() const;
    size_t compute_out_size_in_bytes() const;
    
    void compute_offsets();
    
    const std::string& get_name() const;
    
    void merge_input( const stream_structure& input_structure );
    
protected:
    friend class    point_structure_sort;

    void        _add_attribute( attribute& attr );
    
    std::string _attribute_identifier; // for to_header_string
    std::map< std::string, attribute* > _by_name;

}; // class stream_structure



template< typename T >
attribute&
stream_structure::create_attribute( const std::string& name, size_t array_size )
{
	attribute_type< T >	attribute_type_;
	attribute* attr = 0;
	try
	{
        if ( array_size == 0 )
            attr = attribute_type_.create( name );
        else
            attr = attribute_type_.create( name, array_size );        
		assert( attr );
		_add_attribute( *attr );
	}
	catch( exception& e )
	{
		delete attr;
		throw e;
	}
	return *attr;
}


} // namespace stream_process

#endif

