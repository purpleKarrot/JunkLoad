#include <stream_process/options_map.hpp>

#include <stream_process/options_parser.hpp>

namespace stream_process
{

options_map::options_map( int argc, const char* argv[] )
{
	parse( argc, argv );
}



void
options_map::parse( int argc, const char* argv[] )
{
	options_parser parser( *this, argc, argv );
}


size_t
options_map::parse( const opt_deque& modes, int argc, const char* argv[] )
{
	options_parser parser( *this, argc, argv );
	return parser.setup_mode( modes );
}

} // namespace stream_process

