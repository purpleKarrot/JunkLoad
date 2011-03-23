#ifndef _Test_Op_H_
#define _Test_Op_H_

#include "stream_op.h"
#include "neighbor.h"
#include "rt_struct_member.h"

#include <deque>

namespace stream_process
{
    
class test_op 
    : public stream_op 
{
public:
    test_op();
    ~test_op();
    
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
    
    size_t      _diff_nb_count;
    size_t      _smaller_nbs;
    
    rt_struct_member< int >         _test;
    rt_struct_member< vec3f >       _position;
    rt_struct_member< uint32_t >    _point_index;
    rt_struct_member< uint32_t >    _nb_count;
    rt_struct_member< neighbor >    _neighbors;
    rt_struct_member< uint32_t >    _min_ref_index;
    rt_struct_member< uint32_t >    _max_ref_index;
    rt_struct_member< uint32_t >    _nb_count_kd;
    rt_struct_member< stream_point* > _kd_neighbors;
    rt_struct_member< float >       _distances;
    //rt_struct_member< uint32_t >    _min_ref_index_kd;
    //rt_struct_member< uint32_t >    _max_ref_index_kd;
    //#endif
};


} // namespace stream_process

#endif
