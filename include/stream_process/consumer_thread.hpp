#ifndef __STREAM_PROCESS__CONSUMER_THREAD__HPP__
#define __STREAM_PROCESS__CONSUMER_THREAD__HPP__

#include <stream_process/mode_timer.hpp>

namespace stream_process
{

template<
	typename func_t, 
	typename job_container_t,
    typename thread_manager_t
	>
class consumer_thread
{
public:
	typedef func_t                              func_type;
    typedef thread_manager_t                    thread_manager_type;
	typedef job_container_t                     jobs_type;
    typedef typename jobs_type::mt_pq_type      mt_pq_type;
    typedef typename jobs_type::mt_queue_type   mt_queue_type;
    
    struct shared_data
    {
        shared_data( jobs_type& jobs_, thread_manager_type* tm_, 
            const std::vector< std::string >& stages_ )
            : jobs( jobs_ )
            , tm( tm_ )
            , stages( stages_ )
        {}
        
        jobs_type&                          jobs;
        thread_manager_type*                tm;
        const std::vector< std::string >&   stages;
    };

	consumer_thread( shared_data& shared_data_, size_t num )
		: _jobs( shared_data_.jobs )
        , _tm( shared_data_.tm )
        , _thread_number( num )
    {
        init_status( shared_data_.stages );
    }

    ~consumer_thread() {}
	
	inline void operator()()
	{
        mt_pq_type& todo    = _jobs.get_todo();
        mt_queue_type& done = _jobs.get_done();
    
		func_t*	f           = 0;
        bool stop           = false;
        
        set_status( _idle_mode );
		while ( true ) 
		{
            todo.wait_and_pop( f );
            
            if ( f->get_msg() == 1 )
            {
                _mode_timer.stop();
                break;
            }

            set_status( f->get_stage()->get_stage_number() );

			func_t& f_ = *f;
			f_();
			done.push( f );

            set_status( _idle_mode );
		}

        shutdown();
	}
	
    inline void set_status( size_t status )
    {
        _mode_timer.change_mode( status );
    }
    
    inline void init_status( const std::vector< std::string >& stages )
    {
        _mode_timer.set_modes( stages );
        _idle_mode = _mode_timer.add_mode( "idle" );
    }
    
    void shutdown()
    {
        std::vector< double >& times        = _mode_timer.mode_times;
        assert( _tm );
        _tm->set_stats( times, _thread_number );
    }
    
protected:
	jobs_type&              _jobs;
    size_t                  _thread_number;
    thread_manager_type*    _tm;
    
    mode_timer              _mode_timer;
    size_t                  _idle_mode;

}; // class consumer_thread

} // namespace stream_process

#endif

