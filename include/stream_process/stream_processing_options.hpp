#ifndef __VMML__STREAM_PROCESSING_OPTIONS__H__
#define __VMML__STREAM_PROCESSING_OPTIONS__H__

#include "exception.hpp"

#include "options.h"

#include <vector>
#include <string>


namespace stream_process
{
class stream_processing_options : public options
{
public:
    stream_processing_options();
    virtual ~stream_processing_options() {};

    virtual bool check() const;

    void get_operator_chain( std::vector< std::string >& op_chain );
    
    void load_op_chain_config();   
    const std::vector< std::vector< std::string > >& 
        get_op_configs_from_cfg_file() const;
    
protected:
    std::vector< std::vector< std::string > > _op_cfgs_from_file;
    
}; // class stream_processing_options


} // namespace stream_process

#endif
