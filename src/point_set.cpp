#include <stream_process/point_set.hpp>

#include <stream_process/file_suffix_helper.hpp>

namespace stream_process
{

void
point_set::exists( const path& path_ )
{
    if ( ! fs::exists( path_ ) )
    {
        throw exception( 
            std::string( "file " ) + path_.string() 
                + " could not be found.",
            SPROCESS_HERE 
            );
    }
}



void
point_set::load( const std::string& base_file_name )
{
    path header_file    = base_file_name + file_suffix_helper::get_header_suffix();
    path data_file      = base_file_name + file_suffix_helper::get_vertex_suffix();
    
    exists( header_file );
    _header.read_from_file( base_file_name );
    
    _point_size_in_bytes = _header.get_vertex_structure().compute_size_in_bytes();
    
    exists( data_file );

    _mapped_file.open( data_file.string() );
    if ( ! _mapped_file.is_open() )
    {
        throw exception( 
            std::string( "opening file " ) + data_file.string() + " failed.",
            SPROCESS_HERE 
            );
    } 
}



const point_set_header&
point_set::get_header() const
{
    return _header;
}


} // namespace stream_process

