#ifndef __STREAM_PROCESS__INDEX_QUEUE__H__
#define __STREAM_PROCESS__INDEX_QUEUE_H__

#include <queue>
#include <vector>
#include <functional>

namespace stream_process
{

// smallest referenced index (first) of point (second)
typedef std::pair< uint32_t , uint32_t > index_reference; 
typedef std::priority_queue< 
            index_reference, 
            std::deque< index_reference >, 
            std::greater< index_reference > 
            > index_queue;

} // namespace stream_process

#endif

