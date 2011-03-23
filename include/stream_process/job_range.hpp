#ifndef __STREAM_PROCESS__OP_ALGORITHM_JOB__HPP__
#define __STREAM_PROCESS__OP_ALGORITHM_JOB__HPP__

#include "stream_point.h"
#include <queue>
#include <vector>

namespace stream_process
{
typedef std::vector< stream_point* > point_batch;

class op_algorithm;

struct job_range
{
    stream_point**          begin;
    stream_point**          end;
	point_batch::iterator	points_begin;
	point_batch::iterator	points_end;

}; // struct op_algorithm_job

} // namespace stream_process

#endif

