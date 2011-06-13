#ifndef __TRIP__INTRO_SORT__HPP__
#define __TRIP__INTRO_SORT__HPP__

#include <boost/threadpool.hpp>

#include <stdlib.h>
#include <vector>
#include <algorithm>

namespace trip
{
namespace detail
{

inline int bsr(int _value)
{
	int result;
#if defined __i386 && defined __GNUC__
	__asm__("bsrl %1, %0 \n\t"
			: "=r"(result)
			: "r"(_value));
#else
	result = 0;
	for (; _value != 1; _value >>= 1)
		++result;
#endif
	return result;
}

template<typename T>
inline T median(const T& begin, const T& center, const T& end)
{
	if (center < begin && center < end)
		return begin < end ? begin : end;

	if (begin < center && end < center)
		return begin < end ? end : begin;

	return center;
}

template<typename Iterator>
inline void insertion_sort(Iterator begin_, Iterator end_)
{
	typedef typename std::iterator_traits<Iterator>::value_type value_t;

	// if only two members, just swap
	if (end_ - begin_ == 2)
	{
		if (*(end_ - 1) < *begin_)
			std::iter_swap(begin_, end_ - 1);
		return;
	}

	for (Iterator i = begin_ + 1; i != end_; ++i)
	{
		Iterator j = i;
		value_t v = *i;

		while (j != begin_ && v < *(j - 1))
		{
			*j = *(j - 1);
			--j;
		}

		*j = v;
	}
}

template<typename Iterator>
void do_intro_sort(Iterator begin_, Iterator end_,
		std::size_t depth_, boost::threadpool::pool& _threadpool)
{
	typedef typename std::iterator_traits<Iterator>::value_type value_t;

	enum thresholds
	{
		// limit for insertion sort
		INSERTION_THRESHOLD = 16,
		// minimum members to assign a new thread
		THREAD_THRESHOLD = 4096
	};

	while (end_ - begin_ > 1)
	{
		// if only few members are left use insertion sort
		if (end_ - begin_ < INSERTION_THRESHOLD)
		{
			insertion_sort(begin_, end_);
			return;
		}

		// if recursion limit reached use heap sort
		if (depth_-- == 0)
		{
			std::make_heap(begin_, end_);
			std::sort_heap(begin_, end_);
			return;
		}

		// calculate the pivot
		value_t p = median(*begin_, *(begin_ + (end_ - begin_) / 2), *(end_ - 1));
		Iterator i = begin_, j = end_;

		// quicksort partitioning
		while (true)
		{
			while (*i < p)
				++i;
			--j;
			while (p < *j)
				--j;
			if (!(i < j))
				break;
			std::iter_swap(i, j);
			++i;
		}

		int r = end_ - i;
		if (r > 1)
		{
			if (r < THREAD_THRESHOLD)
			{
				do_intro_sort(i, end_, depth_, _threadpool);
			}
			else
			{
				_threadpool.schedule(boost::bind(&do_intro_sort<Iterator>,
						i, end_, depth_, boost::ref(_threadpool)));
			}
		}

		end_ = i;
	}
}

} // namespace detail

template<class Iterator>
void intro_sort(Iterator first, Iterator last)
{
	int number_of_threads = boost::thread::hardware_concurrency();
	if (number_of_threads < 1)
		number_of_threads = 1;

	boost::threadpool::pool threadpool(number_of_threads);

	std::size_t depth = 2 * trip::detail::bsr(last - first);

	if (last - first > 1)
	{
		detail::do_intro_sort(first, last, depth, threadpool);
		threadpool.wait();
	}
}

} // namespace trip

#endif
