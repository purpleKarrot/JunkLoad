#ifndef __STREAM_PROCESS__NORMAL_TEST_OP__HPP__
#define __STREAM_PROCESS__NORMAL_TEST_OP__HPP__

#include "stream_op.h"
#include "rt_struct_member.h"

#include <deque>

namespace stream_process
{
    
class normal_test_op 
    : public stream_op 
{
public:
    normal_test_op();
    ~normal_test_op();
    
    virtual void pull_push();
    virtual stream_point* front();
    virtual void pop_front();
    
    virtual size_t size() const;
    
    virtual void setup_stage_0();
    virtual void setup_stage_1();
    virtual void setup_stage_2();
    
    virtual void clear_stage();
    
protected:
    virtual void test( stream_point* point );

    std::deque<stream_point*> _fifo;
    
    rt_struct_member< vec3f >       _normal;
    rt_struct_member< vec3f >       _normal_copy;
    rt_struct_member< vec3f >       _position;
    rt_struct_member< vec3f >       _position_copy;
    
    vec3f   _normal_max;
    vec3f   _normal_min;
    vec3f   _position_max;
    vec3f   _position_min;
};


} // namespace stream_process

#endif
