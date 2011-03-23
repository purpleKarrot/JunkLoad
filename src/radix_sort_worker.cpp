#include "radix_sort_worker.hpp"

namespace stream_process
{

radix_sort_worker::radix_sort_worker( mapped_point_data& mapped_point_data_, 
    size_t offset_ )
    : _mapped_point_data( mapped_point_data_ )
    , _point_size_in_bytes( 
            _mapped_point_data.get_point_info().get_point_size_in_bytes() 
        )
{
    _data.set_offset( offset_ );

    _tmp_buffer = new char[ _point_size_in_bytes ];
}




radix_sort_worker::radix_sort_worker( const radix_sort_worker& source_ )
    : _mapped_point_data( source_._mapped_point_data )
    , _point_size_in_bytes( source_._point_size_in_bytes )
{
    _data.set_offset( source_._data.get_offset() );
    _tmp_buffer = new char[ _point_size_in_bytes ];
}




radix_sort_worker::~radix_sort_worker()
{
    delete _tmp_buffer;
}



void
radix_sort_worker::setup( const sort_range& sort_range_ )
{
    _sort_range     = sort_range_;
}



void
radix_sort_worker::operator()()
{
    _sort_partially();

}



void
radix_sort_worker::sort()
{
    _sort_partially();
}



size_t
radix_sort_worker::get_results( sort_range& left_, sort_range& right_ ) const
{
    left_   = _left_part;
    right_  = _right_part;
    
    return left_.sorting_bit;
}



void
radix_sort_worker::_sort_partially()
{
    assert( _sort_range.left < _sort_range.right );
    
    const size_t axis_offset    = _data.get_offset();
    const size_t& one_point     = _point_size_in_bytes;

    char* begin_ptr             = _mapped_point_data.get_data_ptr();

    char* left_ptr              = begin_ptr + _sort_range.left * _point_size_in_bytes;
    char* right_ptr             = begin_ptr + _sort_range.right * _point_size_in_bytes;

    char* end_ptr               = right_ptr + one_point;
    
    uint32_t sort_bit_mask      = 1u << _sort_range.sorting_bit;
    
#define SORT_BIT_IS_ZERO( ptr )\
	! ( *reinterpret_cast< uint32_t* >( ptr + axis_offset ) & sort_bit_mask )

#define SORT_BIT_IS_ONE( ptr )\
	( *reinterpret_cast< uint32_t* >( ptr + axis_offset ) & sort_bit_mask )

#define SWAP_POINTS( first, second ) \
	memcpy( _tmp_buffer, first, _point_size_in_bytes );\
	memcpy( first, second, _point_size_in_bytes );\
	memcpy( second, _tmp_buffer, _point_size_in_bytes ); 

// debug
//    rt_struct_member< uint32_t > data;
//    data.set_offset( axis_offset );
//#define GET_VALUE( point ) reinterpret_cast< stream_point* >( point )->get( data )

    while( left_ptr != right_ptr )
    {
        //std::cout << "left "    << (void*) left_ptr     << std::endl;
        //std::cout << "right "   << (void*) right_ptr    << std::endl;
        //std::cout << ( right_ptr - left_ptr ) / _point_size_in_bytes << std::endl;
        
        while( SORT_BIT_IS_ZERO( left_ptr ) && left_ptr < right_ptr )
        {
            //std::cout << "plus left" << std::endl;
            left_ptr += one_point;
            //assert( GET_VALUE( left_ptr ) != 0u );
        }
        while( SORT_BIT_IS_ONE( right_ptr ) && right_ptr > left_ptr )
        {
            //std::cout << "minus right" << std::endl;
            right_ptr -= one_point;
            //assert( GET_VALUE( right_ptr ) != 0u );
        }
        SWAP_POINTS( left_ptr, right_ptr );
	}

    // correct for right_ptr 0-bit
    if ( SORT_BIT_IS_ZERO( right_ptr ) )
    {
        right_ptr += one_point;
    }
        
    size_t diff			= right_ptr - begin_ptr;
    diff /= _point_size_in_bytes;

    //debug std::cout << "diff " << diff << std::endl;

    _left_part.left    = _sort_range.left;
    _left_part.right   = diff - 1;

    _right_part.left   = diff;
    _right_part.right  = _sort_range.right;
    _left_part.sorting_bit = _right_part.sorting_bit 
        = _sort_range.sorting_bit - 1;
        
    //debug std::cout << "radix sorted range " << _left_right << ", new ranges are "
    //debug     << _left_part << " and " << _right_part << std::endl;
        
    // if all of the sorting bits are the same in the range, we want the resulting
    // 'all' range to be in _left_part
    if ( _left_part.size() == _sort_range.size() )
    {
        _right_part = _left_part;
    }
    if ( _right_part.size() == _sort_range.size() )
    {
        _left_part = _right_part;
    }
    
    //_debug_print_sorted_range();

}



void
radix_sort_worker::debug_test_sorting()
{
    size_t left				= _sort_range.left;
    size_t right			= _sort_range.right;
    
    assert( left < right );
    
    char* begin_ptr			= _mapped_point_data.get_data_ptr();
    size_t axis_offset		= _data.get_offset();
    
	const size_t& one_point	= _point_size_in_bytes;

    char* left_ptr			= begin_ptr + left * one_point;
    char* right_ptr         = begin_ptr + right * one_point;
    char* end_ptr           = right_ptr + one_point;
    
    uint32_t sort_bit_mask	= 1u << _sort_range.sorting_bit;
    
#define SORT_BIT_IS_ZERO( ptr )\
	! ( *reinterpret_cast< uint32_t* >( ptr + axis_offset ) & sort_bit_mask )

#define SORT_BIT_IS_ONE( ptr )\
	( *reinterpret_cast< uint32_t* >( ptr + axis_offset ) & sort_bit_mask )

    bool still_zero = true;
    size_t counter = 0;
    while( left_ptr != right_ptr )
    {
        if ( still_zero )
        {
            if ( SORT_BIT_IS_ONE( left_ptr ) )
            {
                still_zero = false;
            }
            else
                ++counter;
            left_ptr += one_point;
        }
        else
        {
            if ( SORT_BIT_IS_ZERO( left_ptr ) )
                assert( 0 );
            left_ptr += one_point;
        }    
    }
    std::cout 
        << "tested: radix range: " << left << "-" << right 
        << " bit: " << _sort_range.sorting_bit 
        << " mask " << sort_bit_mask 
        << std::endl;
}



void
radix_sort_worker::_debug_print_sorted_range()
{
    size_t left				= _sort_range.left;
    size_t right			= _sort_range.right;
    
    assert( left < right );
    
    char* begin_ptr			= _mapped_point_data.get_data_ptr();
    size_t axis_offset		= _data.get_offset();
    
	const size_t& one_point	= _point_size_in_bytes;

    char* left_ptr			= begin_ptr + left * _point_size_in_bytes;
    char* right_ptr			= begin_ptr + right * _point_size_in_bytes;
    char* end_ptr           = right_ptr + one_point;

    for ( size_t i = _sort_range.left; left_ptr != right_ptr; left_ptr += one_point, ++i )
    {
        std::cout << i << ": " << 
            _debug_print_mask( *reinterpret_cast< uint32_t* >( left_ptr + axis_offset ) )
            << std::endl;
    }

}



std::string
radix_sort_worker::_debug_print_mask( const uint32_t& bitmask ) const
{
    std::string out = "";
    for( size_t i = 0; i < 32; ++i )
    {
        if ( i == _sort_range.sorting_bit )
            out = "]" + out;
            
        if ( bitmask & 1u << i )
            out = "1" + out;
        else
            out = "0" + out;

        if ( i == _sort_range.sorting_bit )
            out = "[" + out;
            
    }
    return out;
}



const sort_range&
radix_sort_worker::get_left_range() const
{
    return _left_part;
}



const sort_range&
radix_sort_worker::get_right_range() const
{
    return _right_part;
}


} // namespace stream_process

