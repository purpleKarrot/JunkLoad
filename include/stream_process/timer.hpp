#ifndef __STREAM_PROCESS__TIMER__HPP__
#define __STREAM_PROCESS__TIMER__HPP__

#include <boost/timer.hpp>

namespace stream_process
{

class timer
{

public:
	timer() :
		elapsed(0)
	{
	}

	double get_seconds()
	{
		return elapsed;
	}

	void start()
	{
		impl.restart();
	}

	void end()
	{
		elapsed = impl.elapsed();
	}

protected:
	boost::timer impl;
	double elapsed;
};

} // namespace stream_process

#endif
