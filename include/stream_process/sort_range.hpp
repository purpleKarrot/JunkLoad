#ifndef __STREAM_PROCESS__SORT_RANGE__HPP__
#define __STREAM_PROCESS__SORT_RANGE__HPP__

#include <utility>
#include <vector>
#include <iostream>

/*
*   @brief this class encapsules information about a range to be sorted. 
*       note that left and right are inclusive, e.g. the range is 
*       [ left, right ], and will be used as 'array-indices' into the point-map.
*
*/

namespace stream_process
{

class sort_range
{
public:
    // WARNING: the default ctor will not initialize any members
    sort_range();
    
    sort_range( size_t left_, size_t right_ );
    
    size_t size() const;
    inline const sort_range& operator=( const sort_range& other_ );
    
    inline bool operator<( const sort_range& other ) const;
    inline bool operator>( const sort_range& other ) const;
    
    friend std::ostream& operator<< ( std::ostream& os, const sort_range& range_ )
    {
        os << range_.left << "-" << range_.right;
        return os;
    }


    size_t left;
    size_t right;
    size_t sorting_bit;

}; // class sort_range



inline
const sort_range&
sort_range::operator=( const sort_range& other_ )
{
    memcpy( this, &other_, sizeof( sort_range ) );
    return *this;
}



inline
bool
sort_range::operator<( const sort_range& other ) const
{
    return left < other.left;
}



inline
bool
sort_range::operator>( const sort_range& other ) const
{
    return left > other.left;
}



} // namespace stream_process

#endif

