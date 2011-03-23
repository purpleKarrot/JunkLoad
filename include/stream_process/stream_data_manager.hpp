#ifndef __STREAM_PROCESS__STREAM_DATA_MANAGER__HPP__
#define __STREAM_PROCESS__STREAM_DATA_MANAGER__HPP__

#include <stream_process/stream_process_types.hpp>

#include <boost/pool/pool.hpp>
#include <boost/thread.hpp>

namespace stream_process
{

template< typename sp_types_t >
class stream_data_manager : public sp_types_t
{
public:
    STREAM_PROCESS_TYPES 

    typedef boost::mutex::scoped_lock   scoped_lock;

    stream_data_manager();
    stream_data_manager( const stream_data_manager& sdm );
    const stream_data_manager& operator=( const stream_data_manager& sdm );
    
    inline stream_data* create();
    inline void destroy( stream_data* stream_data_ );

    inline void create( slice_type* slice, size_t size_ );
    inline void destroy( slice_type* slice );
    
    void init( size_t stream_data_size_in_bytes );

protected:
    boost::pool<>*          _factory;
    size_t                  _size_in_bytes;

	mutable boost::mutex    _mutex;

}; // class stream_data_manager



template< typename sp_types_t >
stream_data_manager< sp_types_t >::
stream_data_manager()
    : _factory( 0 )
    , _size_in_bytes( 0 )
    , _mutex()
{}


template< typename sp_types_t >
stream_data_manager< sp_types_t >::
stream_data_manager( const stream_data_manager& sdm )
    : _factory( sdm._factory )
    , _size_in_bytes( sdm._size_in_bytes  )
    , _mutex()
{}


template< typename sp_types_t >
const stream_data_manager< sp_types_t >&
stream_data_manager< sp_types_t >::
operator=( const stream_data_manager& sdm )
{
    init( sdm._size_in_bytes );
    return *this;
}


template< typename sp_types_t >
inline stream_data*
stream_data_manager< sp_types_t >::
create()
{
    assert( _factory );
    scoped_lock lock( _mutex );
    return reinterpret_cast< stream_data* >( _factory->malloc() );
}



template< typename sp_types_t >
inline void
stream_data_manager< sp_types_t >::
destroy( stream_data* stream_data_ )
{
    assert( _factory );
    scoped_lock lock( _mutex );
    _factory->free( stream_data_ );
}


template< typename sp_types_t >
void
stream_data_manager< sp_types_t >::
create( slice_type* slice, size_t size_ )
{
    assert( _factory );

    scoped_lock lock( _mutex );

    char* data_block
        = reinterpret_cast< char* >( _factory->ordered_malloc( size_ ) );
    
    slice->resize( size_ );

    typename slice_type::iterator it = slice->begin(), it_end = slice->end();
    for( char* ptr = data_block; it != it_end; ++it, ptr += _size_in_bytes )
    {
        *it = reinterpret_cast< stream_data* >( ptr );
    }
}



template< typename sp_types_t >
inline void
stream_data_manager< sp_types_t >::
destroy( slice_type* slice )
{
    assert( _factory );
    assert( slice );

    scoped_lock lock( _mutex );
    
    typename slice_type::iterator 
        it      = slice->begin(),
        it_end  = slice->end();
    for( ; it != it_end; ++it )
    {
        _factory->free( *it );
    }
}




template< typename sp_types_t >
void
stream_data_manager< sp_types_t >::
init( size_t stream_data_size_in_bytes )
{
    assert( ! _factory );
    assert( _size_in_bytes == 0 );

    _size_in_bytes  = stream_data_size_in_bytes;
    _factory = new boost::pool<>( stream_data_size_in_bytes );
}


} // namespace stream_process

#endif

