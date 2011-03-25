#ifndef __STREAM_PROCESS__FILE_SUFFIX_HELPER__HPP__
#define __STREAM_PROCESS__FILE_SUFFIX_HELPER__HPP__

#include <stream_process/exception.hpp>

#include <string>

namespace stream_process
{

class file_suffix_helper
{
public:
	static file_suffix_helper& get_singleton();
	static file_suffix_helper* get_singleton_ptr();

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
		else if (element == "face")
			return get_face_suffix();
		else if (element == "header")
			return get_header_suffix();
		else
			throw exception("unknown element.", SPROCESS_HERE);
	}

protected:
	static file_suffix_helper* _singleton_instance;
};

} // namespace stream_process

#endif
