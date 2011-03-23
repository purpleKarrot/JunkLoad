
#include <stream_process/options_base.hpp>

#include <stream_process/exception.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include <iomanip>
#include <sstream>

namespace stream_process
{

bool
options_base::parse_cmdline( int argc, const char* argv[], std::string& error  )
{
    options_map omap( argc, argv );

    std::string key, value;
    
    error = "";
    
    size_t positionals = 0;

    options_map::iterator pp_it = omap.begin(), pp_it_end = omap.end();
    for( ; pp_it != pp_it_end; ++pp_it )
    {
        key     = pp_it->first;
        value   = pp_it->second;
        
        assert( ! key.empty() );
        
        super::iterator opt_it;

        if ( key.size() == 1 )
        {
            // short opt
            std::map< char, iterator >::iterator soit 
                = _short_names.find( key[0] );
            if ( soit == _short_names.end() )
            {
                error += "option '";
                error += key;
                error += "' not recognized.\n";
                continue;
            }
            else
                opt_it = soit->second;
        }
        else
        {
            opt_it = super::find( key );
            if ( opt_it == super::end() )
            {
                error += "option '";
                error += key;
                error += "' not recognized.\n";
                continue;
            }
        }
        assert( opt_it != super::end() );
        option& o = opt_it->second;
        try
        {
            o.set_string( value );
        }
        catch( exception& e )
        {
            error += "option '";
            error += key;
            error += "' could not be parsed successfully.\n";
        }
        continue;
    }
    
    const size_t in_pos_size = omap.get_number_of_positionals();
    const size_t out_pos_size = _positional_options.size();
    if ( in_pos_size > out_pos_size )
    {
        error += "too many positional options.\n";
    }
    else
    {
        for( size_t index = 0; index < in_pos_size; ++index )
        {
            option& o = _positional_options[ index ]->second;
            o.set_string( omap[ index ] );
        }
    }
    std::vector< iterator >::iterator
        required_opt_it     = _required_options.begin(),
        required_opt_it_end = _required_options.end();
    for( ; required_opt_it != required_opt_it_end; ++required_opt_it )
    {
        const option& option_ = (*required_opt_it)->second;
        if ( ! option_.was_set_by_user() )
        {
            error += "parameter '";
            error += option_.get_long_name();
            error += "' is required, but was not specified.\n";
        }
    }
    return error.empty();

#if 0
    const size_t arg_count = argc;
    size_t positionals = 0;
    for( size_t index = 1; index < arg_count; ++index )
    {
        iterator it;
        bool is_positional = false;
        
        std::string name_( argv[ index ] );
        if ( ! name_.empty() && name_[0] != '-' )
        {
            is_positional = true;
            if ( _positional_options.size() <= positionals )
            {
                std::stringstream ss;
                ss << "options: unrecognized option " << name_ << ". " << std::endl;
                error = ss.str();
                throw exception( error, SPROCESS_HERE );
            }
            it = _positional_options[ positionals++ ];
        }
        else
        {
            it = _find( name_ );
        }

        option& option_ = it->second;
        if ( is_positional )
        {
            option_.set_string( name_ );
        }
        else
        {
            if ( index == arg_count - 1 || argv[ index + 1 ][0] == '-' )
            {
                option_.set_bool( true );
            }
            else
                option_.set_string( argv[ ++index ] );
        }
    }
    
    std::vector< iterator >::iterator
        required_opt_it     = _required_options.begin(),
        required_opt_it_end = _required_options.end();
    for( ; required_opt_it != required_opt_it_end; ++required_opt_it )
    {
        const option& option_ = (*required_opt_it)->second;
        if ( ! option_.was_set_by_user() )
        {
            print_usage( std::cout );
            throw exception( std::string("parameter '") 
                + option_.get_long_name() + "' was not specified.", SPROCESS_HERE );
        }
    }
    

    return true;
#endif    
}



options_base::options_base()
{}


option&
options_base::get( const std::string& opt_name )
{
    iterator it = _find( opt_name );
    if ( it != end() )
        return it->second;
    throw exception(
        std::string( "attempt to read unknown option '") + opt_name + "'.",
        SPROCESS_HERE );
}


const option&
options_base::get( const std::string& opt_name ) const
{
    const_iterator it = _find( opt_name );
    if ( it != end() )
        return it->second;
    throw exception(
        std::string( "attempt to read unknown option '") + opt_name + "'.",
        SPROCESS_HERE );
}


bool
options_base::has_option( const std::string& opt_name )
{
    iterator it = _find( opt_name );
    return it != end(); 
}


void
options_base::add_option( option& option_, const std::string& group_name )
{
    bool overwrite = false;

    iterator it = _find( option_.get_long_name() );
    if ( it != end() )
    {
        #if 0
        std::cerr
            << "overwriting option " << option_.get_long_name()
            << "."
            << std::endl;
        #endif
        it->second = option_;
        overwrite = true;
    }
    else
    {
        std::pair< const std::string, option >
            newopt( option_.get_long_name(), option_ );
        std::pair< iterator, bool > new_it = insert( newopt );
        it = new_it.first;
    }
    
    if ( option_.has_short_name() )
    {
        _register_short_name( it, option_.get_short_name() );
    }
    
    if ( option_.is_positional() ) 
    {
        _positional_options.push_back( it );
        it->second.set_positional_number( _positional_options.size() );
    }
    
    // add to option group
    if ( ! overwrite )
        _option_groups[ group_name ].push_back( it );
}



void
options_base::_register_short_name( iterator it, char short_name )
{
    std::pair< char, iterator > new_it( short_name, it );
    _short_names.insert( new_it );
    it->second.set_short_name( short_name );
}



size_t
options_base::get_number_of_positional_options() const
{
    return _positional_options.size();
}



options_base::iterator
options_base::_find( const std::string& key_ )
{
    switch( key_.size() )
    {
        //case 1:
        //    return _find_long_opt( key_[0] );
        //    break;
        case 2:
            return _find_long_opt( key_[1] );
            break;
        case 0:
            return std::map< std::string, option >::end();
            break;
        default:
            break;
    }
    std::string query_key;
    if ( key_[0] == '-' && key_[1] == '-' )
    {
        query_key = key_.substr( 2, std::string::npos );
    }
    else
        query_key = key_;
    
    return std::map< std::string, option >::find( query_key ); 
}



options_base::iterator
options_base::_find_long_opt( char key_ )
{
    std::map< char, iterator >::iterator short_it =
        _short_names.find( key_ );

    if ( short_it == _short_names.end() )
        return std::map< std::string, option >::end();
    return short_it->second; 
}



options_base::const_iterator
options_base::_find( const std::string& key_ ) const
{
    switch( key_.size() )
    {
        case 1:
            return _find_long_opt( key_[0] );
            break;
        case 2:
            return _find_long_opt( key_[1] );
            break;
        case 0:
            return std::map< std::string, option >::end();
            break;
        default:
            break;
    }
    std::string query_key;
    if ( key_[0] == '-' && key_[1] == '-' )
    {
        query_key = key_.substr( 2, std::string::npos );
    }
    else
        query_key = key_;
    
    return std::map< std::string, option >::find( key_ ); 
}



options_base::const_iterator
options_base::_find_long_opt( char key_ ) const
{
    std::map< char, iterator >::const_iterator short_it =
        _short_names.find( key_ );

    if ( short_it == _short_names.end() )
        return std::map< std::string, option >::end();

    return short_it->second; 
}






std::ostream&
options_base::print_usage( std::ostream& os ) const
{
    std::map< std::string, std::vector< iterator > >::const_iterator
        git     = _option_groups.begin(),
        git_end = _option_groups.end();
    for( ; git != git_end; ++git )
    {
        os << git->first << " options:\n";
        std::vector< iterator >::const_iterator 
            oit     = git->second.begin(),
            oit_end = git->second.end();
        for( ; oit != oit_end; ++oit )
        {
            os << "  ";
            (*oit)->second.print_usage( os );
        }
        os << "\n";
    }

    return os;
}



std::ostream&
options_base::print_values( std::ostream& os ) const
{
    std::map< std::string, std::vector< iterator > >::const_iterator
        git     = _option_groups.begin(),
        git_end = _option_groups.end();
    for( ; git != git_end; ++git )
    {
        os << git->first << " options:\n";
        std::vector< iterator >::const_iterator 
            oit     = git->second.begin(),
            oit_end = git->second.end();
        for( ; oit != oit_end; ++oit )
        {
            os << "  " << (*oit)->second << std::endl;
        }
        os << "\n";
    }
    return os;
}



void
options_base::_mark_option_as_required( const std::string& name )
{
    iterator it = _find( name );
    _required_options.push_back( it );
}



bool
options_base::_parse_boolean_from_string( const std::string& value_ ) const
{
    if ( value_ == "true" 
         || value_ == "True" 
         || value_ == "TRUE" 
         || value_ == "1"
    )
    {
        return true;
    }
    else if ( value_ == "false" 
         || value_ == "False" 
         || value_ == "FALSE" 
         || value_ == "0"
         || value_.empty()
         )
    {
        return false;
    }

    throw exception( "failed to parse boolean value from string.",
        SPROCESS_HERE );

}


const option&
options_base::_get_option_object( const std::string& name_ ) const
{
    const_iterator it = _find( name_ );
    if ( it != end() )
    {
        return it->second;
    }
    else
    {
        throw exception( std::string( "unknown option '" ) + name_ + ".",
            SPROCESS_HERE );
    }
}



} // namespace trip

