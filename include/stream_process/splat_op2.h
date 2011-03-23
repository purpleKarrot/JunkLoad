#ifndef _Splat_Op2_H_
#define _Splat_Op2_H_

#include "splat_op.h"

namespace stream_process
{
/**
*
* @brief splat operator
*
* see also splat_op
* 
* splat_op2 reads the following inputs from the stream:
*   "curv"              - curvature matrix                      - mat4d
*   "scales"            - scales vector                         - vec3f
*
* splat_op2 writes the following outputs into the stream:
*   "axis"              - major principal semiaxis orientation  - vec3f
*   "length"            - major principal semiaxis length       - float
*   "ratio"             - semiaxis aspect ratio                 - float
*
* @author renato pajarola
* @author jonas boesch
*
*/
class splat_op2 
    : public splat_op
{		
public:
    splat_op2();

protected:
    virtual void compute_splat( stream_point* point );

    rt_struct_member< vec3f > _scales;
    rt_struct_member< mat4d > _curvature;

}; // class splat_op2


} // namespace stream_process

#endif
