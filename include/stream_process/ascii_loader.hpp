#ifndef __STREAM_PROCESS__ASCII_LOADER__HPP__
#define __STREAM_PROCESS__ASCII_LOADER__HPP__

#include "file_loader.hpp"
#include "data_types.hpp"

#include <string>
#include <vector>

/*
*   @brief reader for ascii files with the following format:
*   1 headerline with number of vertices
*   each line either: 
*   (round splats ) position, normal, color, radius
*   (elliptic splats)   position, color, normal, ellipse_axis, radius, axis_length, axis_ratio
*/

namespace stream_process
{
class ascii_to_point;

class ascii_loader : public file_loader
{
public:
    static const size_t   ROUND_SPLATS    = 1;
    static const size_t   ELLIPTIC_SPLATS = 2;
    
    ascii_loader();
    virtual ~ascii_loader();

    virtual void load(       
        const std::string& input_filename, 
        const std::string& output_filename, 
        mapped_point_data& mapped_point_data_
    );

protected:
    std::string _get_target_filename( const std::string& filename );

    bool    _extract_tokens_from_next_line();

    ascii_to_point* _ascii_to_point;
       
    size_t			_vertex_count;
    size_t          _format;

    char*           _current_point;

    std::string                 _line;
    std::vector< std::string >  _tokens;
    
}; // class ascii_loader

} // namespace stream_process

#endif

