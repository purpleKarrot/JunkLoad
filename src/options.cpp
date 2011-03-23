#include "options.h"

#include <iostream>
#include <vector>
#include <string>

#include "string_utils.h"
#include "text_file.h"

#include "exception.hpp"
#include "Log.h"


namespace stream_process
{

options::options()
    : _generic_options( "Generic options" )
{
    _generic_options.add_options()
        ("help", "Show help message")
        ("version", "Show version information")
        ;
}


options::options( bool no_options_setup )
    : _generic_options( "Generic options" )
{
    if ( no_options_setup )
    {}
    else
    {
        _generic_options.add_options()
            ("help", "Show help message")
            ("version", "Show version information")
            ;
    }
}



void
options::setup( int argc, const char* argv[] )
{
    _argc_store = argc;
    _argv_store = const_cast< char** >( argv );
}



void
options::post_setup()
{
    _all_options.add( _generic_options );  
    
    boost_po::store(
        boost_po::command_line_parser( _argc_store, _argv_store )
        .options( _all_options )
        .positional( _positional_desc )
        .run(),
        _options_map 
        );
    boost_po::notify( _options_map );       
}




void 
options::print_options() const
{
    var_map::const_iterator it =    _options_map.begin();
    var_map::const_iterator ite =   _options_map.end();
    for( ; it != ite; ++it )
    {
        std::cout << "option " << (*it).first << std::endl;
    }
/*
    if ( _options_map.count("op-chain-cfg") )
    {
        std::cout << "opchain config is: " 
            << _options_map["op-chain-cfg"].as< std::string >() << "\n";
    }
    
    if ( _options_map.count("ops") )
    {
        const std::vector<  std::string >& ops 
            = _options_map["ops"].as< std::vector< std::string > >();
        std::vector< std::string >::const_iterator it = ops.begin();
        std::vector< std::string >::const_iterator itend = ops.end();
        
        std::cout << "ops: ";
        for ( ; it != itend; ++it )
        { 
            std::cout << *it << ", ";
        }
        std::cout << std::endl;
    }
*/
}


    
bool
options::check() const
{
    return false;
}


/*
void
options::load_op_chain_config()
{
    if ( _options_map.count( "op-chain-cfg" ) == 0 )
        return;

	const std::string filename = 
		_options_map["op-chain-cfg"].as< std::string >();

	text_file text_file_;
	const std::vector< std::string >& lines = text_file_.read( filename );
	
	std::vector< std::string >::const_iterator it = lines.begin();
	std::vector< std::string >::const_iterator itend = lines.end();
	for ( ; it != itend; ++it )
	{
		std::vector< std::string > op_config;
		const std::string& line = *it;
		std::vector< std::string > tokens; 
		string_utils::split_string( line, tokens, '#');
		if ( tokens.empty() || tokens[0].empty() ) 
		{}
		else
		{
			std::string line_ = tokens[0];
			tokens.clear();
			string_utils::split_string( line_, tokens );
			if ( ! tokens.empty() )
			{
				_op_cfgs_from_file.push_back( tokens );
                //std::cout << "added " << line_ << " to op_cfgs_from_file" << std::endl;
			}
		}
	}
	LOGDEBUG << "loaded operator chain config from file '" << filename << "'." 
		<< std::endl;
}
*/


void 
options::add_op_desc( const std::string& op_name, 
    boost_po::options_description* desc )
{
    _op_descs[ op_name ] = desc;
    _all_options.add( *desc );
}



void
options::print_usage() const
{
    std::cout << "Usage: \n" 
        << "    sptool -c <chain_config.cfg> [options] \n"
        << "    sptool <op-0> <op-1> <...> <op-n> [options]\n" 
        << std::endl;
    std::cout << _generic_options << "\n";
    
    std::map< std::string, boost_po::options_description* >:: const_iterator it
        = _op_descs.begin();
    std::map< std::string, boost_po::options_description* >:: const_iterator itend
        = _op_descs.end();
    
    std::cout << "Operator-specific options:\n" << std::endl;
    for( ; it != itend; ++it )
    {
        std::cout << *(*it).second << std::endl;
    }    
}


/*
void 
options::get_operator_chain( std::vector< std::string >& op_chain )
{
    if ( _options_map.count("ops") )
    {
        const std::vector<  std::string >& ops 
            = _options_map["ops"].as< std::vector< std::string > >();

        op_chain = ops;
    }    
}
*/


bool
options::has_key( const std::string& key ) const
{
    return has_option( key );
}



bool
options::has_option( const std::string& key ) const
{
    var_map::const_iterator it = _options_map.find( key );
    return it != _options_map.end();
}


/*
const std::vector< std::vector< std::string > >& 
options::get_op_configs_from_cfg_file() const
{
    return _op_cfgs_from_file;
}
*/


void
options::print_all_keys() const
{
    var_map::const_iterator it      = _options_map.begin();
    var_map::const_iterator it_end  = _options_map.end();
    
    for( ; it != it_end; ++it )
    {
        std::cout << "key: " << (*it).first << std::endl;
    }
}



void
options::debug_determine_type( const std::string& key )
{
    var_map::const_iterator it = _options_map.find( key );
    if ( it != _options_map.end() )
    {
        std::cout << "trying to determine type for key " << key << std::endl;
        try
        {
            (*it).second.as< std::string >();
            std::cout << key << " is of type std::string." << std::endl;
        }
        catch( boost::bad_any_cast& e )
        {}
        try
        {
            (*it).second.as< int >();
            std::cout << key << " is of type int." << std::endl;
        }
        catch( boost::bad_any_cast& e )
        {}
        try
        {
            (*it).second.as< unsigned int >();
            std::cout << key << " is of type unsigned int." << std::endl;
        }
        catch( boost::bad_any_cast& e )
        {}
        try
        {
            (*it).second.as< char* >();
            std::cout << key << " is of type char*." << std::endl;
        }
        catch( boost::bad_any_cast& e )
        {}
        try
        {
            (*it).second.as< float >();
            std::cout << key << " is of type float." << std::endl;
        }
        catch( boost::bad_any_cast& e )
        {}
        try
        {
            (*it).second.as< double >();
            std::cout << key << " is of type double." << std::endl;
        }
        catch( boost::bad_any_cast& e )
        {}
    
    
    }
}



const std::string&
options::get_value_string( const std::string& key ) const
{
    var_map::const_iterator it = _options_map.find( key );
    if( it != _options_map.end() )
    {
        return (*it).second.as< std::string >();
    }
    throw exception( std::string("no option value stored for key ") + key, 
        SPROCESS_HERE );

}




}//namespace stream_process

