#ifndef _vmml_Outlier_Op_H_
#define _vmml_Outlier_Op_H_

#include "stream_op.h"
#include "rt_struct_member.h"
#include "neighbor.h"
#include <deque>

namespace stream_process
{

class outlier_op 
    : public stream_op 
{
public:
    outlier_op();
    ~outlier_op(); 

    virtual void pull_push();
    virtual stream_point* front();
    virtual void pop_front();
    
    virtual void clear_stage();

    virtual size_t size() const;

    virtual void setup_stage_0();   
    virtual void setup_stage_1();
    virtual void setup_stage_2();

protected:
    void _compute_outlier_distances( stream_point* point );

    std::deque< stream_point* > _in_buffer;
    std::deque< stream_point* > _out_buffer;

    rt_struct_member< vec3f >       _position;
    rt_struct_member< vec3f >       _normal;
    rt_struct_member< neighbor >    _neighbors;
    rt_struct_member< uint32_t >    _nb_count;
    rt_struct_member< uint32_t >    _max_ref_index;
    rt_struct_member< vec3f >       _scales;
    
    size_t  _points_in_op;

    std::vector< float >            _outlier_distance_to_neighbors;
    std::vector< float >::iterator  _out_dist_it;
    
    float   _min_out_dist;
    float   _max_out_dist;
    double  _avg_out_dist;

}; // class outlier_op


} // namespace stream_process

#endif
