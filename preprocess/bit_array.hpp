#ifndef __STREAM_PROCESS__BIT_ARRAY__HPP__
#define __STREAM_PROCESS__BIT_ARRAY__HPP__

#include <cstddef>
#include <iostream>

namespace stream_process
{

class bit_array
{
public:
	inline bit_array() :
		array(0)
	{
	}

	inline bool get_bit(const size_t flag) const
	{
		return array & flag;
	}

	inline void set_bit(const size_t flag, bool new_state_)
	{
		if (flag & array)
		{
			if (!new_state_)
			{
				array ^= flag;
			}
		}
		else
		{
			if (new_state_)
			{
				array |= flag;
			}
		}
	}

	inline void clear()
	{
		memset(&array, 0, sizeof(size_t));
	}

	inline const bit_array& operator=(size_t flags_)
	{
		array = flags_;
		return *this;
	}

	size_t array;

}; // class bit_array

} // namespace gloo

#endif

