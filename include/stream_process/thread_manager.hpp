#ifndef __STREAM_PROCESS__THREAD_MANAGER__HPP__
#define __STREAM_PROCESS__THREAD_MANAGER__HPP__

#include <stream_process/stream_process_types.hpp>
#include <stream_process/mt_queue.hpp>
#include <stream_process/consumer_thread.hpp>
#include <stream_process/job_container.hpp>

#include <stream_process/priority_queue.hpp>

#include <boost/thread.hpp>
#include <boost/threadpool.hpp>
#include <boost/lexical_cast.hpp>

namespace stream_process
{
template< typename sp_types_t, typename processing_chain_t >
class thread_manager
{
public:
    STREAM_PROCESS_TYPES

	typedef processing_chain_t                      chain_type;
	typedef typename chain_type::job_type			job_type;
    
    typedef job_container< job_type >               jobs_type;
    
    typedef std::vector< std::vector< double > >    stats_type;
  
    typedef consumer_thread< job_type, jobs_type, thread_manager >
        consumer_type;
    
    typedef typename consumer_type::shared_data     shared_data_type;

    thread_manager( jobs_type& jobs_, size_t number_of_threads );
    
    void start();
    void shutdown();
    
    void print_stats();
	   
    jobs_type& get_jobs() 
    {
        return _jobs;
    }
    
    void set_stats( const std::vector< double >& stats_, size_t thread_num )
    {
        boost::mutex::scoped_lock( _stats_mutex );
        _stats[ thread_num ]  = stats_;
    }
    
    void set_stages( const std::vector< std::string >& stages_ )
    {
        _stages = stages_;
    }
    
protected:
    boost::threadpool::pool*    _thread_pool;
    jobs_type&                  _jobs;
    stats_type                  _stats;
    
    mutable boost::mutex        _stats_mutex;
    
    std::vector< std::string >  _stages;

    shared_data_type*           _thread_shared_data;

}; // class thread_manager


#define SP_TEMPLATE_STRING \
    template< typename sp_types_t, typename processing_chain_t >

#define SP_CLASS_NAME \
    thread_manager< sp_types_t, processing_chain_t >


SP_TEMPLATE_STRING
SP_CLASS_NAME::
thread_manager( jobs_type& jobs_, size_t number_of_threads )
    : _thread_pool( new boost::threadpool::pool( number_of_threads ) )
    , _jobs( jobs_ )
    , _thread_shared_data( 0 )
{
    _stats.resize( number_of_threads );
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
start()
{
    _thread_shared_data = new shared_data_type( _jobs, this, _stages );

    const size_t s = _thread_pool->size();
    for( size_t index = 0; index < s; ++index )
    {
        consumer_type c( *_thread_shared_data, index );
        _thread_pool->schedule( c );
    }
}


SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
shutdown()
{
    job_type shutdown_job;
    shutdown_job.set_msg( 1 );
    
    size_t s = _stats.size();
    
    for( size_t index = 0; index < s; ++index )
    {
        _jobs.get_todo().push( & shutdown_job );
    }
    // wait till all jobs finished
    while( _thread_pool->active() != 0 )
    {
        usleep( 10 );
    }

    print_stats();
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
print_stats()
{
    //_stages.push_back( "idle" );

    const size_t number_of_threads = _stats.size();
    const size_t prec = std::cout.precision();
    
    std::cout << std::endl;

    for( size_t index = 0; index < _stages.size(); ++index )
    {
        
        std::string stage_name = _stages[ index ];
        while( stage_name.size() < 20 )
            stage_name += " ";
        
        std::cout << stage_name << " ";
        
        double total = 0;
        for( size_t j = 0; j < number_of_threads; ++j )
        {
            const double t = _stats[ j ][ index ];
            total += t;
            
            // std::cout.precision( 5 );
            // std::cout << std::setw( 12 ) << t << " ";

        }
        //std::cout << "| ";
        std::cout.precision( 5 );
        std::cout << total << "s  " << std::endl;
    }
    std::cout << std::endl;
    std::cout.precision( prec );
}


#undef SP_TEMPLATE_STRING
#undef SP_CLASS_NAME


} // namespace stream_process

#endif

