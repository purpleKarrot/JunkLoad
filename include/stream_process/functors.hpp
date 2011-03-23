#ifndef __STREAM_PROCESS__FUNCTORS__HPP__
#define __STREAM_PROCESS__FUNCTORS__HPP__

namespace stream_process
{

// a functor that can be used to sort lists of pointers or iterators
template< typename T >
struct ptr_less : std::binary_function< T, T, bool >
{
inline bool operator()( T const& a, T const& b ) const
{
    return (*a) < (*b);
}
}; // struct ptr_compare

template< typename T >
struct ptr_greater : std::binary_function< T, T, bool >
{
inline bool operator()( T const& a, T const& b ) const
{
    return (*a) > (*b);
}
}; // struct ptr_compare

} // namespace stream_process

#endif

