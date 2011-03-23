#ifndef __STREAM_PROCESS__REINDEX_MAP__HPP__
#define __STREAM_PROCESS__REINDEX_MAP__HPP__

#include <boost/iostreams/device/mapped_file.hpp>

namespace stream_process
{

template< typename T >
class reindex_map
{
public:
    typedef T     value_type;

    void open( const std::string& filename, size_t max_number_of_elements );
    bool is_open() const;
    void close();

    inline const value_type& operator[]( size_t old_index );
    inline const value_type& get( size_t old_index );

    void set_and_fill_gaps( value_type old_index, value_type new_index );
    void set( value_type old_index, value_type new_index );
    
protected:
    boost::iostreams::mapped_file   _mapped_file;

    T*      _begin;
    size_t  _next_index;

}; // class reindex_map



template< typename T >
void
reindex_map< T >::
open( const std::string& filename, size_t max_number_of_elements )
{
    boost::iostreams::mapped_file_params params;
    params.path             = filename;
    params.mode             = std::ios_base::in | std::ios_base::out;
    params.new_file_size    = max_number_of_elements * sizeof( T );
    
    _mapped_file.open( params );
    
    _begin = reinterpret_cast< T* >( _mapped_file.data() );
    _next_index = 0;
}



template< typename T >
void
reindex_map< T >::
set_and_fill_gaps( value_type old_index, value_type new_index )
{
    if ( _next_index > old_index )
    {
        _next_index = old_index;
    }

    while( _next_index < old_index )
    {
        _begin[ _next_index ] = _begin[ _next_index - 1 ];
        ++_next_index;
    }
    
    _begin[ old_index ] = new_index;
    ++_next_index;
}



template< typename T >
void
reindex_map< T >::
set( T old_index, T new_index )
{
    _begin[ old_index ] = new_index;
}



template< typename T >
inline const T&
reindex_map< T >::
get( size_t old_index )
{
    return _begin[ old_index ];
}


template< typename T >
inline const T&
reindex_map< T >::
operator[]( size_t old_index )
{
    return _begin[ old_index ];
}



template< typename T >
bool
reindex_map< T >::
is_open() const
{
    return _mapped_file.is_open();
}


template< typename T >
void
reindex_map< T >::
close()
{
    _mapped_file.close();
}

} // namespace stream_process

#endif

