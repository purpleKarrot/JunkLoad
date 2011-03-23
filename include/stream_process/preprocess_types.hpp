#ifndef __STREAM_PROCESS__PREPROCESS_TYPES__HPP__
#define __STREAM_PROCESS__PREPROCESS_TYPES__HPP__

namespace stream_process
{

namespace preprocess
{

template< typename Tv, typename Ti >
struct sort_reference
{
    typedef Tv      value_type;
    typedef Ti      index_type;

    value_type  value;
    index_type  index;
    
    inline bool operator<( const sort_reference& oref ) const
    {
        return value < oref.value;
    }

    inline bool operator>( const sort_reference& oref ) const
    {
        return value > oref.value;
    }
};


template< typename T >
struct index_reference
{
    typedef T   index_type;

    index_type  new_index;
    index_type  old_index;
};

} // namespace preprocess

} // namespace stream_process

#endif

