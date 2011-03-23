#ifndef __TRIP__INTRO_SORT__HPP__
#define __TRIP__INTRO_SORT__HPP__

#include <boost/threadpool.hpp>

#include <stdlib.h>
#include <vector>
#include <algorithm>

namespace trip
{

template<typename value_t, typename iterator_t,
		typename compare_t = typename std::less<value_t> >
class intro_sort
{
public:
	intro_sort(boost::threadpool::pool& threadpool_);

	inline void operator()(iterator_t begin_, iterator_t end_) const;

private:
	inline int bsr(int value_) const;

	enum thresholds
	{
		// limit for insertion sort
		INSERTION_THRESHOLD = 16,
		// minimum members to assign a new thread
		THREAD_THRESHOLD = 4096
	};

	// median of _begin, _middle and _end
	inline const value_t& median(const value_t& _begin, const value_t& _middle,
			const value_t& _end) const;

	void insertion_sort(iterator_t begin_, iterator_t end_) const;

	// intro sort based on the stl intro sort
	void sort(iterator_t begin_, iterator_t end_, size_t depth_) const;

	boost::threadpool::pool& _threadpool;
};

template<typename value_t, typename iterator_t, typename compare_t>
inline const value_t& intro_sort<value_t, iterator_t, compare_t>::median(
		const value_t& begin_, const value_t& middle_, const value_t& end_) const
{
	if (compare_t()(middle_, begin_) && compare_t()(middle_, end_))
		return compare_t()(begin_, end_) ? begin_ : end_;
	else if (compare_t()(begin_, middle_) && compare_t()(end_, middle_))
		return compare_t()(begin_, end_) ? end_ : begin_;
	else
		return middle_;
}

template<typename value_t, typename iterator_t, typename compare_t>
inline void intro_sort<value_t, iterator_t, compare_t>::insertion_sort(
		iterator_t begin_, iterator_t end_) const
{
	// if only two members, just swap
	if (end_ - begin_ == 2)
	{
		if (*(end_ - 1) < *begin_)
			std::iter_swap(begin_, end_ - 1);
		return;
	}

	for (iterator_t i = begin_ + 1; i != end_; ++i)
	{
		iterator_t j = i;
		value_t v = *i;
		while (j != begin_ && compare_t()(v, *(j - 1)))
		{
			*j = *(j - 1);
			--j;
		}
		*j = v;
	}
}

template<typename value_t, typename iterator_t, typename compare_t>
intro_sort<value_t, iterator_t, compare_t>::intro_sort(
		boost::threadpool::pool& threadpool_) :
	_threadpool(threadpool_)
{
}

template<typename value_t, typename iterator_t, typename compare_t>
inline void intro_sort<value_t, iterator_t, compare_t>::operator()(
		iterator_t begin_, iterator_t end_) const
{
	if (end_ - begin_ > 1)
	{
		sort(begin_, end_, 2 * bsr(end_ - begin_));
		_threadpool.wait();
	}
}

template<typename value_t, typename iterator_t, typename compare_t>
void intro_sort<value_t, iterator_t, compare_t>::sort(iterator_t begin_,
		iterator_t end_, size_t depth_) const
{
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
		value_t p = median(*begin_, *(begin_ + (end_ - begin_) / 2),
				*(end_ - 1));
		iterator_t i = begin_, j = end_;

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
				sort(i, end_, depth_);
			else
				_threadpool.schedule(boost::bind(&intro_sort::sort, this, i,
						end_, depth_));
		}

		end_ = i;
	}
}

template<typename value_t, typename iterator_t, typename compare_t>
inline int intro_sort<value_t, iterator_t, compare_t>::bsr(int _value) const
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

} // namespace trip

#endif
