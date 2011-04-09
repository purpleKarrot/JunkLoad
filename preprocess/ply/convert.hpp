#ifndef __STREAM_PROCESS__PLY_CONVERTER_HPP__
#define __STREAM_PROCESS__PLY_CONVERTER_HPP__

#include <string>

namespace stream_process
{

void ply_convert(const char* source_file,
				const std::string& target_file);

} // namespace stream_process

#endif
