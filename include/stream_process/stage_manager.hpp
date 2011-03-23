#ifndef __STREAM_PROCESS__STAGE_MANAGER__HPP__
#define __STREAM_PROCESS__STAGE_MANAGER__HPP__

#include <stream_process/stream_process_types.hpp>

#include <stream_process/processing_stage.hpp>

#include <vector>

namespace stream_process
{
template< typename T >  class job_manager;

template< typename sp_types_t >
class stage_manager : public sp_types_t, private std::vector< processing_stage< sp_types_t >* >
{
public:
    STREAM_PROCESS_TYPES
    
    typedef processing_stage< sp_types_t >      stage_t;
    typedef std::vector< stage_t* >             stage_container_t;
    typedef std::vector< job_t* >               job_container_t;
    typedef job_manager< sp_types_t >           job_manager_t;
    
    stage_manager( stream_config& cfg_, job_manager_t& job_manager_ );
    ~stage_manager();
    
    void push_pull( data_slice_t* in_slice );
    bool push_pull_finish();
    void collect_jobs( job_container_t& jobs );
    
    inline size_t push( data_slice_t* slice );
    inline size_t get_input_ready_size() const;
    
    void setup( std::vector< stream_op_t* >& stream_ops );
    
    using stage_container_t::iterator;
    using stage_container_t::const_iterator;
    using stage_container_t::begin;
    using stage_container_t::end;
    using stage_container_t::size;
    
protected:
    stream_config&                  _stream_config;
    job_manager_t&                  _job_manager;
    typename stage_t::shared_data   _stage_globals;

}; // class stage_manager



template< typename sp_types_t >
stage_manager< sp_types_t >::
stage_manager( stream_config& cfg_, job_manager_t& job_manager_ )
    : _stream_config( cfg_ )
    , _job_manager( job_manager_ )
    , _stage_globals( _stream_config, _job_manager )
{}



template< typename sp_types_t >
stage_manager< sp_types_t >::
stage_manager::
~stage_manager()
{
    typename stage_container_t::iterator it = begin(), it_end = end();
    for( ; it != it_end; ++it )
    {
        delete *it;
    }
    stage_container_t::clear();
}


template< typename sp_types_t >
inline size_t
stage_manager< sp_types_t >::
push( data_slice_t* in_slice )
{
    stage_t* s = stage_container_t::front();
    s->push( in_slice );
    return s->get_ready_size();
}



template< typename sp_types_t >
inline size_t
stage_manager< sp_types_t >::
get_input_ready_size() const
{
    size_t ready_size = 0;
    typename stage_container_t::const_iterator
        it      = begin(),
        it_end  = end();
    for(; it != it_end; ++it )
    {
        const stage_t* s = *it;
        if ( ! s->is_mt() )
        {
            ready_size += s->get_ready_size();
        }
    }

    return ready_size;
}




template< typename sp_types_t >
void
stage_manager< sp_types_t >::
push_pull( data_slice_t* in_slice )
{
    if ( in_slice )
        stage_container_t::front()->push( in_slice );

    typename stage_container_t::iterator
        it      = begin(),
        next_it = begin(),
        it_end  = end();
    for( ++next_it; it != it_end; ++it, ++next_it )
    {
        stage_t* stage      = *it;
        stage_t* next       = next_it == it_end ? 0 : *next_it;

        assert( stage );

        data_slice_t* slice = stage->top();

        while( slice )
        {
            stage->pop();

            assert( next );
            next->push( slice );

            slice = stage->top();
        }
    }
}




template< typename sp_types_t >
bool
stage_manager< sp_types_t >::
push_pull_finish()
{
    push_pull( 0 );
    
    size_t empty_stages = 0;
    typename stage_container_t::iterator
        it      = begin(),
        it_end  = end();
    for(; it != it_end; ++it )
    {
        stage_t* stage = *it;
        if ( stage->empty() )
            ++empty_stages;
        // else
        stage->create_new_jobs();
    }
    
    return empty_stages == stage_container_t::size();
}



template< typename sp_types_t >
void
stage_manager< sp_types_t >::
collect_jobs( job_container_t& jobs )
{
    typename stage_container_t::iterator
        it      = begin(),
        it_end  = end();
    for(; it != it_end; ++it )
    {
        stage_t* stage      = *it;
        job_t* j            = stage->get_job();
        while( j )
        {
            // fixme stage job pop explicit
            jobs.push_back( j );
            j = stage->get_job();
        }
    }
}



template< typename sp_types_t >
void
stage_manager< sp_types_t >::
setup( std::vector< stream_op_t* >& stream_ops )
{
    typename std::vector< stream_op_t* >::const_iterator 
        it      = stream_ops.begin(),
        it_end  = stream_ops.end();
    for( size_t index = 0; it != it_end; ++it, ++index )
    {
        stage_t* stage = new stage_t( _stage_globals, index, *it );
        stage_container_t::push_back( stage );
    }
}

} // namespace stream_process

#endif

