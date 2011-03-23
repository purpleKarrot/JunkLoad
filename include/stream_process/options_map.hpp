#ifndef __STREAM_PROCESS__OPTIONS_MAP__HPP__
#define __STREAM_PROCESS__OPTIONS_MAP__HPP__

#include <stream_process/exception.hpp>

#include <string>
#include <map>
#include <queue>
#include <iostream>
#include <iterator>

namespace stream_process
{
// stores options - leading dashes are removed from the key.
// valid keys: -x / --xy  ( one-dash = one-letter-option, 
//   two dashes = at least two letters in the option name )


class options_map : private std::map< std::string, std::string >
{
public:
	typedef std::map< std::string, std::string >	opt_map;
	typedef std::deque< std::string >				opt_deque;
	
	using opt_map::operator[];
	using opt_map::iterator;
	using opt_map::const_iterator;
	using opt_map::find;
	using opt_map::begin;
	using opt_map::end;
	
	options_map() {}
	options_map( int argc, const char* argv[] );

	void parse( int argc, const char* argv[] );
	size_t parse( const opt_deque& modes, int argc, const char* argv[] );
	
	bool has_option( const std::string& name ) const
	{
		return opt_map::find( name ) != opt_map::end();
	}
    
    void add_option( const std::string& key_, const std::string& value_ )
    {
        opt_map::operator[]( key_ ) = value_;
    }
	
	bool empty() { return opt_map::empty() && _positionals.empty(); }
	
	const std::string& operator[]( size_t index ) const
	{
		if ( index < _positionals.size() )
			return _positionals[ index ];
		throw exception( "index out of bounds.", SPROCESS_HERE );
	}
	
	void push_back( const std::string& positional_ )
	{
		_positionals.push_back( positional_ );
	}
	
	void push_front( const std::string& positional_ )
	{
		_positionals.push_front( positional_ );
	}
	
	size_t get_number_of_positionals() const
	{
		return _positionals.size();
	}
	
	void swap( options_map& other_om )
	{
		opt_map& map_ = *this;
		map_.swap( other_om );
		_positionals.swap( other_om._positionals );
	}
    
    void print( std::ostream& os = std::cout )
    {
        if ( ! opt_map::empty() )
        {
            os << "options: \n";
            const_iterator it = begin(), it_end = end();
            for( ; it != it_end; ++it )
            {
                std::string key = it->first;
                while ( key.size() < 16 )
                {
                    key += " ";
                }
                os << key << " = " << it->second << "\n";
            }
        }
        if ( ! _positionals.empty() )
        {
            os << "positional options: \n";
            std::ostream_iterator< std::string >ositer( std::cout, "\n");
            std::copy(_positionals.begin(), _positionals.end(), ositer );
        }
        os << std::endl;
    }
	
protected:
	std::deque< std::string >	_positionals;

}; // class options_map

} // namespace stream_process

#endif

