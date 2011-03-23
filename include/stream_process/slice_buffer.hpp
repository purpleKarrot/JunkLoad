#ifndef __STREAM_PROCESS__SLICE_BUFFER__HPP__
#define __STREAM_PROCESS__SLICE_BUFFER__HPP__

#include <stream_process/stream_process_types.hpp>
#include <stream_process/slice_tracker.hpp>
#include <stream_process/functors.hpp>
#include <stream_process/bit.hpp>

#include <cstddef>

#include <queue>
#include <utility>
#include <functional>


namespace stream_process
{
template< typename sp_types_t >
class slice_buffer 
	: public sp_types_t
	, private std::deque< typename sp_types_t::slice_type* >
{
public:
	STREAM_PROCESS_TYPES

    typedef std::deque< slice_type* >           super;

    typedef slice_tracker< sp_types_t >         tracker_type;
    typedef ptr_greater< slice_type* >          greater_type;

	typedef std::deque< slice_type* >           container_t;

	// enforces that slices are being released in ascending order.
	static const size_t		FORCE_NEXT		= 1; 
	static const size_t		TEST_MAX_BOUNDS	= 1 << 1;
    static const size_t     CLEAR_STAGE     = 1 << 2;

	
	slice_buffer()
        : _flags( 0 )
        , _next_slice_number( 0 )
        , _largest_slice( 0 )
    {}
	
	using super::size;
	using super::empty;
	
	void push( slice_type* slice )
	{
        assert( slice );
        
		super::push_back( slice );
		if ( _flags & FORCE_NEXT )
		{
			if ( ! _largest_slice || (*slice) > (*_largest_slice ) )
				_largest_slice = slice;

            assert( _largest_slice );
            
			std::push_heap( super::begin(), super::end(), greater_type() );
		}
	}

	void pop() 
	{
		assert( ! super::empty() );
		if ( _flags & FORCE_NEXT )
		{
			std::pop_heap( super::begin(), super::end(), greater_type() );
			
			if ( super::back() == _largest_slice )
			{
				assert( super::size() == 1 );
				_largest_slice = 0;
			}
            assert( super::back()->get_slice_number() == _next_slice_number );
            ++_next_slice_number;
			super::pop_back();
		}
		else
		{
			super::pop_front();
		}
	}
	
	slice_type* top()
	{
		if ( super::empty() )
        {
			return 0;
        }

		slice_type* top_ = super::front();
        assert( top_ );

		if ( _flags & FORCE_NEXT )
        { 
            if ( _next_slice_number != top_->get_slice_number() )
            {
                #if 0
                    std::cout
                        << "slice " << top_->get_slice_number()
                        << " failed NEXT test, expected " 
                        << _next_slice_number
                        << std::endl;
                #endif
                return 0;
            }
		}
				
		if ( _flags & TEST_MAX_BOUNDS ) 
        { 
            assert( _largest_slice );
            if ( _flags & CLEAR_STAGE && _largest_slice->is_last() )
            { 
                    return top_;
            }
            else if ( ! top_->ring_one_max_safe( *_largest_slice ) )
            {
                #if 0
                std::cout
                    << "slice " << top_->get_slice_number()
                    << " failed RING ONE test vs. " 
                    << _largest_slice->get_slice_number()
                    << std::endl;
                #endif
                return 0;
            }
		}
        #if 0
        std::cout
            << "slice " << top_->get_slice_number()
            << " passed all tests";
        if ( _largest_slice )
            std::cout << " vs. " << _largest_slice->get_slice_number();
        std::cout
            << std::endl;
        #endif
		return top_;
	}


	bool get_force_next() const
	{
		return get_bit( _flags, FORCE_NEXT );
	}
	void set_force_next( bool force_next_ )
	{
		set_bit( _flags, FORCE_NEXT, force_next_ );
	}
	bool get_test_max_bounds() const
	{
		return get_bit( _flags, TEST_MAX_BOUNDS );
	}
	void set_test_max_bounds( bool test_max_ )
	{
		set_bit( _flags, TEST_MAX_BOUNDS, test_max_ );
	}
	void set_clear_stage( bool clear_stage_ )
	{
		set_bit( _flags, CLEAR_STAGE, clear_stage_ );
	}
		
protected:
	size_t				_flags;
	size_t				_next_slice_number;
	slice_type*         _largest_slice;
	tracker_type        _tracker;
	
}; // class slice_buffer

} // namespace stream_process

#endif

