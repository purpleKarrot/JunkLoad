#include "EigenvalueSolver.h"

#include <cstdlib>
#include <cstdio>
#include <cmath>


namespace stream_process
{

void multVector( float* u, const mat4d& m, const vec3f& v)
{
  int i, j;
  float tmp;

  for (j = 0; j < 3; j++) {
    tmp = 0.0;
    for (i = 0; i < 3; i++)
      tmp += v[i] * m(j,i);
    tmp += m(j,3);
    u[j] = tmp;
  }
}


void solveEigenvalues2D(
    mat3f& Axis,
    float& ell,
    float& ratio,
    const mat4d& R,
    const mat4d& cov
    )
{
  float trace, det, l1, l2, f;
  vec3f v1, v2, v3;
  mat4d RT;

  // solve eigenvalues for upper-left 2x2 covariance submatrix
  trace = cov(0,0) + cov(1,1);
  det   = cov(0,0) * cov(1,1) - cov(0,1) * cov(1,0);
  f = trace*trace - 4.0*det;
  
  if ( f < -0.001 ) // allow for small numeric error
  {
    l1 = l2 = 0.0;
  }
  else if ( f <= 0.0 )
  {
    l1 = l2 = 0.5 * trace;
  }
  else
  {
    f = sqrtf( f );
    l1 = 0.5 * (trace + f);
    l2 = 0.5 * (trace - f);
  }

  // get eigenvectors
  if ( l1 == l2 )
  {
    v1[0] = 1.0;
    v1[1] = 0.0;
  } else {
    // get first eigenvector, major ellipse axis
    if (l1 > l2)
    {
      if (cov(0,0) - l1 != 0.0)
      {
        v1[0] = -cov(0,1) / (cov(0,0) - l1);
        v1[1] = 1.0;
      }
      else if (cov(1,1) - l1 != 0.0)
      {
        v1[0] = 1.0;
        v1[1] = -cov(1,0) / (cov(1,1) - l1);
      }
      else
      {
        fprintf(stderr, "Error: no eigenvector computable\n");
      }
    }
    else
    {
      if (cov(1,1) - l2 != 0.0)
      {
        v1[0] = 1.0;
        v1[1] = -cov(1,0) / (cov(1,1) - l2);
      }
      else if (cov(0,0) - l2 != 0.0)
      {
        v1[0] = -cov(0,1) / (cov(0,0) - l1);
        v1[1] = 1.0;
      }
      else
      {
        fprintf(stderr, "Error: no eigenvector computable\n");
      }
    }
  }
  v1[2] = 0.0;
  v1.normalize();

  // minor axis is perpendicular in the x,y-plane
  v2.set( -v1( 1 ), v1( 0 ), 0.0f );
  v3.set( 0.0f, 0.0f, 1.0f );
  
  // rotate vectors back into WCS
    R.transposeTo( RT );

    vec3f row;
    Axis.getRow( 0, row );
    multVector( row.array, RT,v1 );
    Axis.setRow( 0, row ); 

    Axis.getRow( 1, row );
    multVector( row.array, RT, v2 );
    Axis.setRow( 1, row );

    Axis.getRow( 2, row );
    multVector( row.array, RT, v3 );
    Axis.setRow( 2, row );

  // set splat aspect ratio
  l1 = fabs(l1);
  l2 = fabs(l2);

  if ( l1 == 0.0 || l2 == 0.0 )
  {
    ell = 1.0;
    ratio = 1.0;
  }
  else if ( l1 >= l2 )
  {
    ell = l1;
    ratio = l2 / l1;
  }
  else
  {
    ell = l2;
    ratio = l1 / l2;
  }
  if ( ratio < 0 )
  {
    // maybe then we should have to flip axis as well
    ratio = -ratio;
  }

  // bound aspect ratio
  if (ratio < 0.5)
    ratio = 0.5;
}

} // namespace stream_process

