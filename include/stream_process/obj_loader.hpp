#ifndef __STREAM_PROCESS__OBJ_LOADER__HPP__
#define __STREAM_PROCESS__OBJ_LOADER__HPP__

#include "file_loader.hpp"
#include "data_types.hpp"

#include <string>
#include <vector>

namespace stream_process
{

/**
*
*   @brief this class loads a obj file and converts it to a binary format that
*   the stream_processor can understand. the result is stored in a mem-mapped
*   file.
*
*/

class obj_loader : public file_loader
{
public:
    virtual void load(       
        const std::string& input_filename, 
        const std::string& output_filename, 
        mapped_point_data& mapped_point_data_
    );


protected:
	void _scan_file();

	size_t			_vertex_count;
	bool			_has_normals;
	bool			_has_texcoords;

}; // class obj_loader

} // namespace stream_process

#endif

