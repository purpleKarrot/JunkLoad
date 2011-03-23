#ifndef __STREAM_PROCESS__BIT__HPP__
#define __STREAM_PROCESS__BIT__HPP__

#include <cstddef>

namespace stream_process
{

inline bool get_bit( const size_t flag, const size_t query_flag )
{
		return flag & query_flag;
}

inline void set_bit( size_t& flag, const size_t query_flag, bool new_state_ )
{
	if ( ( flag & query_flag ) != new_state_ )
		flag ^= query_flag;
}

} // namespace stream_process

#endif

