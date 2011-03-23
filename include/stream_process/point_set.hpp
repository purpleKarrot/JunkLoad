#ifndef __STREAM_PROCESS__POINT_SET__HPP__
#define __STREAM_PROCESS__POINT_SET__HPP__

#include <stream_process/attribute_accessor.hpp>
#include <stream_process/point_set_header.hpp>

#include <boost/filesystem.hpp>
#include <boost/iostreams/device/mapped_file.hpp>

namespace stream_process
{
// a class for simple read-only access to point sets

namespace io = boost::iostreams;
namespace fs = boost::filesystem;

class point_set
{
public:
    typedef io::mapped_file_source  mapped_file_source;
    typedef fs::path                path;
       
    void load( const std::string& base_file_name );
    
    inline bool has_attribute( const std::string& name );

    template< typename T > attribute_accessor< T >
        get_accessor( const std::string& attr_name ) const;

    inline const stream_data* operator[]( size_t index ) const;

    const point_set_header& get_header() const;
    
protected:
    void exists( const path& path_ );
    
    point_set_header    _header;
    mapped_file_source  _mapped_file;
    
    size_t              _point_size_in_bytes;

}; // class point_set


template< typename T >
attribute_accessor< T >
point_set::get_accessor( const std::string& attr_name ) const
{
    const stream_data_structure& ps     = _header.get_vertex_structure();

    attribute_accessor< T > accessor;
    accessor.setup_attribute( ps, attr_name );
    accessor.setup();
    return accessor;
}


inline bool
point_set::has_attribute( const std::string& name )
{
    return _header.get_vertex_structure().has_attribute( name );
}



inline const stream_data*
point_set::operator[]( size_t index ) const
{
    if ( index >= _header.get_number_of_vertices() )
    {
        throw exception( "index out of bounds", SPROCESS_HERE );
    }

    const char* data = _mapped_file.data();
    data += _point_size_in_bytes * index;
    return reinterpret_cast< const stream_data* >( data );
}


} // namespace stream_process

#endif

