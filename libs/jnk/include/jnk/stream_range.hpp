#ifndef PPK_ELEMENT_RANGE_HPP
#define PPK_ELEMENT_RANGE_HPP

#include "stream_iterator.hpp"
#include <boost/range/iterator_range.hpp>

namespace junk
{

typedef boost::iterator_range<stream_iterator> stream_range;
typedef boost::iterator_range<const_stream_iterator> const_stream_range;

inline stream_range make_stream_range(char* begin, char* end,
		std::size_t element_size)
{
	return stream_range(
			stream_iterator(begin, element_size),
			stream_iterator(end, element_size)
			);
}

inline const_stream_range make_stream_range(const char* begin, const char* end,
		std::size_t element_size)
{
	return const_stream_range(
			const_stream_iterator(begin, element_size),
			const_stream_iterator(end, element_size)
			);
}

} // namespace junk

#endif /* PPK_ELEMENT_RANGE_HPP */
