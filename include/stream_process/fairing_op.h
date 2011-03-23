#ifndef _Fair_Op_H_
#define _Fair_Op_H_

#include "stream_op.h"
#include "gaussian.h"
#include "rt_struct_member.h"
#include "neighbor.h"

// FIXME

namespace stream_process
{

class fairing_op 
    : public stream_op 
{

public:
    fairing_op();
    ~fairing_op();

    virtual void pull_push();
    virtual stream_point* front();
    virtual void pop_front();
    virtual void clear_stage();

    virtual size_t smallest_element();
    virtual size_t smallest_reference();

    virtual size_t size() const;

    virtual void setup_stage_0();
    virtual void setup_stage_1();
    virtual void setup_stage_2();
  
protected:
    float _diagonal;
    Gaussian< float >* F;
    Gaussian< float >* G;
    IndexQueue ZQ;
    std::deque< stream_point* > _fifo;
    std::deque< stream_point* > _fifo2;
    
    bool _splat;

    void fair_splat( stream_point* point );

    rt_struct_member< uint32_t >        _point_index;
    rt_struct_member< vec3f >           _position;
    //rt_struct_member< vec3f >           _faired_position;
    rt_struct_member< vec3f >           _normal;
    rt_struct_member< neighbor >        _neighbors;
    rt_struct_member< uint32_t >        _nb_count;
    rt_struct_member< uint32_t >        _min_ref_index;
    rt_struct_member< uint32_t >        _max_ref_index;
    rt_struct_member< float >           _length;
    rt_struct_member< float >           _ratio;
    rt_struct_member< float >           _radius;

    rt_struct_member< float >           _area;
    rt_struct_member< vec3f >           _position_copy;
    rt_struct_member< vec3f >           _normal_copy;

}; // class fairing_op


} // namespace stream_process

#endif
