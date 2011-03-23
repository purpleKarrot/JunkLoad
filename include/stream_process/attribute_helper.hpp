#ifndef __STREAM_PROCESS__ATTRIBUTE_HELPER__HPP__
#define __STREAM_PROCESS__ATTRIBUTE_HELPER__HPP__

#include <stream_process/point_set.hpp>
#include <stream_process/mapped_data_set.hpp>

#include <boost/lexical_cast.hpp>

namespace stream_process
{

class attribute_helper
{
public:
    
    
    std::string get_as_string( const point_set& ps, size_t point_index, 
        const std::string& attribute_name );

    std::string get_data_as_string( const stream_data* vertex_, 
        const attribute& attribute_, const std::string& seperator_ = ", " );


    
    template< typename T >
    const T& get_attribute( const stream_data* point, size_t offset, 
        size_t point_index, size_t array_index = 0 ) const;

    template< typename T >
    const T& get_attribute( const point_set& ps, const std::string& name,
        size_t point_index, size_t array_index = 0 ) const;

    template< typename T >
    std::string get_attribute_string( const stream_data* point, 
        const attribute& attr, const std::string& seperator_ = ", " ) const;

}; // class attribute_helper


template< typename T >
const T&
attribute_helper::
get_attribute( const point_set& ps, const std::string& name,
    size_t point_index, size_t array_index ) const
{
    attribute_accessor< T > get_attribute = ps.get_accessor< T >( name );
    const stream_data* point = ps[ point_index ];
    
    if ( array_index )
    {
        const T* t = & get_attribute( point );
        return *(t + array_index);
    }
    return get_attribute( point );
}



template< typename T >
const T&
attribute_helper::
get_attribute( const stream_data* point, size_t offset,
    size_t point_index, size_t array_index ) const
{
    attribute_accessor< T > get_attribute;
    get_attribute.set_offset( offset );

    if ( array_index )
    {
        const T* t = & get_attribute( point );
        return *(t + array_index);
    }
    return get_attribute( point );
}



template< typename T >
std::string
attribute_helper::
get_attribute_string( const stream_data* point, const attribute& attr,
    const std::string& seperator_ ) const
{
    std::string result;

    attribute_accessor< T > get_attribute;

    const size_t array_size = attr.get_number_of_elements();
    size_t offset           = attr.get_offset();
    
    for( size_t index = 0; index < array_size; ++index )
    {
        get_attribute.set_offset( offset );

        const T& t = get_attribute( point );
        
        if ( index != 0 )
            result += seperator_;
        result += boost::lexical_cast< std::string, T >( t );

        offset += sizeof( T );
    }
    return result;
}

} // namespace stream_process

#endif

