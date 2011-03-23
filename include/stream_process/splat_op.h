#ifndef _Splat_Op_H_
#define _Splat_Op_H_

#include "stream_op.h"
#include "rt_struct_member.h"
#include "neighbor.h"
#include <deque>

/**
*
* @brief splat operator
*
* splat_op reads the following inputs from the stream:
*   "position"          - point position                        - vec3f          
*   "normal"            - normal vector                         - vec3f 
*   "neighbors"         - distance/ptr pair of knn              - neighbor[k]
*   "tmp_covar"         - covariance matrix                     - mat4d
*
* splat_op writes the following outputs into the stream:
*   "axis"              - major principal semiaxis orientation  - vec3f
*   "length"            - major principal semiaxis length       - float
*   "ratio"             - semiaxis aspect ratio                 - float
*
* @author renato pajarola
* @author jonas boesch
*
*/

#define SPROCESS_SPLAT_OP_PROJECT_ELLIPSE_AXIS

namespace stream_process
{

class splat_op
    : public stream_op 
{
public:
    splat_op();

    virtual void pull_push();
    virtual stream_point* front();
    virtual void pop_front();

    virtual size_t size() const;
    
    virtual void setup_stage_0();
    virtual void setup_stage_1();
    virtual void setup_stage_2();
    
protected:
    virtual void compute_splat( stream_point* point );
    // DEBUG?
    #ifdef SPROCESS_SPLAT_OP_PROJECT_ELLIPSE_AXIS
    virtual void _compute_proj_axis( stream_point* point );
    #endif

    std::deque< stream_point* > _fifo;
    
    rt_struct_member< vec3f >           _axis;
    rt_struct_member< float >           _length;
    rt_struct_member< float >           _ratio;
    rt_struct_member< vec3f >           _normal;
    rt_struct_member< mat4d >           _tmp_covar;
    rt_struct_member< neighbor >        _neighbors;
    rt_struct_member< uint32_t >        _nb_count;
    rt_struct_member< vec3f >           _position;
};


} // namespace stream_process

#endif
