#ifndef __STREAM_PROCESS__CHAIN_OPERATORS__HPP__
#define __STREAM_PROCESS__CHAIN_OPERATORS__HPP__

#include "stream_point.h"

#include <vector>
#include <map>
#include <string>
#include <list>
#include <functional>
#include "op_container.hpp"

namespace stream_process
{

class active_set_op;
class chain_operators : public op_container< active_set_op >
{
public:
    chain_operators();

    // chain_op forwards
    void insert( stream_point* point );
    void remove( stream_point* point );

    size_t get_active_set_size();

protected:
    size_t _points_in_active_set;

}; // class chain_operators

} // namespace stream_process

#endif

