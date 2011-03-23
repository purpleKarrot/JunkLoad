#ifndef __STREAM_PROCESS__INSERTION_SORT_WORKER__HPP__
#define __STREAM_PROCESS__INSERTION_SORT_WORKER__HPP__

#include "mapped_point_data.hpp"
#include "rt_struct_member.h"
#include "sort_range.hpp"

#include <utility>

namespace stream_process
{

class insertion_sort_worker
{
public:
    insertion_sort_worker( mapped_point_data& mapped_point_data_, size_t offset_ );
    insertion_sort_worker( const insertion_sort_worker& source );
    ~insertion_sort_worker();

    void setup( const sort_range& left_right );

    void sort();
    void operator()();
    
    void debug_test_range( const std::string& msg = "" );
    void debug_test_range_float( const std::string& msg = "" );
    
protected:
    void _sort();

    mapped_point_data&          _mapped_point_data;
    const size_t                _point_size_in_bytes;
    
    rt_struct_member< uint32_t >   _data;
    
    sort_range                  _sort_range;
    char*                       _tmp_buffer;
    
}; // class insertion_sort_worker

} // namespace stream_process

#endif

