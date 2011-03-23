#ifndef __STREAM_PROCESS__FOREACH__HPP__
#define __STREAM_PROCESS__FOREACH__HPP__

#include <algorithm>

#include <boost/foreach.hpp>

#ifndef foreach

#define foreach BOOST_FOREACH

#endif

#if 0
template< typename T, typename functor_t >
void
foreach( T& t, functor_t& func )
{
    typedef typename T::iterator iterator;
    for( iterator it = t.begin(), it_end = t.end(); it != it_end; ++it )
    {
        func( *it );
    }
};
#endif


template< typename T, typename functor_t >
inline void
foreach_ptr( T& t, const functor_t& func )
{
    std::for_each( t.begin(), t.end(), func );
};



#endif

