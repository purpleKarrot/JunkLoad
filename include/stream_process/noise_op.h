#ifndef _Noise_Op_H_
#define _Noise_Op_H_

#include "stream_op.h"
#include "rt_struct_member.h"

#include <deque>

namespace stream_process
{

class noise_op 
    : public stream_op 
{
public:
    noise_op();
    ~noise_op();

    virtual void pull_push();
    virtual stream_point* front();
    virtual void pop_front();

    virtual size_t size() const;
    
    virtual void setup_stage_0();
    virtual void setup_stage_1();
    virtual void setup_stage_2();

protected:

    virtual void _addNoise();

    std::deque<stream_point*> _fifo;
    stream_point* _point;
    vec3f _max_change;
    
    float _scale;        

    rt_struct_member< vec3f > _position;
    
}; // class noise_op


} // namespace stream_process

#endif
