#ifndef __STREAM_PROCESS__BIT_ARRAY__HPP__
#define __STREAM_PROCESS__BIT_ARRAY__HPP__

#include <cstddef>

namespace stream_process
{

class bit_array
{
public:
	bit_array() :
			array(0)
	{
	}

	bool get_bit(std::size_t flag) const
	{
		return array & flag;
	}

	void set_bit(std::size_t flag, bool new_state_)
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

	void clear()
	{
		array = 0;
	}

	const bit_array& operator=(std::size_t flags_)
	{
		array = flags_;
		return *this;
	}

	std::size_t array;
};

} // namespace stream_process

#endif
