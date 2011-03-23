#include "stream_processing_options.hpp"

#include <iostream>
#include <vector>
#include <string>

#include "string_utils.h"
#include "text_file.h"

#include "exception.hpp"
#include "Log.h"

namespace stream_process
{

stream_processing_options::stream_processing_options()
    : options()
{
    _generic_options.add_options()
        ("op-chain-cfg,c", boost_po::value<std::string>(), 
            "Specify operator chain config.\n  If operators are specified directly on the command line, the op-chain-config file will be ignored." )
        ("ops,O", boost_po::value< std::vector< std::string > >()->composing(), 
            "Specify operators " )
        ("preprocess-data,p", boost_po::value< bool >()->default_value( false ),
            "Preprocess data.\n  Transforms and sorts the source data.")
        ("no-ot", boost_po::value< bool >()->default_value( false ),
            "No optimal transform.\n  Do not run the optimal transform step when preprocessing data.")
        ("multi-threaded,m", boost_po::value< bool >()->default_value( true ),
            "Multi-threaded mode.\n  Runs preprocess and main processing in parallel.")
        ("sorting-only,s", boost_po::value< bool >()->default_value( false ),
            "Sorting-only mode.\n  Only sorts and transforms the input data set.")
        ("disable-stats,d", "Disable statistics ")		
        ("log-level,l", boost_po::value< int >()->default_value( 3 ), 
            "Set logging level.\n  1 - Errors\n  2 - Warnings\n  3 - Info\n  4 - Verbose\n  5 - Debug")
        ;
    _positional_desc.add( "ops", -1 );
}



bool
stream_processing_options::check() const
{
    bool seems_ok = true;
    std::string error_msg = "";
    
    if ( _options_map.count( "ops" ) )
    {
    } // if no ops were specified on the command line, check op_cfg file
    else if ( _options_map.count( "op-chain-cfg" ) )
    {
    }
    else
    {
        if ( has_key( "sorting-only" ) && get_value< bool >( "sorting-only" ) )
        {}
        else
        {
            error_msg = "error: no operators specified.";
            seems_ok = false;
        }
    }

    if ( _options_map.count( "in-file" ) == 0 )
    {
        if ( seems_ok )
        {
            error_msg = "error: no input file specified.";
            seems_ok = false;
        }
    }

    if ( _options_map.count( "help" ) ) 
    {
        seems_ok = false;
    }

    if ( ! seems_ok )
    {
        print_usage();
        std::cout << std::endl << error_msg << std::endl;
    }
    return seems_ok;
}




void
stream_processing_options::load_op_chain_config()
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



void 
stream_processing_options::get_operator_chain( 
    std::vector< std::string >& op_chain )
{
    if ( _options_map.count("ops") )
    {
        const std::vector<  std::string >& ops 
            = _options_map["ops"].as< std::vector< std::string > >();

        op_chain = ops;
    }    
}



const std::vector< std::vector< std::string > >& 
stream_processing_options::get_op_configs_from_cfg_file() const
{
    return _op_cfgs_from_file;
}



}//namespace stream_process

