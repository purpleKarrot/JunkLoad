#ifndef __STREAM_PROCESS__MAPPED_POINT_DATA__HPP__
#define __STREAM_PROCESS__MAPPED_POINT_DATA__HPP__


#include "stream_point.h"
#include "point_info.hpp"
#include <boost/iostreams/device/mapped_file.hpp>

#include <string>

/*
*   this class makes it easier to work with point data files (data+headers).
*
*/

namespace stream_process
{

class mapped_point_data
{
public:
    // ctor that initializes point_info and mem-maps the data file itself
    mapped_point_data( const std::string& points_file_name, 
		const std::string& header_file_name );

    // ctor with already initialized instances of point_info and 
    // the mapped file
    mapped_point_data( point_info& point_info_, 
        boost::iostreams::mapped_file& point_data, 
        const std::string& filename );

    // standard ctor does not yet initialize anything, you need to call 
    // setup_...() later - e.g. when you need to create a new point data file
    // and header
    mapped_point_data();

    ~mapped_point_data();
    
    stream_point* get_point( size_t index );
    const stream_point* get_point( size_t index ) const;
    
    // use this function to get a single attribute of a single point
    // if data from multiple or all points are needed, it's easier and faster 
    // to use point_info::get_rt_struct_member() & point(0) += point_size...
    // to extract the data.
    template< typename T >
    T* get_point_attribute( const std::string& name, size_t point_index );
    template< typename T >
    const T* get_point_attribute( const std::string& name, size_t point_index ) const;

    // accessors
    point_info& get_point_info();
    const point_info& get_point_info() const;
    
    char* get_data_ptr();
    const char* get_data_ptr() const;
    
    const std::string& get_filename() const;
    
    void write_header( const std::string& header_filename = "" );
    
    // setup functions, required when using the standard ctor
    void setup_point_info( const std::string& header_filename, 
        point_info* point_info_ = 0 );
    void setup_mmap( const std::string& data_filename );

    // use these functions when creating new point data
    void setup_empty_point_info();
    // PRE: point_info is set up properly, specifically it must contain
    // size_in_bytes per point, and point_count
    void setup_empty_mmap( const std::string& filename );

    // scan through all points and recompute aabb
    void recompute_aabb();
    
protected:
    void    _read_header( const std::string& filename );
    void    _mmap_data( const std::string& filename );

    point_info*                     _point_info;
    boost::iostreams::mapped_file*	_point_data;
    bool                            _clean_up_info_and_data_objects;
    
    std::string                     _filename;

private:
    // disallow copy ctor
    mapped_point_data( const mapped_point_data& mapped_point_data_ ) {};

}; // class mapped_point_data



template< typename T >
T*
mapped_point_data::get_point_attribute( const std::string& attribute_name, 
    size_t point_index )
{
    rt_struct_member< T > member 
        = _point_info->get_rt_struct_member_for_attribute< T >( attribute_name );
    stream_point* point = get_point( point_index );
    return point->get_ptr( member );
}



template< typename T >
const T*
mapped_point_data::get_point_attribute( const std::string& attribute_name, 
    size_t point_index ) const
{
    rt_struct_member< T > member 
        = _point_info->get_rt_struct_member_for_attribute< T >( attribute_name );
    const stream_point* point = get_point( point_index );
    return member.get_ptr( point );
}


/*
inline mapped_point_data::const_iterator
mapped_point_data::begin()
{
    return reinterpret_cast< const stream_point* >( _point_data.data() ); 
}



inline mapped_point_data::const_iterator
mapped_point_data::end()
{
    size_t point_size   = _point_info.get_size_in_bytes();
    size_t point_count  = _point_info.get_point_count();
    
    return reinterpret_cast< const stream_point* >( _point_data.data() 
        + point_size * point_count ); 
}
*/


} // namespace stream_process

#endif

