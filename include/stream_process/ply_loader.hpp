#ifndef __STREAM_PROCESS__PLY_LOADER__HPP__
#define __STREAM_PROCESS__PLY_LOADER__HPP__

#include "file_loader.hpp"
#include <ply/ply.h>
#include "data_types.hpp"

#include <string>
#include <vector>

namespace stream_process
{

/**
*
*   @brief this class loads a ply file and converts it to a binary format that
*   the stream_processor can understand. the result is stored in a mem-mapped
*   file.
*
*   @author jonas boesch
*
*/

class ply_loader : public file_loader
{
public:
    ply_loader();
    virtual ~ply_loader() {};
    virtual void load(       
        const std::string& input_filename, 
        const std::string& output_filename, 
        mapped_point_data& mapped_point_data_
    );

protected:
    void _read_vertex_meta_data();
    void _read_vertex_data();
    
    void _debug_print_points();
    void _debug_check_point( char* point );
    
    PlyProperty _create_ply_property( const std::string& name, int internal_type, 
    int offset, int count_internal = 0, int count_offset = 0 );
    
    std::vector< std::string >  _other_properties;

	PlyFile*		_ply_file;
	PlyProperty**	_vertex_properties;
    int             _vertex_count;
	int				_vertex_property_count;
	int				_element_count;
    char**			_element_names;
    int				_file_type; // file type, either ascii or binary
    float           _version;

}; // class ply_loader

} // namespace stream_process

#endif

