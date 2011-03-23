#ifndef __STREAM_PROCESS__READ_OP_BARRIER__HPP__
#define __STREAM_PROCESS__READ_OP_BARRIER__HPP__

#include "preprocess_shared_data.hpp"

#include <string>

namespace stream_process
{

class read_op_barrier
{
public:
    read_op_barrier( uint32_t& index_ );
    virtual ~read_op_barrier();

    virtual void check() = 0;

protected:
    uint32_t&     _index;
    uint32_t      _min_unsorted_index;
}; // class read_op_barrier


class empty_read_op_barrier : public read_op_barrier
{
public:
    empty_read_op_barrier( uint32_t& index_ );
        
    virtual void check() {};
};


class preprocess_aware_read_op_barrier : public read_op_barrier
{
public:
    preprocess_aware_read_op_barrier( preprocess_shared_data& 
        preprocess_shared_data_, uint32_t& index );
    
    virtual void check();

protected:
    preprocess_shared_data&     _preprocess_shared_data;
    size_t                      _wait_duration;
};

} // namespace stream_process

#endif

