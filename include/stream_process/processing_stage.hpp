#ifndef __STREAM_PROCESS__PROCESSING_STAGE__HPP__
#define __STREAM_PROCESS__PROCESSING_STAGE__HPP__

#include <stream_process/stream_config.hpp>
#include <stream_process/stream_process_types.hpp>

#include <stream_process/job.hpp>
#include <stream_process/functors.hpp>

#include <stream_process/operator_base.hpp>
#include <stream_process/stream_operator.hpp>
#include <stream_process/job_manager.hpp>

#include <stream_process/slice_buffer.hpp>

#include <boost/pool/pool.hpp>
#include <boost/lexical_cast.hpp>

#include <queue>
#include <limits>

namespace stream_process
{


template< typename sp_types_t >
class processing_stage : public sp_types_t
{
public:
    typedef typename sp_types_t::data_slice_t   data_slice_t;
    typedef job< sp_types_t >                   job_t;
    typedef std::deque< job_t* >                job_buffer_t;
    typedef typename data_slice_t::bounds_t     bounds_t;

    typedef operator_base< sp_types_t >         op_base_t;
    typedef stream_operator< op_base_t >        stream_op_t;
    typedef std::deque< stream_op_t* >          op_buffer_t;
	
	typedef slice_buffer< sp_types_t >			slice_buffer_t;
    typedef job_manager< sp_types_t >           job_manager_t;
	
    struct shared_data
    {
        shared_data( stream_config& cfg_, job_manager_t& job_manager_ )
            : _stream_config( cfg_ )
            , _job_manager( job_manager_ )
        {}
        stream_config&  _stream_config;
        job_manager_t&  _job_manager;
    };
    
    processing_stage( shared_data& shared_data_, size_t stage_num, stream_op_t* stream_op_ );
    ~processing_stage();
    
    // a slice is kept in the stage until its dependencies are also in
    // the same stage. as soon as the last dependency-fulfilling batch is 
    // inserted, a job is created with all the currently ready batches
    // PRE: all batches come in in-order
    void push( data_slice_t* slice_ );
    
    data_slice_t* top()
    {
        return _out_buffer.empty() ? 0 : _out_buffer.front();
    }
    
    void pop()
    {
        assert( !_out_buffer.empty() );
        _out_buffer.pop();
    }
    
    // give out a job to be processed, or 0
    job_t* get_job();

    // called after a job completed.
    void set_job( job_t* job_ )
    {
        assert( job_->get_stage_number() == _stage_number );
        // update out buffer
        stream_op_t* op         = job_->get_op();
        data_slice_t* slice     = op->top();

        while( slice )
        {
            op->pop();
            _out_buffer.push( slice );
            slice = op->top();  
        }

        if ( _op_is_mt )
        {
            _idle_jobs.push_back( job_ );
        }
        else
        {
            assert( _st_job == 0 );
            _st_job = job_;
            assert( job_->reset() );
        }
        create_new_jobs();
    }

    bool empty()
    {
    #if 0
        assert( _slices );
        return _slices->empty() && _job_buffer.empty() && _out_buffer.empty();
    #else
        bool e = _in_buffer.empty() && _ready_buffer.empty() && _job_buffer.empty() && _out_buffer.empty();
#if 0
        if ( _clear_stage && ! e )
        {
            std::cout << "stage " << _stage_number << " not empty: in "
                << _in_buffer.size() << " rdy " << _ready_buffer.size() 
                << " j " << _job_buffer.size() << " o " << _out_buffer.size()
                << " idle j " << _idle_jobs.size() 
                << " st " << _st_job 
                << std::endl;
        }
#endif
        return e;
    #endif
    }
    
    size_t get_stage_number() { return _stage_number; }
    
    size_t get_ready_size() const { return _ready_buffer.size(); }
    
    bool clear_stage();
    
    void set_test_bounds( bool test_bounds_ )
    {
        _test_bounds = test_bounds_;
        if ( _test_bounds )
        {
            _in_buffer.set_force_next( true );
            _in_buffer.set_test_max_bounds( true );
        }
    }
    
    void print()
    {
        std::cout
            << "stage " << _stage_number << "[ " << _op->get_op_name() << " ]"
            << ", bounds = " << _test_bounds
            << ", slice buffer size " << _in_buffer->size() << " + " 
            << _out_buffer.size() 
            << ", job buffer size " << _job_buffer.size() << std::endl;
    }

    std::string get_info_string()
    {
        std::string num = boost::lexical_cast< std::string >( _stage_number );
        std::string info = "";
        info += num;
        while ( info.size() < 3 )
            info += " ";

        info += " ";
        info += _op->get_op_name();
        info += " ";

        while ( info.size() < 19 )
            info += " ";
        std::string opts = "";
        
        if ( _op_is_mt )
            opts += " +mt";
        if ( _test_bounds )
            opts += " +bounds";
        if ( ! opts.empty() )
        {
            info += " [";
            info += opts;
            info += " ] ";
        }
        return info;
    }


    void create_new_jobs();
    void set_stream_op( stream_op_t* stream_op_ );
    
    bool is_mt() const { return _op_is_mt; }
    
protected:
    bool                    _create_job( data_slice_t* slice_ );
    void                    _update_container( bool new_test_bounds_ );

    bool                    _test_bounds;
    const bool              _sort_slices;
    
    shared_data&            _shared_data;
    job_manager_t&          _job_manager;
    stream_config&          _stream_config;

    stream_op_t*            _op;
    size_t                  _stage_number;
    
    job_t*                  _st_job;

    job_buffer_t            _job_buffer;
    job_buffer_t            _idle_jobs;

    op_buffer_t             _op_buffer;
    
    bool                    _op_is_mt;
    
    bool                    _clear_stage;
	
	slice_buffer_t			_in_buffer;
    std::queue< data_slice_t* > _ready_buffer;
    std::queue< data_slice_t* > _out_buffer;
    
    size_t                  _max_slices_in_stage;
    
}; // class processing_stage


template< typename sp_types_t >
processing_stage< sp_types_t >::
processing_stage( shared_data& shared_data_, size_t stage_num, stream_op_t* stream_op_ )
    : _shared_data( shared_data_ )
    , _job_manager( _shared_data._job_manager )
    , _stream_config( _shared_data._stream_config )
    , _op( 0 )
    , _stage_number( stage_num )
    , _test_bounds( false )
    , _sort_slices( stream_op_->needs_bounds_checking() )
    , _op_is_mt( false )
    , _st_job( 0 )
    , _clear_stage( false )
    , _max_slices_in_stage( 0 )
{
    assert( stream_op_ );
    set_stream_op( stream_op_ );
    
    if ( _stage_number > 0 )
        _in_buffer.set_force_next( true );
}



template< typename sp_types_t >
processing_stage< sp_types_t >::
~processing_stage()
{
    std::cout << "stage " << _stage_number << " max slices " << _max_slices_in_stage << std::endl;

    if ( ! _op_is_mt )
    {
        assert( _st_job );
    }
    else
    {
        {
            typename op_buffer_t::iterator 
                it      = _op_buffer.begin(),
                it_end  = _op_buffer.end();
            for( ; it != it_end; ++it )
            {
                delete (*it);
            }
            _op_buffer.clear();
        }
        
        {
            assert( _job_buffer.empty() );
            typename job_buffer_t::iterator
                it      = _idle_jobs.begin(),
                it_end  = _idle_jobs.end();
            for( ; it != it_end; ++it )
            {
                _job_manager.destroy( *it );
            }
            _idle_jobs.clear();
        }
    }

    if ( _st_job )
        _job_manager.destroy( _st_job );
        
    assert( empty() );
}



template< typename sp_types_t >
void
processing_stage< sp_types_t >::
push( data_slice_t* slice )
{
    // insert slice
    _in_buffer.push( slice );
    
    size_t s = _in_buffer.size() + _ready_buffer.size() + _out_buffer.size();
    if ( s > _max_slices_in_stage )
        _max_slices_in_stage = s;

    // check for ready slices
    slice = _in_buffer.top();
    while( slice )
    {
        _in_buffer.pop();
        _ready_buffer.push( slice );
        slice = _in_buffer.top();
    }
    create_new_jobs();
}


template< typename sp_types_t >
void
processing_stage< sp_types_t >::
create_new_jobs()
{
    while( ! _ready_buffer.empty() && _create_job( _ready_buffer.front() ) )
    {
        _ready_buffer.pop();
    }
    #if 0
    std::cout
        << "stage " << _stage_number 
        << " create new jobs() "
        << " in buf " << _in_buffer.size()
        << " ready buf " << _ready_buffer.size() 
        << " out buf " << _out_buffer.size()
        << " jobs " << _job_buffer.size()
        << " idle jobs " << _idle_jobs.size()
        << " st jobs " << _st_job
        << std::endl;
    #endif

}



template< typename sp_types_t >
bool
processing_stage< sp_types_t >::
_create_job( data_slice_t* slice_ )
{
    job_t* j = 0;
    if ( _op_is_mt )
    {
        if ( _idle_jobs.empty() )
            return false;
        
        j = _idle_jobs.front();
        _idle_jobs.pop_front();
        j->init( slice_ );
    }
    else
    {
        if ( ! _st_job )
            return false;

        j = _st_job;
        _st_job = 0;
        j->init( slice_, _op, this );
    }
    _job_buffer.push_back( j );
    return true;
}



template< typename sp_types_t >
typename processing_stage< sp_types_t >::job_t*
processing_stage< sp_types_t >::get_job()
{
    if ( _job_buffer.empty() )
        return 0;
    
    job_t* j = _job_buffer.front();
    _job_buffer.pop_front();
    return j;
}



template< typename sp_types_t >
void
processing_stage< sp_types_t >::
set_stream_op( stream_op_t* stream_op_ )
{
    assert( ! _op );
    assert( stream_op_ );
    _op = stream_op_;
    
    _op_is_mt = _op->is_multi_threadable();
    if ( ! _op_is_mt )
    {
        _st_job = _job_manager.create();
        _st_job->set_is_single( true );
    }
    else
    {
        // we create enough instances so that even if all threads work on this
        // stage, we have enough...
        size_t max_threads = _stream_config.get_options().get( "threads" ).get_int();
        for( size_t index = 0; index < max_threads; ++index )
        {
            stream_op_t* op = _op->clone();
            //std::cout << "cloned " << _op->get_op_name() << std::endl;
            assert( op );
            _op_buffer.push_back( op );
        }

        typename op_buffer_t::iterator
            it      = _op_buffer.begin(),
            it_end  = _op_buffer.end();
        for( ; it != it_end; ++it )
        {
            job_t* j = _job_manager.create();
            j->setup( *it, this );
            _idle_jobs.push_back( j );
        }
        
    }
    
    set_test_bounds( _op->needs_bounds_checking() );
    
}


template< typename sp_types_t >
bool
processing_stage< sp_types_t >::
clear_stage()
{
    _op->clear_stage();
    
    typename op_buffer_t::iterator
        it      = _op_buffer.begin(),
        it_end  = _op_buffer.end();
    for( ; it != it_end; ++it )
    {
        (*it)->clear_stage();
    }
    
    if ( _sort_slices )
    {
        _in_buffer.set_clear_stage( true );
    }
    
    //set_test_bounds( false );
    _clear_stage = true;
    return empty();
}



template< typename sp_types_t >
void
processing_stage< sp_types_t >::
_update_container( bool new_test_bounds_ )
{
}



} // namespace stream_process

#endif

