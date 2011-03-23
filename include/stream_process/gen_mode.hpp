#ifndef __STREAM_PROCESS__GEN__HPP__
#define __STREAM_PROCESS__GEN__HPP__

#include <stream_process/options_base.hpp>

namespace stream_process
{

class gen_mode
{
public:
    gen_mode( const options_map& omap );

protected:
    void print_usage();
    
    const options_map&  _options;

}; // class gen

} // namespace stream_process

#endif

