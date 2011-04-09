#ifndef __STREAM_PROCESS__FILE_SUFFIX_HELPER__HPP__
#define __STREAM_PROCESS__FILE_SUFFIX_HELPER__HPP__

#include <string>

namespace stream_process
{

struct file_suffix_helper
{
	static std::string get_header_suffix()
	{
		return ".header";
	}

	static std::string get_vertex_suffix()
	{
		return ".vertices";
	}

	static std::string get_face_suffix()
	{
		return ".faces";
	}

	static std::string get_suffix(const std::string& element)
	{
		if (element == "vertex")
			return get_vertex_suffix();

		if (element == "face")
			return get_face_suffix();

		if (element == "header")
			return get_header_suffix();

		return "." + element;
	}
};

} // namespace stream_process

#endif
