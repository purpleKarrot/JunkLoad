#ifndef __STREAM_PROCESS__FILE_LOADER__HPP__
#define __STREAM_PROCESS__FILE_LOADER__HPP__

#include <string>
#include <fstream>

namespace stream_process
{

class mapped_point_data;
class file_loader
{
public:
    file_loader();
    virtual ~file_loader() {};
    virtual void load(
        const std::string& input_filename, 
        const std::string& output_filename, 
        mapped_point_data& mapped_point_data_
         ) = 0;
protected:
    void _open_file( const std::string& filename, 
        std::ios_base::openmode openmode_ 
            = std::ios_base::in | std::ios_base::out );
            
    virtual void _find_min_max();
    
    std::fstream _file;
    mapped_point_data*          _mapped_point_data;
    
    std::string                 _output_filename;
    
}; // class file_loader

} // namespace stream_process

#endif

