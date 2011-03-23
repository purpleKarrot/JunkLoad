#ifndef __STREAM_PROCESS__IO_SHARED_DATA__HPP__
#define __STREAM_PROCESS__IO_SHARED_DATA__HPP__

#include <stream_process/stream_data.hpp>
#include <stream_process/slice_manager.hpp>

namespace stream_process
{
template< typename sp_types_t >
class io_shared_data : public slice_manager< sp_types_t >
{
public:
    typedef slice_manager< sp_types_t >         super;
    typedef typename sp_types_t::data_slice_t   data_slice_t;

    io_shared_data()
        : super()
        , _stream_size_in_bytes( 0 )
        , _out_size_in_bytes( 0 )
    {}
    
    ~io_shared_data()
    {}

    void setup( size_t stream_size_, size_t out_size_, size_t batch_size )
    {
        _stream_size_in_bytes   = stream_size_;
        _out_size_in_bytes      = out_size_; 
        super::init( _stream_size_in_bytes, batch_size );
    }
    
protected:
    size_t          _stream_size_in_bytes;
    size_t          _out_size_in_bytes;

}; // class io_shared_data

} // namespace stream_process

#endif

