#ifndef __STREAM_PROCESS__ELEMENT_READER__HPP__
#define __STREAM_PROCESS__ELEMENT_READER__HPP__

#include <stream_process/stream_process_types.hpp>

#include <stream_process/data_element.hpp>

namespace stream_process
{

template< typename sp_types_t >
class element_reader
{
public:
    STREAM_PROCESS_TYPES

    typedef input_converter< sp_types_t >   input_cv_t;
    
    element_reader( data_element& data_element_, const std::string& filename_base );
    
    char* data();
    const char* data() const;
    size_t increment( size_t increment_ );

    // FIXME const
    char*   next();
    void    unread_last(); // resets ptrs so next will return the same element again
    
    bool    has_more()
    {
        return _number_of_elements_read < _number_of_elements;
    }
    
protected:
    data_element&           _data_element;
    mapped_data_element     _mapped_data_element;
    
    char*           _in_data;

    size_t          _size_in_bytes;

    size_t          _number_of_elements;
    size_t          _number_of_elements_read;
    
    
}; // class element_reader



template< typename sp_types_t >
element_reader< sp_types_t >::
element_reader( data_element& data_element_, const std::string& filename_base )
    : _data_element( data_element_ )
    , _mapped_data_element( data_element_, filename_base )
    , _in_data( _mapped_data_element.data() )
    , _size_in_bytes( _data_element.get_size_in_bytes() )
    , _number_of_elements( _data_element.get_size() )
    , _number_of_elements_read( 0 )
{}



template< typename sp_types_t >
size_t
element_reader< sp_types_t >::
increment( size_t increment_ )
{
    if ( _number_of_elements_read + increment_ > _number_of_elements )
    {
        increment_ = _number_of_elements - _number_of_elements_read;
        _number_of_elements_read = _number_of_elements;
        _in_data += increment_ * _size_in_bytes;
    }
    else
    {
        _number_of_elements_read += increment_;
        _in_data += increment_ * _size_in_bytes;
    }
    return increment_;
}



template< typename sp_types_t >
char*
element_reader< sp_types_t >::
data()
{
    assert( _in_data );
    return _in_data;
}



template< typename sp_types_t >
const char*
element_reader< sp_types_t >::
data() const
{
    assert( _in_data );
    return _in_data;
}



template< typename sp_types_t >
char*
element_reader< sp_types_t >::
next()
{
    assert( _in_data );
    assert( _number_of_elements_read < _number_of_elements );
    char* data = _in_data;
    _in_data += _size_in_bytes;
    ++_number_of_elements_read;
    return data;
}



template< typename sp_types_t >
void
element_reader< sp_types_t >::
unread_last()
{
    _in_data  -= _size_in_bytes;
}

} // namespace stream_process

#endif

