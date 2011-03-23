#include "file_loader.hpp"
#include "exception.hpp"

#include "point_info.hpp"
#include "mapped_point_data.hpp"

#include "Log.h"

namespace stream_process
{

file_loader::file_loader()
    : _mapped_point_data( 0 )
    , _output_filename( "tmp" )
{}



void
file_loader::_open_file( const std::string& filename, 
    std::ios_base::openmode openmode_ )
{
    if ( _file.is_open() )
        _file.close();

    _file.open( filename.c_str(), openmode_ );

    if ( ! _file.is_open() )
    {
        std::string msg = "opening file ";
        msg += filename;
        msg += " failed.";
        throw exception( msg.c_str(), SPROCESS_HERE );
    }

}



void
file_loader::_find_min_max()
{
    _mapped_point_data->recompute_aabb();
}





} // namespace stream_process

