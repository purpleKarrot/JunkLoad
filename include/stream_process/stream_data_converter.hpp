#ifndef __STREAM_PROCESS__STREAM_DATA_CONVERTER__HPP__
#define __STREAM_PROCESS__STREAM_DATA_CONVERTER__HPP__


#include <stream_process/stream_structure.hpp>
#include <stream_process/stream_data_conversion_helpers.hpp>

#include <vector>

namespace stream_process
{


class stream_data_converter
{
public:
    stream_data_converter( const stream_structure& in_, const stream_structure& out_ );

    // convert single point
    void convert( const char* in_ptr, char* out_ptr ) const;
    
    // convert batch
    void convert( 
        stream_data_const_iterator in_begin_,
        stream_data_const_iterator in_end_,
        stream_data_iterator out_begin_,
        stream_data_iterator out_end_ 
        ) const;

    // convert batch
    void convert( 
        const char* in_begin_, 
        const char*  in_end_, 
        size_t in_element_size_in_bytes,
        char* out_begin_,
        char* out_end_, 
        size_t out_element_size_in_bytes ) const;


    
protected:
    void    _setup();
    bool    _input_has_all_required_outputs();
    
    void add_converter( const attribute& in_, const attribute& out_ );

    template< typename Tin >
    void add_converter( const attribute& in_, const attribute& out_ );
    

    const stream_structure&     _in;
    const stream_structure&     _out;
    
    const size_t                _in_size_in_bytes;
    const size_t                _out_size_in_bytes;

    std::vector< converter* >                                       _converters;

}; // class stream_data_converter



template< typename Tin >
void
stream_data_converter::add_converter( const attribute& in_, const attribute& out_ )
{
    converter* cv = 0;
    switch( out_.get_data_type_id() )
    {
        case SP_FLOAT_32:
            cv = new converter_t< Tin, float >( in_, out_ );
            break;
        case SP_FLOAT_64:
            cv = new converter_t< Tin, double >( in_, out_ );
            break;
        case SP_INT_8:
            cv = new converter_t< Tin, int8_t >( in_, out_ );
            break;
        case SP_UINT_8:
            cv = new converter_t< Tin, uint8_t >( in_, out_ );
            break;
        case SP_INT_16:
            cv = new converter_t< Tin, int16_t >( in_, out_ );
            break;
        case SP_UINT_16:
            cv = new converter_t< Tin, uint16_t >( in_, out_ );
            break;
        case SP_INT_32:
            cv = new converter_t< Tin, int32_t >( in_, out_ );
            break;
        case SP_UINT_32:
            cv = new converter_t< Tin, uint32_t >( in_, out_ );
            break;
        case SP_INT_64:
            cv = new converter_t< Tin, int64_t >( in_, out_ );
            break;
        case SP_UINT_64:
            cv = new converter_t< Tin, uint64_t >( in_, out_ );
            break;
        default:
            throw exception(
                "invalid type conversion request.", 
                SPROCESS_HERE
            );
            break;
    }
    if ( cv != 0 )
        _converters.push_back( cv );
}





} // namespace stream_process

#endif

