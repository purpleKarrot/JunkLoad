#ifndef __STREAM_PROCESS__SORT_WORKER__HPP__
#define __STREAM_PROCESS__SORT_WORKER__HPP__

#include "sorting_controller.hpp"

#include "worker_thread.hpp"
#include "sort_range.hpp"

#include <vector>
#include <queue>

namespace stream_process
{

class radix_sort_worker;
class insertion_sort_worker;
class sort_key_transform;

class mapped_point_data;

class sort_worker : public worker_thread< sorting_controller >
{
public:
    typedef std::priority_queue< sort_range, 
        std::deque< sort_range >, std::greater< sort_range > > sort_range_priority_queue;

    sort_worker( mapped_point_data& mapped_point_data_, size_t offset );
    // copy-ctor required for boost::thread 
    sort_worker( const sort_worker& source_ );
    ~sort_worker();
    
    void sort();
    
    void sort_single_threaded();
    
    // boost::thread wants a functor
    inline void operator()() { sort(); };

    void add_sort_range( const sort_range& sort_range_ );
    
    void set_sort_range( const sort_range& sort_range_ );
    
    void radix_sort();
    void insertion_sort();
    void reverse_key_transform();
    
    std::list< sort_range >&    get_sorted_ranges();
    std::list< sort_range >&    get_partially_sorted_ranges();
    

protected:
    void _sort_next();
    void _finished_radix_sort( const sort_range& sort_range_ );

    radix_sort_worker*          _radix_sort;
    insertion_sort_worker*      _insertion_sort;
    sort_key_transform*         _key_transform;
    sort_range                  _sort_range;
    std::list< sort_range >     _todos;
    std::list< sort_range >     _partially_sorted;
    std::list< sort_range >     _sorted;
    size_t                      _sleep_duration;

}; // class sort_worker

} // namespace stream_process

#endif

