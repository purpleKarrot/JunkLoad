#ifndef __STREAM_PROCESS__JOB_CONTAINER__HPP__
#define __STREAM_PROCESS__JOB_CONTAINER__HPP__

#include <stream_process/mt_queue.hpp>
#include <stream_process/priority_queue.hpp>

namespace stream_process
{

template< typename job_t >
class job_container
{
public:
    typedef job_t                           job_type;
    typedef ptr_greater< job_type* >        job_cmp_type;
    typedef std::vector< job_type* >        job_container_type;

    typedef priority_queue< job_type*, job_container_type, job_cmp_type > pq_type;

    typedef std::queue< job_type* >             queue_type;
    typedef mt_queue< job_type*, queue_type >	mt_queue_type;
    typedef mt_queue< job_type*, pq_type >		mt_pq_type;

    mt_pq_type&     get_todo();
    mt_queue_type&  get_done();
 
    inline void start_round( size_t jobs_ )
    {
        _number_of_jobs = jobs_;
        _completed = 0;
    }   

    inline bool wait_and_pop_completed( job_type*& j )
    {
        _done.wait_and_pop( j );
        ++_completed;
        return _completed == _number_of_jobs;
    }

protected:
    mt_pq_type      _todo;
    mt_queue_type   _done;

    size_t          _number_of_jobs;
    size_t          _completed;

}; // class job_container


template< typename job_t >
typename job_container< job_t >::mt_pq_type&
job_container< job_t >::
get_todo()
{
    return _todo;
}


template< typename job_t >
typename job_container< job_t >::mt_queue_type&
job_container< job_t >::
get_done()
{
    return _done;
}

} // namespace stream_process

#endif

