#ifndef __STREAM_PROCESS__CURVATURE_OP__H__
#define __STREAM_PROCESS__CURVATURE_OP__H__

#include "stream_op.h"
#include "rt_struct_member.h"
#include "neighbor.h"

#include <deque>

namespace stream_process
{

/**
*
* @brief curvature estimation operator
*
* curvature_op reads the following inputs from the stream:
*   "position"          - point position                    - vec3f          
*   "normal"            - normal vector                     - vec3f 
*   "neighbors"         - distance/ptr pair of knn          - neighbor[k]
*   "min_ref_index"     - neighbour with smallest index     - uint32_t
*   "max_ref_index"     - neighbour with largest index      - uint32_t
*   "point_index"       - point index in stream             - uint32_t          
*
* curvature_op writes the following outputs into the stream:
*   "curv"              - curvature matrix                  - mat4d
*   "scales"            - scales vector                     - vec3f
*
* @author renato pajarola
* @author jonas boesch
*
*/

template< typename T > class rt_struct_member;
    
class curvature_op 
    : public stream_op 
{
public:
    curvature_op();

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
    static double **a, **v;

    IndexQueue ZQ;
    std::deque< stream_point* > _fifo1;
    std::deque< stream_point* > _fifo2;

    void compute_curvature( stream_point* point );
    void get_curvature_svd( stream_point* point, const mat4d& quadric );

    // curv op creates these members
    rt_struct_member< mat4d >           _curvature;
    rt_struct_member< vec3f >           _scales;
 
    // curv op uses these members
    rt_struct_member< uint32_t >        _min_ref_index;
    rt_struct_member< uint32_t >        _max_ref_index;
    rt_struct_member< uint32_t >        _point_index;
    rt_struct_member< neighbor >        _neighbors;
    rt_struct_member< vec3f >           _normal;
    rt_struct_member< vec3f >           _position;

    unsigned _max_neighbors;
    size_t _points_in_op;
};


} // namespace stream_process

#endif

