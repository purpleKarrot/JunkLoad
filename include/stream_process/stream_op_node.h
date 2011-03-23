#ifndef __VMML_STREAM_OP_NODE_H__
#define __VMML_STREAM_OP_NODE_H__

#include "StreamProcessing.h"
#include "rt_struct_factory.h"
#include "exception.hpp"

#include "rt_struct_user.h"

namespace stream_process
{
class stream_op_node : public rt_struct_user
{
public:
    stream_op_node();
    virtual ~stream_op_node();
    
protected:

}; /// class stream_op_node


} // namespace stream_process

#endif
