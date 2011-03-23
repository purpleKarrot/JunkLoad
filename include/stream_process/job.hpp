#ifndef __STREAM_PROCESS__JOB__HPP__
#define __STREAM_PROCESS__JOB__HPP__

#include <stream_process/operator_base.hpp>
#include <stream_process/stream_operator.hpp>

#include <cstddef> 

namespace stream_process
{

template<
	typename data_t,
	typename op_t,
	typename stage_t, 
	typename priority_t >
class thread_job
{
public:
	typedef data_t		data_type;
	typedef op_t		op_type;
	typedef stage_t		stage_type;
	typedef priority_t	priority_type;

	thread_job() : _data( 0 ), _op( 0 ), _stage( 0 ), _msg( 0 ), _priority() {}
	thread_job( op_type* op_, stage_type* stage_ )
		: _data( 0 ), _op( op_ ), _stage( stage_ )
	{
		assert( _op );
		assert( _stage );
		_priority	= _stage->get_priority();
	}

	inline void setup( data_type* data_ )
	{
		_data	= data_;
		assert( _data );
		assert( _op );
		assert( _stage );
	}

	inline void init( data_type* data_, op_type* op_, stage_type* stage_ )
	{
		_data	= data_;
		_op		= op_;
		_stage	= stage_;

		assert( _data );
		assert( _op );
		assert( _stage );
		
		_priority	= _stage->get_priority();
	}
	
	inline void operator()()
	{
		assert( _data );
		assert( _op );
		assert( _stage );
		
		_op->push( _data );
	}
	
	inline bool operator<( const thread_job& job_ ) const
	{
		return _priority < job_._priority;
	}

	inline bool operator>( const thread_job& job_ ) const
	{
		return _priority > job_._priority;
	}
    
    inline void set_msg( size_t msg_ )
    {
        _msg = msg_;
    }
    
    inline size_t get_msg() const
    {
        return _msg;
    }
    
	inline data_type* get_data()	{ return _data; }
	inline op_type* get_op()		{ return _op; }
	inline stage_type* get_stage()	{ return _stage; }
	
protected:
	data_type*				_data;
	op_type*				_op;
	stage_type*				_stage;
	priority_type			_priority;
    size_t                  _msg;
};

#if 0
template< typename T > class processing_stage;
template< typename T, typename C > class thread_manager;


template< typename sp_types_t >
class job : public sp_types_t
{
public:
    typedef typename sp_types_t::data_slice_t   data_slice_t;
    typedef operator_base< sp_types_t >         op_base_t;
    typedef stream_operator< op_base_t >        stream_op_t;
    typedef processing_stage< sp_types_t >      stage_t;
    typedef thread_manager< sp_types_t >        thread_manager_t;

    struct shared_data
    {
        shared_data( thread_manager_t* tm_, size_t sleep_dur_ )
            : _thread_manager( tm_ ), _sleep_duration( sleep_dur_ ) {}

        thread_manager_t*   _thread_manager;
        size_t              _sleep_duration;
    };
    
    job( shared_data& shared_data_, size_t index_ )
        : _shared_data( shared_data_ ), _is_single_job( false ), _index( index_ ) {}

    ~job() {}
    
    void init( data_slice_t* slice, stream_op_t* op_, stage_t* stage_ )
    {
        _slice          = slice;
        _op             = op_;
        _stage			= stage_;
        assert( _stage->get_stage_number() == 0 || _slice );
    }

    void init( data_slice_t* slice )
    {
        assert( _op );
        assert( _stage );
        
        assert( slice );
        _slice      = slice;
    }

    void setup( stream_op_t* op_, stage_t* stage_ )
    {
        _slice          = 0;
        _op             = op_;
        _stage			= stage_;
    }

    bool reset()
    {
        _slice          = 0;
        _op             = 0;
        _stage			= 0;
        return true;
    }

    inline void work()
    {
        this->operator()();
    }
    inline void operator()() const
    {
        assert( _op );
        //assert( _data_slice );
        assert( _stage );
        _op->push( _slice );

        if ( _shared_data._thread_manager )
        {
            _shared_data._thread_manager->set_completed_job( _index );
        }
    }
    
    bool operator<( const job& job_ )
    {
        // single jobs always have highest priority
        if ( _is_single_job )
        {
            if ( ! job_._is_single_job )
                return true;
        }
        else if ( job_.is_single_job )
            return false;

        return _slice->get_slice_number() < job_._slice->get_slice_number();
    }

    bool operator>( const job& job_ )
    {
        // single jobs always have highest priority
        if ( _is_single_job )
        {
            if ( ! job_._is_single_job )
                return true;
        }
        else if ( job_._is_single_job )
            return false;

        return _slice->get_slice_number() > job_._slice->get_slice_number();
    }
    
    data_slice_t* get_slice()
    {
        return _slice;
    }
    
    size_t get_stage_number()
    {
		assert( _stage );
        return _stage->get_stage_number();
    }
	
	stage_t* get_stage()
	{
		return _stage;
	}
    
    stream_op_t* get_op()
    {
        return _op;
    }

    void set_is_single( bool is_single_ )
    {
        _is_single_job = is_single_;
    }

    bool is_single() const
    {
        return _is_single_job;
    }

    size_t get_index() const { return _index; }

protected:
    shared_data&        _shared_data;

    stream_op_t*        _op;
	stage_t*            _stage;
    bool                _is_single_job;
    size_t              _index;
    
    data_slice_t*       _slice;

}; // class job

#endif

} // namespace stream_process

#endif

