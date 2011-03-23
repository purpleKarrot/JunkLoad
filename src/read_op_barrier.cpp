#include "read_op_barrier.hpp"

namespace stream_process
{
read_op_barrier::read_op_barrier( uint32_t& index_ ) 
    : _index( index_ )
    , _min_unsorted_index( 0 )
{}


read_op_barrier::~read_op_barrier()
{}


empty_read_op_barrier::empty_read_op_barrier( uint32_t& index_ )
    : read_op_barrier( index_ )
{}
    

preprocess_aware_read_op_barrier::preprocess_aware_read_op_barrier( 
    preprocess_shared_data& preprocess_shared_data_, uint32_t& index_ )
    : read_op_barrier( index_ )
    , _preprocess_shared_data( preprocess_shared_data_ )
    , _wait_duration( 250 )
{}



void
preprocess_aware_read_op_barrier::check()
{
    if ( _min_unsorted_index <= _index )
    {
        // we only update the shared var if we have to, because thread
        // synching is slooow
        _min_unsorted_index = _preprocess_shared_data.get_min_unsorted_index();     

        // if the preprocess is still not done, we sleep and wait until 
        // more points are available
        while ( _min_unsorted_index <= _index )
        {
            usleep( _wait_duration );       
            _min_unsorted_index = _preprocess_shared_data.get_min_unsorted_index();
        }
    }
}


} // namespace stream_process

