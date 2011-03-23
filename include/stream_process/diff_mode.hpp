#ifndef __STREAM_PROCESS__DIFF__HPP__
#define __STREAM_PROCESS__DIFF__HPP__

#include <stream_process/point_set.hpp>
#include <stream_process/options_map.hpp>

#include <stream_process/attribute_helper.hpp>

#include <iostream>

namespace stream_process
{

class diff_mode
{
public:
    diff_mode( const options_map& omap );

protected:
    void    print_usage();
    
    inline bool are_equal( const std::string& name, size_t index ) const;
    
    template< typename T >
    inline bool are_equal_t( size_t index, size_t offset0, size_t offset1, size_t array_size ) const;

    const options_map&  _options;
    point_set           _point_sets[ 2 ];
    
}; // class diff



inline bool
diff_mode::are_equal( const std::string& name, size_t index ) const
{
    const stream_data_structure& ps0
        = _point_sets[ 0 ].get_header().get_vertex_structure();

    const stream_data_structure& ps1
        = _point_sets[ 1 ].get_header().get_vertex_structure();
    
    const attribute& attr0 = ps0.get_attribute( name );
    const attribute& attr1 = ps0.get_attribute( name );
    
    size_t array_size = attr0.get_number_of_elements();
    
    size_t offset0 = attr0.get_offset();
    size_t offset1 = attr1.get_offset();
    
    attribute_helper helper;
    
    switch( attr0.get_data_type_id() )
    {
        case SP_FLOAT_32:
            return are_equal_t< float >( index, offset0, offset1, array_size );
        case SP_FLOAT_64:
            return are_equal_t< double >( index, offset0, offset1, array_size );
        case SP_UINT_8:
            return are_equal_t< uint8_t >( index, offset0, offset1, array_size );
        case SP_INT_8:
            return are_equal_t< int8_t >( index, offset0, offset1, array_size );
        case SP_UINT_16:
            return are_equal_t< uint16_t >( index, offset0, offset1, array_size );
        case SP_INT_16:
            return are_equal_t< int16_t >( index, offset0, offset1, array_size );
        case SP_UINT_32:
            return are_equal_t< uint32_t >( index, offset0, offset1, array_size );
        case SP_INT_32:
            return are_equal_t< int32_t >( index, offset0, offset1, array_size );
        case SP_UINT_64:
            return are_equal_t< uint64_t >( index, offset0, offset1, array_size );
        case SP_INT_64:
            return are_equal_t< int64_t >( index, offset0, offset1, array_size );
        case SP_UNKNOWN_DATA_TYPE:
            return true;
        default:
            throw exception( "error while comparing attributes", SPROCESS_HERE );
            return false;
    }
    
    return true;
}



template< typename T >
inline bool
diff_mode::are_equal_t( size_t index, size_t offset0, size_t offset1, size_t array_size ) const
{
    const stream_data* s0 = _point_sets[ 0 ][ index ];
    const stream_data* s1 = _point_sets[ 1 ][ index ];
    
    attribute_accessor< T > get_attribute;

    bool are_equal_ = true;
    size_t array_offset = 0;
    for( size_t index = 0; index < array_size; ++index )
    {
        array_offset = index * sizeof( T );
        
        get_attribute.set_offset( offset0 +array_offset );
        const T& t0 = get_attribute( s0 );
        
        get_attribute.set_offset( offset1 + array_offset );
        const T& t1 = get_attribute( s1 );
        
        if ( t0 != t1 )
            return false;
    }
    return true;
}



} // namespace stream_process

#endif

