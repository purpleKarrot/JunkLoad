#ifndef __VMML__CHAIN_CONFIG_H__
#define __VMML__CHAIN_CONFIG_H__

#include <string>
#include <map>
#include <list>
#include <vector>

#include <boost/program_options.hpp>

namespace stream_process
{

typedef boost::program_options::variables_map chain_config;

// FIXME either use options class or create a proper config class
// with bool has_option() and T& get_option( string key ) funcs...

} // namespace stream_process

#endif
