#ifndef __STREAM_PROCESS__MT_QUEUE__HPP__
#define __STREAM_PROCESS__MT_QUEUE__HPP__

/** 
* based on code by anthony williams, justsoftwaresolutions.co.uk, do-whatever-you-want-with-it-license
* http://www.justsoftwaresolutions.co.uk/threading/implementing-a-thread-safe-queue-using-condition-variables.html
*
*
*/
#include <queue>
#include <boost/thread.hpp>

template<
    typename data_t,
    typename queue_t    = std::priority_queue< data_t >
    >
class mt_queue
{
public:
    typedef boost::mutex::scoped_lock   scoped_lock;
    
    void push( data_t const& data_ )
    {
        scoped_lock lock( _mutex );
        _queue.push( data_ );
        lock.unlock();
        _condition_variable.notify_one();
    }
    
    bool empty() const
    {
        scoped_lock lock( _mutex );
        return _queue.empty();
    }
    
    
    bool try_pop( data_t& popped_value )
    {
        scoped_lock lock( _mutex );

        if( _queue.empty() )
            return false;
        
        popped_value = _queue.front();
        _queue.pop();
        return true;
    }
	
    
	size_t size()
	{
        scoped_lock lock( _mutex );
		return _queue.size();
	}

    void wait_and_pop( data_t& popped_value )
    {
        scoped_lock lock( _mutex );
        while( _queue.empty() )
        {
            _condition_variable.wait( lock );
        }
        
        popped_value = _queue.front();
        _queue.pop();
    }

protected: 
	queue_t				    	_queue;
	mutable boost::mutex 		_mutex;
	boost::condition_variable	_condition_variable;

}; // class mt_queue

#endif

