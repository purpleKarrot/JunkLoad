#include <stream_process/options_parser.hpp>

#include <stream_process/options_map.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

namespace stream_process
{


options_parser::options_parser( options_map& om_, int argc, const char* argv[] )
	: _options_map( om_ )
{
	using boost::algorithm::starts_with;
	using boost::algorithm::erase_head;
	using boost::algorithm::erase_head_copy;
	using boost::algorithm::contains;

    for( int index = 1; index < argc; ++index )
    {
		std::string argh = argv[ index ];
		std::string value;

		if ( index + 1 < argc )
			value = argv[ index + 1 ];
			
		size_t dashes = 0;

		dashes = 0;
		if ( argh.size() > 1 && argh[ 0 ] == '-' )
		{
			if ( argh.size() > 2 && argh[ 1 ] == '-' )
				dashes = 2;
			else
				dashes = 1;
		}
		
		// if dashes is != 0, it's a key, otherwise a positional option value
		if ( dashes )
		{
			erase_head( argh, dashes );
			if ( ! value.empty() && ! starts_with( value, "-" ) )
			{
				_options_map[ argh ] = value;
                ++index;
			}
			else
			{
				// boolean options (doesn't require a value for the key)
				_options_map[ argh ] = "true";
			}
		}
		else
		{
			_options_map.push_back( argh );
		}
	}
}



size_t
options_parser::setup_mode( const std::deque< std::string >& modes )
{
	size_t mode_index = 0;

	std::deque< std::string >::const_iterator
		it		= modes.begin(),
		it_end	= modes.end();
	for( size_t index = 0; it != it_end; ++it, ++index )
	{
		const std::string& mode = *it;
		options_map::const_iterator mode_it = _options_map.find( mode );
		
		if ( mode_it != _options_map.end() )
		{
			mode_index = index;
			if ( mode_it->second != "true" )
			{
				_options_map.push_front( mode_it->second );
			}
		}
	}
	return mode_index;
}



} // namespace stream_process

