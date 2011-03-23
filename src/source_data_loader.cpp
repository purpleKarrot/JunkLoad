#include "source_data_loader.hpp"

#include "mapped_point_data.hpp"

#include "file_loader.hpp"
#include "ply_loader.hpp"
#include "obj_loader.hpp"
#include "ascii_loader.hpp"

namespace stream_process
{

source_data_loader::source_data_loader()
{}



void
source_data_loader::load( const std::string& filename,
    const std::string& target_filename, 
    mapped_point_data& mapped_point_data_  )
{
    std::string suffix;
    size_t dot_pos = filename.find_last_of( "." );
    suffix = filename.substr( dot_pos + 1, std::string::npos );
    if ( suffix == "ply" )
    {
        ply_loader ply_loader_;
        ply_loader_.load( filename, target_filename, mapped_point_data_ );
    }
    else if ( suffix == "obj" )
    {
        obj_loader obj_loader_;
        obj_loader_.load( filename, target_filename, mapped_point_data_ );
    }
    else if ( suffix == "txt" )
    {
        ascii_loader ascii_loader_;
        ascii_loader_.load( filename, target_filename, mapped_point_data_ );
    }
    else 
    {
        std::string msg( "loading source data file ");
        msg += filename;
        msg += " failed - unsupported format.";
        throw exception( msg.c_str(), SPROCESS_HERE );
    }
}

} // namespace stream_process

