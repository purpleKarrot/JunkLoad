#ifndef __STREAM_PROCESS__STREAM_DATA_CONVERTER_HELPERS__HPP__
#define __STREAM_PROCESS__STREAM_DATA_CONVERTER_HELPERS__HPP__

#include <boost/numeric/conversion/converter.hpp>

#include <stream_process/stream_data_iterators.hpp>

namespace stream_process
{

class converter
{
public:
    converter( const attribute& in_, const attribute& out_ )
        : _in( in_ )
        , _out( out_ )
    {}
    virtual ~converter() {}

    virtual void operator()( const char* in_ptr, char* out_ptr ) const = 0;
    
    virtual void operator()( 
        stream_data_const_iterator in_begin_,
        stream_data_const_iterator in_end_,
        stream_data_iterator out_begin_,
        stream_data_iterator out_end_ 
        ) = 0;

    virtual void operator()( 
        const char* in_begin_,
        const char*  in_end_,
        size_t in_element_size_in_bytes,
        char* out_begin_,
        char* out_end_,
        size_t out_element_size_in_bytes
        )
    {
        if ( in_begin_ == 0 ||
            in_end_ == 0 ||
            out_begin_ == 0 ||
            out_end_ == 0 )
        {
            throw exception( "attempt to copy from/to nullpointer.",
                SPROCESS_HERE );
        }

        stream_data_const_iterator ib( in_begin_, in_element_size_in_bytes );
        stream_data_const_iterator ie( in_end_, in_element_size_in_bytes );
        stream_data_iterator ob( out_begin_, out_element_size_in_bytes );
        stream_data_iterator oe( out_end_, out_element_size_in_bytes );
        operator()( ib, ie, ob, oe );
    }

protected:
    const attribute&    _in;
    const attribute&    _out;
};



class copy_converter : public converter
{
public:
    copy_converter( const attribute& in_, const attribute& out_ )
        : converter( in_, out_ )
    {}
    
    virtual void operator()( const char* in_ptr, char* out_ptr ) const
    {
        const char* in_start    = in_ptr + _in.get_offset();
        char* out_start         = out_ptr + _out.get_offset();
        memcpy( out_start, in_start, _out.get_size_in_bytes() );
    }

    virtual void operator()( 
        stream_data_const_iterator in_begin_,
        stream_data_const_iterator in_end_,
        stream_data_iterator out_begin_,
        stream_data_iterator out_end_ 
        )
    {
        stream_data_const_iterator in_it    = in_begin_;
        stream_data_iterator out_it         = out_begin_;
        
        
        for( ; in_it != in_end_ && out_it != out_end_; ++in_it, ++out_it )
        {

            const stream_data* in = (*in_it) + _in.get_offset();
            stream_data* out = (*out_it) + _out.get_offset();
            memcpy( out, in, _out.get_size_in_bytes() );
        }
        
        if ( in_it != in_end_ || out_it != out_end_ )
        {
            throw exception( "mismatch while batch-converting data", 
                SPROCESS_HERE );
        }
    
    }
    
    virtual void operator()( 
        const char* in_begin_,
        const char*  in_end_,
        size_t in_element_size_in_bytes,
        char* out_begin_,
        char* out_end_,
        size_t out_element_size_in_bytes
        )
    {
        converter::operator()( in_begin_, in_end_, in_element_size_in_bytes,
            out_begin_, out_end_, out_element_size_in_bytes );
    }
    
};



template< typename Tin, typename Tout >
class converter_t : public converter
{
public:
    converter_t( const attribute& in_, const attribute& out_ )
        : converter( in_, out_ )
    {}
    
    virtual void operator()( const char* in_ptr, char* out_ptr ) const
    {
        const Tin& tin  = *reinterpret_cast< const Tin* >( in_ptr + _in.get_offset() );
        Tout& tout      = *reinterpret_cast< Tout* >( out_ptr + _out.get_offset() );
        
        tout = _convert( tin );
    }

    virtual void operator()( 
        stream_data_const_iterator in_begin_,
        stream_data_const_iterator in_end_,
        stream_data_iterator out_begin_,
        stream_data_iterator out_end_ 
        )
    {
        stream_data_const_iterator in_it    = in_begin_;
        stream_data_iterator out_it         = out_begin_;
        
        for( ; in_it != in_end_ && out_it != out_end_; ++in_it, ++out_it )
        {
            const stream_data* in = (*in_it) + _in.get_offset();
            stream_data* out = (*out_it) + _out.get_offset();
            const Tin& tin  = 
                *reinterpret_cast< const Tin* >( in + _in.get_offset() );
            Tout& tout      = 
                *reinterpret_cast< Tout* >( out + _out.get_offset() );
        }
        
        if ( in_it != in_end_ || out_it != out_end_ )
        {
            throw exception( "mismatch while batch-converting data", 
                SPROCESS_HERE );
        }
    
    }

    virtual void operator()( 
        const char* in_begin_,
        const char*  in_end_,
        size_t in_element_size_in_bytes,
        char* out_begin_,
        char* out_end_,
        size_t out_element_size_in_bytes
        )
    {
        converter::operator()( in_begin_, in_end_, in_element_size_in_bytes,
            out_begin_, out_end_, out_element_size_in_bytes );
    }

protected:
    boost::numeric::converter< Tin, Tout >  _convert;
};






} // namespace stream_process

#endif

