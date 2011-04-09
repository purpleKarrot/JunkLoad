#ifndef __STREAM_PROCESS__DATA_SET_HEADER_IO__HPP__
#define __STREAM_PROCESS__DATA_SET_HEADER_IO__HPP__

#include "data_set_header.hpp"

#include <string>
#include <fstream>
#include <queue>

namespace stream_process
{

class data_set_header_io
{
public:
	void read_from_file(const std::string& filename_base,
			data_set_header& hdr_);

	void write_to_file(const std::string& filename_base,
			const data_set_header& hdr_);

protected:
	void _open_file(const std::string& filename, std::fstream::openmode mode_);
	void _parse_header(data_set_header& _working_copy);
	bool _parse_line(std::deque<std::string>& tokens,
			data_set_header& _working_copy);

	std::string _filename;
	std::fstream _fstream;

	size_t _input_offset;
	size_t _input_face_offset;
};

} // namespace stream_process

#endif
