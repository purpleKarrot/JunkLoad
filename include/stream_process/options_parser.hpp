#ifndef __STREAM_PROCESS__OPTIONS_PARSER__HPP__
#define __STREAM_PROCESS__OPTIONS_PARSER__HPP__

#include <string>
#include <map>
#include <queue>
#include <vector>

namespace stream_process
{
class options_map;
class options_parser
{
public:
	options_parser( options_map& om_, int argc, const char* argv[] );
	
	size_t setup_mode( const std::deque< std::string >& modes );
	
protected:
	options_map&	_options_map;

}; // class options_parser

} // namespace stream_process

#endif

