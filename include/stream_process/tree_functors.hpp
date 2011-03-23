#ifndef __STREAM_PROCESS__TREE_FUNCTORS__HPP__
#define __STREAM_PROCESS__TREE_FUNCTORS__HPP__

namespace stream_process
{

template< typename T >
struct passthrough_accessor
{
    inline const T& operator()( const T& obj ) const
    {
        return obj;
    }
};


template< typename T >
struct dereference_accessor
{
    inline const T& operator()( const T* obj ) const
    {
        return *obj;
    }
};


} // namespace stream_process

#endif

