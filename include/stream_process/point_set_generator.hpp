#ifndef __STREAM_PROCESS__POINT_SET_GENERATOR__HPP__
#define __STREAM_PROCESS__POINT_SET_GENERATOR__HPP__

#include <stream_process/data_types.hpp>

#include <string>

namespace stream_process
{

class point_set_generator
{
public:
    void generate_unit_sphere( const std::string& filename_base, size_t points );


}; // class point_set_generator

} // namespace stream_process

#endif

