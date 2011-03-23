#ifndef __STREAM_PROCESS__JOB_MANAGER__HPP__
#define __STREAM_PROCESS__JOB_MANAGER__HPP__

#include <stream_process/stream_process_types.hpp>
#include <stream_process/thread_manager.hpp>
#include <stream_process/job.hpp>

namespace stream_process
{
template< typename sp_types_t >
class job_manager
{
public:
    STREAM_PROCESS_TYPES 

    typedef thread_manager< sp_types_t >    thread_manager_t;
    typedef typename job_t::shared_data     job_shared_data_t;

    job_manager();
    ~job_manager()
    {
        std::cout << "created " << _jobs_created << " jobs and "
            << "destroyed " << _jobs_destroyed << " jobs." << std::endl;
        assert( _jobs_created == _jobs_destroyed );
    }

    job_t* create()
    {
        job_t* j = reinterpret_cast< job_t* >( _job_pool.malloc() );
        new ( j ) job_t( _job_shared_data, _jobs_created );
        
        _jobs.push_back( j );
        ++_jobs_created;
        assert( _jobs.size() == _jobs_created );

        return j;
    }

    void destroy( job_t* j )
    {
        // _jobs[ j->get_index() ] = 0;
        j->~job_t();
        _job_pool.free( j );
        
        ++_jobs_destroyed;
    }

    void set_thread_manager_ptr( thread_manager_t* thread_manager_ptr_ )
    {
        _job_shared_data._thread_manager = thread_manager_ptr_;
        if ( thread_manager_ptr_ )
            thread_manager_ptr_->set_job_vector( _jobs );
    }
    
    void get_max_job_index() { return _jobs_created; }

protected:
    boost::pool<>       _job_pool;
    
    job_shared_data_t   _job_shared_data;
    
    size_t              _jobs_created;
    size_t              _jobs_destroyed;
    
    std::vector< job_t* >   _jobs;

}; // class job_manager


template< typename sp_types_t >
job_manager< sp_types_t >::
job_manager()
    : _job_pool( sizeof( job_t ) )
    , _job_shared_data( 0, 25 )
    , _jobs_created( 0 )
    , _jobs_destroyed( 0 )
{}



} // namespace stream_process

#endif

