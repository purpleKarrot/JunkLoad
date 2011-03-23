#ifndef __VMML__ACTIVE_SET__OPERATOR__H__
#define __VMML__ACTIVE_SET__OPERATOR__H__

#include "stream_point.h"
#include "rt_struct.h"
#include "rt_struct_factory.h"
#include "active_set_fifo.h"
#include "chain_config.h"
#include "op_common.hpp"

namespace stream_process
{

class active_set_op : public op_common
{
public:
    virtual ~active_set_op();
    
    virtual void insert( stream_point* point ) = 0;
    virtual void remove( stream_point* point ) = 0;

    virtual void prepare_processing();
    virtual void prepare_shutdown();
    
}; // class active_set_op


} // namespace stream_process

#endif
