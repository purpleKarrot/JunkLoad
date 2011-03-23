#ifndef __STREAM_PROCESS__RADIX_SORT_WORKER__HPP__
#define __STREAM_PROCESS__RADIX_SORT_WORKER__HPP__

#include "mapped_point_data.hpp"
#include "rt_struct_member.h"
#include "sort_range.hpp"

#include <utility>

namespace stream_process
{

class radix_sort_worker
{
public:
    radix_sort_worker( mapped_point_data& mapped_point_data_, size_t offset_ );
    radix_sort_worker( const radix_sort_worker& source_ );
    ~radix_sort_worker();

    void setup( const sort_range& left_right );

    void sort();
    void operator()();
    
    // copies the two ranges into the parameters, and returns the sorting bit
    // as retval
    size_t get_results( sort_range& left_, sort_range& right_ ) const;
    const sort_range& get_left_range() const;
    const sort_range& get_right_range() const;

    // sloooooow :)
    void debug_test_sorting();
    
protected:
    void    _sort_partially();
    void    _debug_print_sorted_range();
    std::string _debug_print_mask( const uint32_t& bitmask ) const;
    
    mapped_point_data&              _mapped_point_data;
    size_t                          _point_size_in_bytes;
    
    rt_struct_member< uint32_t >    _data;
    
    sort_range                      _sort_range;
    sort_range                      _left_part;
    sort_range                      _right_part;

    char*                           _tmp_buffer;
    
}; // class radix_sort_worker

} // namespace stream_process

#endif

