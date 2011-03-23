
#ifndef __STREAM_PROCESS__OPTIONS__HPP__
#define __STREAM_PROCESS__OPTIONS__HPP__

#include <stream_process/option.hpp>
#include <stream_process/options_map.hpp>

#include <map>
#include <vector>
#include <string>

#include <iostream>

/**
*
* @brief this class deals with configurations/options such as cmdline params
* 
* the basic idea is to inherit from the class, and add all possible options
* to the options class in the constructor of the child class....
*
*/

namespace stream_process
{

class options_base : private std::map< std::string, option >
{
public:
    typedef std::map< std::string, option >     super;

    options_base();
    
    // set cmdline options, 
    // @param error error description if something went wrong...
    bool parse_cmdline( int argc, const char* argv[], std::string& error );

    option& get( const std::string& opt_name );
    const option& get( const std::string& opt_name ) const;
    bool has_option( const std::string& opt_name );

    void add_option( option& option_, const std::string& group_name = "general" );
 
    size_t get_number_of_positional_options() const;

    std::ostream& print_usage( std::ostream& os = std::cout ) const;
    std::ostream& print_values( std::ostream& os = std::cout ) const;

    friend std::ostream& operator<<( std::ostream& os, 
        const options_base& options_ ){ return options_.print_values( os ); };

    // setup option class
    // if positional is true, its registered as positional option at
    // the 'next' position

protected:
    using std::map< std::string, option >::iterator;
    using std::map< std::string, option >::const_iterator;

    const option& _get_option_object( const std::string& name_ ) const;

    void _register_short_name( iterator it, char short_name );

    void _mark_option_as_required( const std::string& name_ );

    bool _parse_boolean_from_string( const std::string& value_ ) const;
    
    iterator _find( const std::string& key_ );
    iterator _find_long_opt( char key_ );
    const_iterator _find( const std::string& key_ ) const;
    const_iterator _find_long_opt( char key_ ) const;

    std::map< char, iterator >      _short_names;
    std::vector< iterator >         _positional_options;
    
    std::vector< iterator >         _required_options;
    
    std::map< std::string, std::vector< iterator > >    _option_groups;

}; // class options_base


} // namespace trip

#endif

