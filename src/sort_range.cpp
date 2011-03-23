#include "sort_range.hpp"

namespace stream_process
{
sort_range::sort_range()
{}


sort_range::sort_range( size_t left_, size_t right_ )
    : left( left_ )
    , right( right_ )
    , sorting_bit( 1 << 31 )
{
    
}


size_t
sort_range::size() const
{
    return right - left;
}


} // namespace stream_process

