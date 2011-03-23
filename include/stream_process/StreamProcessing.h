#ifndef _Stream_Processing_H_
#define _Stream_Processing_H_

// for uint32_t
#include <inttypes.h> 

/* Project Header for Stream Processing / stream_proc */

#define NEWSTREAMPROC 1

#include "CommonTypedefs.h"


namespace stream_process
{

} // namespace stream_process

// verbose debugging output
//#ifndef NDEBUG
#define SPROCESS_DEBUG_OUT 0
// turning this on is slow, but will allow the compiler to do type checking
// on the requested data elements. 
//#define SPROCESS_USE_DYNAMIC_ELEMENTS
#ifdef SPROCESS_USE_DYNAMIC_ELEMENTS
    #define SPDYN
#endif

//#endif


#endif
