#ifndef __STREAM_PROCESS__ELEMENT_WRITER__HPP__
#define __STREAM_PROCESS__ELEMENT_WRITER__HPP__

#include <stream_process/stream_process_types.hpp>

#include <stream_process/data_element.hpp>

namespace stream_process
{

template< typename sp_types_t >
class element_writer
{
public:
    STREAM_PROCESS_TYPES

    element_writer( data_element& data_element_,
        const std::string& filename );
    ~element_writer();
    
    void setup();

    void write( const typename slice_type::container_type& stream_ );
    
    size_t get_number_of_elements_written()
    {
        return _number_of_elements_written;
    }

protected:
    data_element&   _data_element;
    std::ofstream   _out_stream;
    
    size_t          _next_slice_number;
    size_t          _number_of_elements_written;
    size_t          _size_in_bytes;
    
}; // class element_writer


template< typename sp_types_t >
element_writer< sp_types_t >::
element_writer( data_element& data_element_, const std::string& filename )
    : _data_element( data_element_ )
    , _next_slice_number( 0 )
    , _number_of_elements_written( 0 )
    , _size_in_bytes( 0 )
{
    _out_stream.open( filename.c_str() );

    if ( ! _out_stream.is_open() )
    {
        throw exception(
            std::string( "opening output file " ) + filename 
            + " failed.",
            SPROCESS_HERE 
            );
    }
}



template< typename sp_types_t >
element_writer< sp_types_t >::
~element_writer()
{
    _out_stream.close();
    assert( ! _out_stream.bad() );
}




template< typename sp_types_t >
void
element_writer< sp_types_t >::
setup()
{
    _size_in_bytes = _data_element.get_structure().compute_out_size_in_bytes();
}


template< typename sp_types_t >
void
element_writer< sp_types_t >::
write( const typename slice_type::container_type& stream_ )
{
    typename slice_type::const_iterator 
        it = stream_.begin(), it_end = stream_.end();
    for( ; it != it_end; ++it )
    {
        _out_stream.write( (const char*) *it, _size_in_bytes );
        ++_number_of_elements_written;
    
        assert( _out_stream.good() );
    }
}


} // namespace stream_process

#endif

