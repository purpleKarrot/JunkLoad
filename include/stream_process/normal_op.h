#ifndef _Normal_Op_H_
#define _Normal_Op_H_

#include "stream_op.h"

#include <deque>
#include "rt_struct_member.h"
#include "neighbor.h"

//#define STREAM_PROCESS_NORMAL_DEBUG

#ifdef STREAM_PROCESS_NORMAL_DEBUG
#include "normal_debug_helper.hpp"
#endif

namespace stream_process
{

/**
*
* @brief normal estimation operator
*
* normal_op reads the following inputs from the stream:
*   "position"  - point position                - vec3f          
*   "neighbors" - distance/ptr pair of knn      - neighbor[k]   
*
* normal_op writes the following outputs into the stream:
*   "normal"    - normal vector                 - vec3f 
*   "radius"    - bounding sphere radius        - float
*   "tmp_covar" - covariance matrix             - Matrix4f
*
*
* @author renato pajarola
* @author jonas boesch
*
*/

class normal_op 
    : public stream_op 
{
public:
    normal_op();

    virtual void pull_push();
    virtual stream_point* front();
    virtual void pop_front();

    virtual size_t size() const { return _fifo.size(); };

    virtual void setup_stage_0();
    virtual void setup_stage_1();
    virtual void setup_stage_2();

protected:
    static double **a, **v;
    
    std::deque< stream_point* > _fifo;

    void compute_normal( stream_point* point);
    bool get_normal( vec3f& normal, const mat4d& cov);
    bool get_normal_svd( vec3f& normal, const mat4d& cov);

    // normal_op creates these members
    rt_struct_member< mat4d >           _tmp_covar;
    rt_struct_member< vec3f >           _normal;
    rt_struct_member< float >           _radius;
    // normal_op uses these members
    rt_struct_member< vec3f >           _position;
    rt_struct_member< neighbor >        _neighbors;

    rt_struct_member< uint32_t >        _point_index;
    
    size_t  _max_neighbors;
    
    #ifdef STREAM_PROCESS_NORMAL_DEBUG
    normal_debug_helper                 _normal_debug_helper;
    #endif

}; // class normal_op


} // namespace stream_process

#endif
