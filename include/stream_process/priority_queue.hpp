#ifndef __STREAM_PROCESS__PRIORITY_QUEUE__HPP__
#define __STREAM_PROCESS__PRIORITY_QUEUE__HPP__

#include <queue>

namespace stream_process
{

template< typename T, typename container_t, typename compare_t >
class priority_queue : public std::priority_queue< T, container_t, compare_t >
{
public:
    typedef std::priority_queue< T, container_t, compare_t >    super;

    inline const T& front()
    {
        return super::top();
    }

protected:

}; // class priority_queue

} // namespace stream_process

#endif

