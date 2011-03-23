#include "sort_key_transform.hpp"

#include "mapped_point_data.hpp"

#include "sort_range.hpp"

namespace stream_process
{

sort_key_transform::sort_key_transform( mapped_point_data& mapped_point_data_,
        size_t offset_ )
    : _mapped_point_data( mapped_point_data_ )
    , _offset( offset_ )
{}



sort_key_transform::sort_key_transform( const sort_key_transform& source_ )
    : _mapped_point_data( source_._mapped_point_data )
    , _offset( source_._offset )
{}



void
sort_key_transform::transform()
{
    sort_range range_;
    range_.left = 0;
    range_.right = _mapped_point_data.get_point_info().get_point_count() - 1;
    
    transform( range_ );

    //LOGINFO << "transformed sort-keys for range " << range_ << std::endl;
}



void
sort_key_transform::transform_back()
{
    sort_range range_;
    range_.left = 0;
    range_.right = _mapped_point_data.get_point_info().get_point_count() - 1;
    transform_back( range_ );
}



void
sort_key_transform::transform( const sort_range& range_ )
{
    //debug std::cout << "applying key transform on points " 
    //    << range_.left << "-" << range_.right << std::endl;

    // if float is positive or zero, flip most significant bit
    // else invert bitmask
    
    const point_info& point_info_       = _mapped_point_data.get_point_info();
    const size_t point_size_in_bytes    = point_info_.get_point_size_in_bytes();
    const size_t point_count            = point_info_.get_point_count();
    
    char* base_ptr  = _mapped_point_data.get_data_ptr();
    char* begin_ptr = base_ptr + range_.left * point_size_in_bytes;
    char* end_ptr   = base_ptr + ( range_.right + 1 ) * point_size_in_bytes; 
    char* point     = begin_ptr;
    
    const uint32_t msb_only = 1u << 31;
    
    const uint32_t& min_   = (uint32_t) std::numeric_limits< int32_t >::min();
    
    size_t index = range_.left;
    for ( ; point != end_ptr; point += point_size_in_bytes, ++index )
    {
        uint32_t& key = * reinterpret_cast< uint32_t* >( point + _offset );
        if ( key & msb_only )
        {
            // invert bitmask, float number is negative
            // std::cout << "pre_invert  " << index << " " << _debug_print_mask( key ) << std::endl;
            key = ~ key;
            // std::cout << "post_invert " << index << " " << _debug_print_mask( key ) << std::endl;
        }
        else
        {
            // flip msb only, float number is 0 or positive
            // std::cout << "pre_flip  " << index << " " << _debug_print_mask( key ) << std::endl;
            key = key | min_;
            // std::cout << "post_flip " << index << " " << _debug_print_mask( key ) << std::endl;
        }
    
    }
}



void
sort_key_transform::transform_back( const sort_range& range_ )
{
    //std::cout << "reversing key transform on points " 
    //    << range_.left << "-" << range_.right << std::endl;

    const point_info& point_info_       = _mapped_point_data.get_point_info();
    const size_t point_size_in_bytes    = point_info_.get_point_size_in_bytes();
    const size_t point_count            = point_info_.get_point_count();
    
    char* base_ptr  = _mapped_point_data.get_data_ptr();
    char* begin_ptr = base_ptr + range_.left * point_size_in_bytes;
    char* end_ptr   = base_ptr + ( range_.right + 1 ) * point_size_in_bytes; 
       
    const uint32_t msb_only = 1u << 31;
    const uint32_t& min_   = (uint32_t) std::numeric_limits< int32_t >::min();
    
    for ( char* point = begin_ptr; point != end_ptr; point += point_size_in_bytes )
    {
        uint32_t& key = * reinterpret_cast< uint32_t* >( point + _offset );
        if ( key & msb_only )
        {
            // flip msb only
            // std::cout << "pre_flip  " << _debug_print_mask( key ) << std::endl;
            key = key ^ min_;
            // std::cout << "post_flip " << _debug_print_mask( key ) << std::endl;
        }
        else
        {
            // invert bitmask
            // std::cout << "pre_invert  " << _debug_print_mask( key ) << std::endl;
            key = ~ key;
            // std::cout << "post_invert " << _debug_print_mask( key ) << std::endl;
        }
    
    }
}



std::string
sort_key_transform::_debug_print_mask( const uint32_t& bitmask ) const
{
    std::string out = "";
    for( size_t i = 0; i < 32; ++i )
    {
        if ( bitmask & 1u << i )
            out = "1" + out;
        else
            out = "0" + out;
    }
    return out;
}



} // namespace stream_process

