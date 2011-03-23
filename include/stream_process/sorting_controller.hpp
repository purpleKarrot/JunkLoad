#ifndef __STREAM_PROCESS__RADIX_SORT_CONTROLLER__HPP__
#define __STREAM_PROCESS__RADIX_SORT_CONTROLLER__HPP__

#include "mapped_point_data.hpp"

#include "sort_range.hpp"

#include <queue>
#include <algorithm>
#include <list>

#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>

namespace stream_process
{

class sort_worker;

class radix_sort_worker;
class insertion_sort_worker;
class sort_key_transform;

class sorting_controller
{
public:
    typedef std::priority_queue< sort_range, 
        std::deque< sort_range >, std::greater< sort_range > > sort_range_priority_queue;

    sorting_controller( mapped_point_data& mapped_point_data_, 
        size_t max_thread_count_ = 8 );
    ~sorting_controller();
    
    void setup();
    void sort(); // single-threaded sorting
    void sort_multi_threaded();
    
    size_t get_new_job( sort_worker& worker );

    //void add_radix_sort_job( const sort_range& range_ );
    
    //void notify_range_finished( const sort_range& range_ );
    //void notify_ranges_finished( sort_range_priority_queue& queue_ );

    size_t  compute_and_get_min_unsorted_index();

protected:
    void _insertion_sort_and_transform_back();
    bool _check_done_queue();

    mapped_point_data&          _mapped_point_data;
    const point_info&           _point_info;

    size_t                      _max_thread_count;

    boost::mutex                _radix_sort_queue_mutex;
    sort_range_priority_queue   _radix_sort_todo_queue;

    boost::mutex                _done_queue_mutex;
    sort_range_priority_queue   _done_queue;

    size_t                      _min_unsorted_index;
    size_t                      _insertion_sort_max_size;
    
    std::list< boost::thread* >         _threads;

}; // class sorting_controller

} // namespace stream_process

#endif

