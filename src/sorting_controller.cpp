#include "sorting_controller.hpp"

#include "radix_sort_worker.hpp"
#include "insertion_sort_worker.hpp"
#include "sort_key_transform.hpp"

#include "sort_worker.hpp"


namespace stream_process
{
sorting_controller::sorting_controller( mapped_point_data& mapped_point_data_, 
    size_t max_thread_count_ )
    : _mapped_point_data( mapped_point_data_ )
    , _point_info( mapped_point_data_.get_point_info() )
    , _max_thread_count( max_thread_count_ )
    , _min_unsorted_index( 0 )
    , _insertion_sort_max_size( 20 )
{
    worker_thread< sorting_controller >::set_controller( this );
}



sorting_controller::~sorting_controller()
{
    if ( ! _threads.empty() )
    {
        std::list< boost::thread* >::iterator it        = _threads.begin();
        std::list< boost::thread* >::iterator it_end    = _threads.end();
        for( ; it != it_end; ++it )
        {
            delete *it;
        }
        _threads.clear();
    }
}



void
sorting_controller::setup()
{
    const rt_struct_member_info& info 
        = _point_info.get_attribute_info( "position" );

    // we sort according to the z axis
    size_t offset = info.input_offset + 2 * sizeof( float );
    //std::cout << "offset " << offset << std::endl;

    sort_key_transform key_transformer( _mapped_point_data, offset );
    
    // transform all keys for sorting
    key_transformer.transform();

    // add 'all' task to todo pq
    sort_range range_ = sort_range( 0, _point_info.get_point_count() - 1 );
    range_.sorting_bit = 31;
    _radix_sort_todo_queue.push( range_ );

}



void
sorting_controller::sort_multi_threaded()
{    
    const rt_struct_member_info& info 
        = _point_info.get_attribute_info( "position" );

    // we sort according to the z axis
    size_t offset = info.input_offset + 2 * sizeof( float );

    sort_worker worker( _mapped_point_data, offset );
    for( size_t i = 0; i < _max_thread_count; ++i )
    {
        boost::thread* thread_ = new boost::thread( worker );
        _threads.push_back( thread_ );
    }

}



// single-threaded sorting
void
sorting_controller::sort()
{
   const rt_struct_member_info& info 
        = _point_info.get_attribute_info( "position" );

    // we sort according to the z axis
    size_t offset = info.input_offset + 2 * sizeof( float );

    sort_worker worker( _mapped_point_data,  offset );
    worker.sort_single_threaded();
}



size_t
sorting_controller::get_new_job( sort_worker& worker )
{
    boost::mutex::scoped_lock lock( _radix_sort_queue_mutex );
    
    // retrieve results
    std::list< sort_range >& sorted_ranges = worker.get_sorted_ranges();
    while ( ! sorted_ranges.empty() )
    {
        _done_queue.push( sorted_ranges.front() );
        sorted_ranges.pop_front();
    }
    _check_done_queue();
    std::list< sort_range >& partially_sorted_ranges 
        = worker.get_partially_sorted_ranges();
    while ( ! partially_sorted_ranges.empty() )
    {
        _radix_sort_todo_queue.push( partially_sorted_ranges.front() );
        partially_sorted_ranges.pop_front();
    }
    
    // assign new job
    if ( ! _radix_sort_todo_queue.empty() )
    {
        worker.add_sort_range( _radix_sort_todo_queue.top() );
        _radix_sort_todo_queue.pop();
        return 1;
    }

    if ( _min_unsorted_index != _point_info.get_point_count() )
    {   
        return 4; 
    }

    return 0;
}



/*
void
radix_sort_controller::add_radix_sort_job( const sort_range& range_ )
{
    boost::mutex::scoped_lock lock( _radix_sort_queue_mutex );
    _radix_sort_todo_queue.push( range_ );    
}


void
radix_sort_controller::notify_range_finished( const sort_range& range_ )
{
    boost::mutex::scoped_lock done_lock( _radix_sort_queue_mutex );
    _done_queue.push( range_ );
}



void
radix_sort_controller::notify_ranges_finished( sort_range_priority_queue& queue_ )
{
    boost::mutex::scoped_lock done_lock( _radix_sort_queue_mutex );
    while ( ! queue_.empty() )
    {
        _done_queue.push( queue_.top() );
        queue_.pop();
    }
}
*/

bool
sorting_controller::_check_done_queue()
{
    bool stop = false;
    while ( ! _done_queue.empty() && ! stop )
    {
        const sort_range& range_ = _done_queue.top();
        if ( range_.left == _min_unsorted_index )
        {
            _min_unsorted_index = range_.right + 1;
            _done_queue.pop();
        }
        else
            stop = true;
    
    }
    return false;
}



size_t
sorting_controller::compute_and_get_min_unsorted_index()
{
    // because check_done accesses the done queue
    boost::mutex::scoped_lock lock( _radix_sort_queue_mutex );
    _check_done_queue();
    return _min_unsorted_index;
}


} // namespace stream_process

