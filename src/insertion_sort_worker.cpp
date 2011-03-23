#include "insertion_sort_worker.hpp"

#include <cassert>

namespace stream_process
{

insertion_sort_worker::insertion_sort_worker( mapped_point_data& mapped_point_data_, 
    size_t offset_ )
    : _mapped_point_data( mapped_point_data_ )
    , _point_size_in_bytes( 
            _mapped_point_data.get_point_info().get_point_size_in_bytes() 
        )
{
    _data.set_offset( offset_ );

    _tmp_buffer = new char[ _point_size_in_bytes ];
}



insertion_sort_worker::insertion_sort_worker( const insertion_sort_worker& source_ )
    : _mapped_point_data( source_._mapped_point_data )
    , _point_size_in_bytes( source_._point_size_in_bytes )
{
    _data.set_offset( source_._data.get_offset() );
    
    _tmp_buffer = new char[ _point_size_in_bytes ];
}




insertion_sort_worker::~insertion_sort_worker()
{
    delete _tmp_buffer;
}



void
insertion_sort_worker::setup( const sort_range& sort_range_ )
{
    _sort_range     = sort_range_;
}



void
insertion_sort_worker::operator()()
{
    _sort();
}



void
insertion_sort_worker::sort()
{
    _sort();
}



void
insertion_sort_worker::_sort()
{
	const size_t& one_point = _point_size_in_bytes;

    char* base_ptr      = _mapped_point_data.get_data_ptr();  
    char* left_ptr      = base_ptr + _sort_range.left * one_point;
    char* right_ptr     = base_ptr + _sort_range.right * one_point;
    char* end_ptr       = right_ptr + one_point;
 
    char* j;
	char* value				= _tmp_buffer;
	
// yeah macros are evil, but otherwise the code is just unreadable.
#define GET_VALUE( point ) reinterpret_cast< stream_point* >( point )->get( _data )
#define COPY_POINT( target, source ) memcpy( target, source, _point_size_in_bytes )
	
    for( char* i = left_ptr + one_point; i < end_ptr; i += one_point )
    {
        COPY_POINT( value, i );
		j = i;
        j -= one_point;
        
        while ( j >= left_ptr && GET_VALUE( j ) > GET_VALUE( value ) )
        {
			COPY_POINT( j + one_point, j );
            j -= one_point;
            //debug assert( GET_VALUE( j ) != 0u );
            //debug assert( GET_VALUE( value ) != 0u );
        }
		COPY_POINT( j + one_point, value );
    }

    // debug
    #if 0
    for( char* i = left_ptr + one_point; i < end_ptr; i += one_point )
    {
        assert( GET_VALUE( i - one_point ) <= GET_VALUE( i ) );
    }
    #endif
/*
    for( size_t i = left + 1, j; i < size; ++i )
    {
        value = points[ i ];
        j = i;
        j -= 1;
        
        while ( j >= 0 && points[ j ] > value )
        {
            points[ j + 1 ] = points[ j ];
            j -= 1;
        }
        points[ j + 1 ] = value;
        
    }
*/

}



void
insertion_sort_worker::debug_test_range( const std::string& msg )
{
#define GET_VALUE( point ) reinterpret_cast< stream_point* >( point )->get( _data )

    const size_t&   left    = _sort_range.left;
    const size_t&   right   = _sort_range.right;
    const size_t    size    = _sort_range.size();
  
	const size_t& one_point = _point_size_in_bytes;

    char* begin_ptr         = _mapped_point_data.get_data_ptr();  
    char* left_ptr          = begin_ptr + left * _point_size_in_bytes;
    char* right_ptr         = begin_ptr + right * _point_size_in_bytes;
    char* end_ptr           = right_ptr + one_point;
    

    for( char* i = left_ptr + one_point; i < end_ptr; i += one_point )
    {
        const uint32_t& a = GET_VALUE( i - one_point );
        const uint32_t& b = GET_VALUE( i );
        bool ok = ( a <= b );
        if ( ! ok )
        {
            std::cout << "a " << GET_VALUE( i - one_point ) << std::endl;
            std::cout << "b " << GET_VALUE( i ) << std::endl;
            std::cout << "error while insertion sorting " << left << " to " << right << ". " << msg << std::endl;
            assert( 0 );
        }
    }
    
    std::cout << "insertion_sorted " << left << " to " << right << ". " << msg << std::endl;

}



void
insertion_sort_worker::debug_test_range_float( const std::string& msg )
{
    rt_struct_member< float > data;
    data.set_offset( _data.get_offset() );
#define GET_VALUE_F( point ) reinterpret_cast< stream_point* >( point )->get( data )

    const size_t&   left    = _sort_range.left;
    const size_t&   right   = _sort_range.right;
    const size_t    size    = _sort_range.size();
  
	const size_t& one_point = _point_size_in_bytes;

    char* begin_ptr         = _mapped_point_data.get_data_ptr();  
    char* left_ptr          = begin_ptr + left * _point_size_in_bytes;
    char* right_ptr         = begin_ptr + right * _point_size_in_bytes;
    char* end_ptr           = right_ptr + one_point;
    
    size_t index = _sort_range.left;
    for( char* i = left_ptr + one_point; i < end_ptr; i += one_point, ++index )
    {
        const float& a = GET_VALUE_F( i - one_point );
        const float& b = GET_VALUE_F( i );
        bool ok = ( a <= b );
        if ( ! ok )
        {
            std::cout << "a (" << index << ") " << GET_VALUE_F( i - one_point ) << std::endl;
            std::cout << "b (" << index + 1 << ") " << GET_VALUE_F( i ) << std::endl;
            std::cout << "error while insertion sorting " << left << " to " << right << ". " << msg << std::endl;
            assert( 0 );
        }
    }
    
    std::cout << "insertion_sorted " << left << " to " << right << ". " << msg << std::endl;

}



} // namespace stream_process

