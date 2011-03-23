#include <stream_process/stream_data_converter.hpp>

namespace stream_process
{

stream_data_converter::stream_data_converter(
    const stream_structure& in_,
    const stream_structure& out_ )
    : _in( in_ )
    , _out( out_ )
    , _in_size_in_bytes( _in.compute_size_in_bytes() )
    , _out_size_in_bytes( _out.compute_size_in_bytes() )
{
    _setup();
}



// convert single point
void
stream_data_converter::convert( const char* in_ptr, char* out_ptr ) const
{
    std::vector< converter* >::const_iterator
        it      = _converters.begin(),
        it_end  = _converters.end();
    for( ; it != it_end; ++it )
    {
        converter& cv = **it;
        cv( in_ptr, out_ptr );
    }
}




// convert batch
void
stream_data_converter::convert( 
    stream_data_const_iterator in_begin_,
    stream_data_const_iterator in_end_,
    stream_data_iterator out_begin_,
    stream_data_iterator out_end_ 
    ) const
{
    std::vector< converter* >::const_iterator
        it      = _converters.begin(),
        it_end  = _converters.end();
    for( ; it != it_end; ++it )
    {
        converter& cv = **it;
        cv( in_begin_, in_end_, out_begin_, out_end_ );
    }
}



// convert batch
void
stream_data_converter::convert( 
    const char* in_begin_, const char*  in_end_, size_t in_element_size_in_bytes,
    char* out_begin_, char* out_end_, size_t out_element_size_in_bytes ) const
{
    std::vector< converter* >::const_iterator
        it      = _converters.begin(),
        it_end  = _converters.end();
    for( ; it != it_end; ++it )
    {
        converter& cv = **it;
        cv( in_begin_, in_end_, in_element_size_in_bytes, 
            out_begin_, out_end_, out_element_size_in_bytes );
    }
}




void
stream_data_converter::_setup()
{
    if ( ! _input_has_all_required_outputs() )
    {
        throw exception( "attempt to convert from stream_data with missing attributes",
            SPROCESS_HERE );
    }

    stream_structure::const_iterator 
        it      = _out.begin(),
        it_end  = _out.end();
    for( ; it != it_end; ++it )
    {
        const attribute& oattr = **it;
        const attribute& iattr = _in.get_attribute( oattr.get_name() );
        add_converter( iattr, oattr );
    }

}




bool
stream_data_converter::_input_has_all_required_outputs()
{
    stream_structure::const_iterator 
        it      = _out.begin(),
        it_end  = _out.end();
    for( ; it != it_end; ++it )
    {
        const attribute& oattr = **it;
        if ( ! _in.has_attribute( oattr.get_name(), oattr.get_data_type_id(),
            oattr.get_number_of_elements() ) )
        {
            std::cout << "missing attribute: " << oattr.get_name() << std::endl;
            return false;
        }
    }
    return true;
}



void
stream_data_converter::add_converter( const attribute& in_, const attribute& out_ )
{
    if ( in_.get_data_type_id() == out_.get_data_type_id() )
    {
        copy_converter* cc = new copy_converter( in_, out_ );
        _converters.push_back( cc );
        return;
    }

    switch( in_.get_data_type_id() )
    {
        case SP_FLOAT_32:
            add_converter< const float >( in_, out_ );
            break;
        case SP_FLOAT_64:
            add_converter< const double >( in_, out_ );
            break;
        case SP_INT_8:
            add_converter< const int8_t >( in_, out_ );
            break;
        case SP_UINT_8:
            add_converter< const uint8_t >( in_, out_ );
            break;
        case SP_INT_16:
            add_converter< const int16_t >( in_, out_ );
            break;
        case SP_UINT_16:
            add_converter< const uint16_t >( in_, out_ );
            break;
        case SP_INT_32:
            add_converter< const int32_t >( in_, out_ );
            break;
        case SP_UINT_32:
            add_converter< const uint32_t >( in_, out_ );
            break;
        case SP_INT_64:
            add_converter< const int64_t >( in_, out_ );
            break;
        case SP_UINT_64:
            add_converter< const uint64_t >( in_, out_ );
            break;
        default:
            throw exception(
                "invalid type conversion request.", 
                SPROCESS_HERE
            );
            break;
    }

}



} // namespace stream_process

