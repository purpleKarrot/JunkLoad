#ifndef __VMML__ACTIVE_SET_FIFO__H__
#define __VMML__ACTIVE_SET_FIFO__H__

#include <deque>
#include "stream_point.h"

namespace stream_process
{

	typedef std::deque< stream_point* > active_set_fifo;

} // namespace stream_process

#endif

