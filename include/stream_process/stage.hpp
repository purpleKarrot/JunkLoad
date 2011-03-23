#ifndef __STREAM_PROCESS__STAGE__HPP__
#define __STREAM_PROCESS__STAGE__HPP__

#include <stream_process/foreach.hpp>
#include <stream_process/job.hpp>

#include <boost/bind.hpp>

#include <vector>
#include <queue>

namespace stream_process
{

template<
	typename slice_t,
	typename stream_op_t,
	typename in_buffer_t,
	typename buffer_t		= std::queue< slice_t* >
	>
class stage
{
public:
	typedef slice_t			slice_type;
	typedef stream_op_t		op_type;
	typedef in_buffer_t		in_buffer_type;
	typedef buffer_t		buffer_type;

	typedef thread_job< slice_type, op_type, stage, size_t > job_type;
	
	typedef typename std::vector< op_type* >::iterator	op_iterator;
	
	struct params
	{
		params( std::vector< op_type* >& ops, size_t num )
			: _ops( ops ), _number_of_stages( 0 ), _number_of_threads( num ) {}
		
		op_type* get_next_op()
		{
			assert( _number_of_stages < _ops.size() );
			return _ops[ _number_of_stages++ ];
		}
		
		size_t						_number_of_stages;
		size_t						_number_of_threads;
		std::vector< op_type* >&	_ops;
		
	}; // struct params
	
	stage( params& params_ )
		: _stage_number( params_._number_of_stages )
		, _number_of_threads( params_._number_of_threads )
		, _priority( 0 )
        , _next( 0 )
        , _previous( 0 )
	{
		op_type* exemplar	= params_.get_next_op();
        _stage_name = exemplar->get_op_name();
		setup( exemplar );
	}
	
    
    // some operators are multi-threadable (multiple instances of the operator
    // can be run in parallel). these setup checks this and clones the operator
    // if necessary, and stores them in job instances.
    
	void setup( op_type* exemplar )
	{
		assert( exemplar );
		assert( _ops.empty() );

        // create op instances
		op_type* clone		= exemplar->clone();
        if ( clone && exemplar->is_multi_threadable() )
        {
			_ops.push_back( clone );
			for( size_t index = 1; index < _number_of_threads; ++index )
			{
				_ops.push_back( exemplar->clone() );
			}
			_priority = 1;
		}
        else
		{
			_ops.push_back( exemplar );
			_priority = 0;
		}
        
        // create job instances
		op_iterator	it = _ops.begin(), it_end = _ops.end();
		for( ; it != it_end; ++it )
		{
			job_type* j = new job_type( *it, this );
			_jobs.push_back( j );
		}
        
        determine_test_bounds();
	}

    inline size_t push_pull()
    {
        slice_type* slice = 0;
        // get slices from previous stage
        if ( _previous )
        {
            slice = _previous->front();
            while( slice )
            {
                _previous->pop();
                _in_buffer.push( slice );
                slice = _previous->front();
            }
        }
        
        // prep slices for computing
        slice = _in_buffer.top();
        while( slice )
        {
            _in_buffer.pop();
            _ready_buffer.push( slice );
            slice = _in_buffer.top();
        }
        return _ready_buffer.size();
    }


	inline job_type* get_job()
	{
		if ( _jobs.empty() || _ready_buffer.empty() )
        {
            #if 0
            if ( _jobs.empty() )
                std::cout << "stage " << _stage_number << " - no job instances " << std::endl;
            if ( _ready_buffer.empty() )
                std::cout << "stage " << _stage_number << " - no slices " << std::endl;
            #endif
			return 0;
        }
		slice_type* slice = _ready_buffer.front();
        _ready_buffer.pop();
		
		job_type* j = _jobs.back();
        _jobs.pop_back();

		j->setup( slice );

        //std::cout << "stage " << _stage_number << " - prepared job " << std::endl;
		return j;
	} 


	inline void set_job( job_type* job_ )
	{
		assert( job_->get_stage() == this );

        op_type* op = job_->get_op();
        slice_type* slice = op->top();
        while( slice )
        {
            op->pop();
            _out_buffer.push( slice );
            slice = op->top();
        }
		_jobs.push_back( job_ );
	}
	
	inline void push( slice_type* slice )
	{
		_in_buffer.push( slice );
	}
	
	inline slice_type* front()
    {
		if ( _out_buffer.empty() )
			return 0;
		return _out_buffer.front();
    }

	inline slice_type* top()
	{
		if ( _out_buffer.empty() )
			return 0;
		return _out_buffer.front();
	}
	
	inline void pop()
	{
		_out_buffer.pop();
	}
	
	void determine_test_bounds()
	{
		assert( ! _ops.empty() );
		
		if ( _ops[ 0 ]->needs_bounds_checking() )
		{
			_in_buffer.set_force_next( true );
			_in_buffer.set_test_max_bounds( true );
		}
	}
    
    
    void set_test_bounds( bool force_next, bool max_bounds )
    {
        _in_buffer.set_force_next( force_next );
        _in_buffer.set_test_max_bounds( max_bounds );
    }
	
	void clear_stage()
	{
        assert( ! _ops.empty() );
		assert( _ops.size() == _number_of_threads || ! _ops.front()->is_multi_threadable() );
		foreach_ptr( _ops, boost::mem_fn( & op_type::clear_stage ) );
		_in_buffer.set_clear_stage( true );
	}
	
	size_t get_priority() const
	{
		return _priority;
	}
    
    stage* get_next()
    {
        return _next;
    }
    
    void set_next( stage* next_ )
    {
        _next = next_;
    }
    
    
    stage* get_previous()
    {
        return _previous;
    }
    
    void set_previous( stage* prev_ )
    {
        _previous = prev_;
    }
    
    size_t get_stage_number() const
    {
        return _stage_number;
    }
    
    size_t get_in_size() const
    {
        return _in_buffer.size();
    }
    
    size_t size() const
    {
        size_t size = _in_buffer.size();
        size += _out_buffer.size();
        typename std::vector< op_type* >::const_iterator it = _ops.begin(), it_end = _ops.end();
        for( ; it != it_end; ++it )
        {
            size += (*it)->size();
        }
        return size;
    }
    
    bool empty() const
    {
        if ( _ops.empty() )
            return false;
        if ( ! _in_buffer.empty() || ! _out_buffer.empty() )
            return false;
        if ( _ops.front()->is_multi_threadable() && _ops.size() != _number_of_threads )
            return false;
        if ( _ops.front()->top() )
            return false;
        return true;
    }
    
    inline void print()
    {
        std::string op_name = _ops[0]->get_op_name();
        size_t s = op_name.size();
        while( s < 20 )
        {
            op_name += " ";
            ++s;
        }
        std::cout << "stage " << _stage_number << ": " << op_name;
        if ( _in_buffer.get_force_next() )
            std::cout << "next|";
        else
            std::cout << "    |";

        if ( _in_buffer.get_test_max_bounds() )
            std::cout << "max|";
        else
            std::cout << "   |";

        std::cout << " jobs " << _jobs.size() 
            << std::endl;
    }
    
    inline const std::string& get_name() const
    {
        return _stage_name;
    }

protected:
	std::vector< op_type* >		_ops;
	std::vector< job_type* >	_jobs;
	
	in_buffer_type	_in_buffer;
	buffer_type		_ready_buffer;
	buffer_type		_out_buffer;
	
    std::string     _stage_name;
	size_t			_stage_number;
	size_t			_bounds_mode;
	size_t			_number_of_threads;
	size_t			_priority;
	
    stage*          _next;
    stage*          _previous;
	
}; // class stage

} // namespace stream_process

#endif

