#ifndef PPK_STREAM_ITERATOR_HPP
#define PPK_STREAM_ITERATOR_HPP

#include <boost/iterator/iterator_facade.hpp>

namespace junk
{

template<typename Value>
class stream_iterator_base:
	public boost::iterator_facade<
		stream_iterator_base<Value>,
		Value,
		boost::random_access_traversal_tag,
		Value,
		std::size_t
		>
{
public:
	stream_iterator_base(Value value, std::size_t element_size) :
			value_(value), element_size_(element_size)
	{
	}

private:
	friend class boost::iterator_core_access;
	template<class> friend class node_iter;

	stream_iterator_base() :
			value_(0), element_size_(0)
	{
	}

	Value dereference() const
	{
		return value_;
	}

	template<class OtherValue>
	bool equal(const stream_iterator_base<OtherValue>& other) const
	{
		return value_ == other.value_;
	}

	void increment()
	{
		value_ += element_size_;
	}

	void decrement()
	{
		value_ -= element_size_;
	}

	void advance(std::size_t steps)
	{
		value_ += steps * element_size_;
	}

	std::size_t distance_to(const stream_iterator_base& other) const
	{
		return (value_ - other.value_) / element_size_;
	}

private:
	Value value_;
	std::size_t element_size_;
};

typedef stream_iterator_base<char*> stream_iterator;
typedef stream_iterator_base<const char*> const_stream_iterator;

} // namespace junk

#endif /* PPK_STREAM_ITERATOR_HPP */
