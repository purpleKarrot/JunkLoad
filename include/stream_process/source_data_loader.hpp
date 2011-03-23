#ifndef __STREAM_PROCESS__SOURCE_DATA_LOADER__HPP__
#define __STREAM_PROCESS__SOURCE_DATA_LOADER__HPP__

#include <string>
#include <map>

namespace stream_process
{

class file_loader;
class mapped_point_data;
class source_data_loader
{
public:
    source_data_loader();

    void load( const std::string& filename, 
        const std::string& target_filename, 
        mapped_point_data& mapped_point_data_  );

protected:
    
}; // class source_data_loader

} // namespace stream_process

#endif

