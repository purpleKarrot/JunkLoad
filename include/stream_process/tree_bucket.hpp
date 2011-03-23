#ifndef __STREAM_PROCESS__TREE_BUCKET__HPP__
#define __STREAM_PROCESS__TREE_BUCKET__HPP__

#include <functional>
#include <algorithm>
#include <list>
#include "Exception.h"

namespace stream_process
{

template< typename payload_t, template< typename payload_t > class bucket_t >
class tree_bucket : private bucket_t< payload_t >
{
public:
	typedef bucket_t< payload_t> bucket_type;
	using bucket_type::iterator;
	using bucket_type::const_iterator;
	using bucket_type::reverse_iterator;
	using bucket_type::const_reverse_iterator;
	using bucket_type::begin;
	using bucket_type::end;
	using bucket_type::clear;

	bool insert_payload( const payload_t& payload )
	{
		if ( bucket_type::size() < _capacity )
		{
			bucket_type::push_back( payload );
			return true;
		}
		else
		{
			return false;
		}
	}
	
	bool remove_payload( const payload_t& payload )
	{
		typename bucket_type::iterator it 
			= std::find( bucket_type::begin(), bucket_type::end(), payload );
		typename bucket_type::iterator it_end = bucket_type::end();
		
		if ( it != it_end )
		{
			std::iter_swap( it, it_end - 1 );
			bucket_type::pop_back();
			if ( bucket_type::empty() )
				return false;
			return true;
		}
		else
		{
			throw STREAM_EXCEPT( "payload not found in bucket." );
		}
	}

	static void set_capacity( size_t capacity_ )
	{
		_capacity = capacity_;
	}
protected:
	static size_t	_capacity;
	
}; // class bucket

template< typename payload_t, template< typename payload_t > class bucket_t >
size_t tree_bucket< payload_t, bucket_t >::_capacity = 4;

} // namespace stream_process

#endif

