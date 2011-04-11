#ifndef __STREAM_PROCESS__DATA_SET_HEADER_IO__HPP__
#define __STREAM_PROCESS__DATA_SET_HEADER_IO__HPP__

#include "data_set_header.hpp"

#include <string>
#include <fstream>
#include <queue>

namespace stream_process
{

struct data_set_header_io
{
	void load(const std::string& filename, header& h);

	void save(const std::string& filename, const header& h);
};

} // namespace stream_process

#endif
