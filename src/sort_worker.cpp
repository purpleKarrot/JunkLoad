#include "sort_worker.hpp"

#include "radix_sort_worker.hpp"
#include "insertion_sort_worker.hpp"
#include "sort_key_transform.hpp"

#include "exception.hpp"

namespace stream_process
{



sort_worker::sort_worker( mapped_point_data& mapped_point_data_, size_t offset )
    : _radix_sort( new radix_sort_worker( mapped_point_data_, offset ) )
    , _insertion_sort( new insertion_sort_worker( mapped_point_data_, offset ) )
    , _key_transform( new sort_key_transform( mapped_point_data_, offset ) )
    , _sleep_duration( 25 )
{}



sort_worker::sort_worker( const sort_worker& source_ )
    : _radix_sort( new radix_sort_worker( *source_._radix_sort ) )
    , _insertion_sort( new insertion_sort_worker( *source_._insertion_sort ) )
    , _key_transform( new sort_key_transform( *source_._key_transform ) )
    , _sort_range( source_._sort_range )
    , _todos( source_._todos )
    , _partially_sorted( source_._partially_sorted )
    , _sorted( source_._sorted )
    , _sleep_duration( source_._sleep_duration )
{}



sort_worker::~sort_worker()
{
    delete _radix_sort;
    delete _insertion_sort;
    delete _key_transform;
}



void
sort_worker::sort()
{
    size_t job_type;
    while( ( job_type = _controller->get_new_job( *this ) ) )
    {
        if ( job_type == 4 )
            usleep( _sleep_duration );
        else
            while( ! _todos.empty() )
            {
                _sort_next();
            }
    }
    // we're done
}



void
sort_worker::_sort_next()
{
    const sort_range& range_ = _todos.front();
    set_sort_range( range_ );
    _todos.pop_front();
    if ( range_.size() > 25 )
        radix_sort();
    else
        insertion_sort();
}


void
sort_worker::sort_single_threaded()
{
    size_t job_type;
    while( ( job_type = _controller->get_new_job( *this ) ) )
    {
        while( ! _todos.empty() )
        {
            _sort_next();
        }
    }
    // we're done
}



void
sort_worker::add_sort_range( const sort_range& sort_range_ )
{
    _todos.push_back( sort_range_ );
}



void
sort_worker::set_sort_range( const sort_range& sort_range_ )
{
    _sort_range = sort_range_;
}


void
sort_worker::_finished_radix_sort( const sort_range& sort_range_ )
{
    if ( sort_range_.sorting_bit > 31 )
    {
        reverse_key_transform();
    }
    // we keep it in this threads own list to avoid costly 
    // locking for small sort jobs
    else if ( sort_range_.size() < 1e4 )
    {
        _todos.push_back( sort_range_ );
    }
    else
        _partially_sorted.push_back( sort_range_ );
    // _controller->add_radix_sort_job( sort_range_ );
}



void
sort_worker::radix_sort()
{
    _radix_sort->setup( _sort_range );
    _radix_sort->sort();
    
    sort_range left, right;
    _radix_sort->get_results( left, right );
    
    if ( left.left != right.left )
        _finished_radix_sort( right );
    _finished_radix_sort( left );

/*
    _radix_sort->setup( _sort_range );
    _radix_sort->sort();
    //_controller->range_is_radix_sorted( _radix_sort->get_left_range(), 
    //    _radix_sort->get_right_range() );
    sort_range left, right;
    _radix_sort->get_results( left, right );

    if ( left.left != right.left )
    {
        _controller->add_radix_sort_job( right );
    }
    set_sort_range( left );
    if ( left.size() > 25 )
    {
        if ( left.sorting_bit < 31 )
        {
            //_controller->add_radix_sort_job( right );
            radix_sort();
        }
        else
            reverse_key_transform();
    }
    else 
    {
        insertion_sort();
    }
*/
}



void
sort_worker::insertion_sort()
{
    _insertion_sort->setup( _sort_range );
    _insertion_sort->sort();
    reverse_key_transform();
}



void
sort_worker::reverse_key_transform()
{
    _key_transform->transform_back( _sort_range );
    //_controller->range_is_insertion_sorted_and_transformed( _sort_range );
    //_controller->notify_range_finished( _sort_range );
    _sorted.push_back( _sort_range );
}



std::list< sort_range >&
sort_worker::get_sorted_ranges()
{
    return _sorted;
}



std::list< sort_range >&
sort_worker::get_partially_sorted_ranges()
{
    return _partially_sorted;
}


} // namespace stream_process

