#ifndef _EigenvalueSolver_H_
#define _EigenvalueSolver_H_

#include "StreamProcessing.h"

//FIXME -> VMMLib

namespace stream_process
{

void solveEigenvalues2D( mat3f& Axis, float &ell, float &ratio,
                        const mat4d& R, const mat4d& cov);

} // namespace stream_process

#endif
