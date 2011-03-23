#ifndef __VMML__OP_OPTIONS__H__
#define __VMML__OP_OPTIONS__H__

#include <boost/program_options.hpp>

namespace stream_process
{

struct op_options
{
    op_options();
    op_options( const std::string& op_name );

    void add_op_config(  const std::vector< std::string >& op_cfg,
        boost::program_options::variables_map& vmap );
    
    boost::program_options::options_description _op_options;
    boost::program_options::positional_options_description _pos_op_options;
    bool empty;

}; // struct op_options

} // namespace stream_process

#endif
