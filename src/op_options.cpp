#include "op_options.h"

namespace stream_process
{

op_options::op_options( const std::string& op_name )
    : _op_options( op_name + " operator options" )
    , empty( false )
{}



op_options::op_options()
    : _op_options()
    , empty( true )
{}


void 
op_options::add_op_config( const std::vector< std::string >& op_cfg,
    boost::program_options::variables_map& vmap )
{
    // ugly hack to allow old-style config files
    size_t s = op_cfg.size();
    
    char* argv[ s ];
    for( size_t i = 0; i < s; ++i )
    {
        argv[ i ] = const_cast< char* >( op_cfg[ i ].c_str() );
    }
    
    boost::program_options::store(
      boost::program_options::command_line_parser( s, argv )
      .options( _op_options )
      .positional( _pos_op_options )
      .run(),
      vmap
      );
}


} // namespace stream_process
