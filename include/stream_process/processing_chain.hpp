#ifndef __STREAM_PROCESS__PROCESSING_CHAIN__HPP__
#define __STREAM_PROCESS__PROCESSING_CHAIN__HPP__

#include <stream_process/stream_process_types.hpp>
#include <stream_process/op_manager.hpp>

#include <stream_process/foreach.hpp>

#include <stream_process/stream_file_reader.hpp>
#include <stream_process/stream_file_writer.hpp>
#include <stream_process/slice_manager.hpp>
#include <stream_process/slice_buffer.hpp>

#include <stream_process/job_container.hpp>

#include <stream_process/stage.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>

#include <string>
#include <iterator>

namespace stream_process
{

template< typename T > class job_manager;

template< typename sp_types_t >
class processing_chain : public sp_types_t
{
public:
	STREAM_PROCESS_TYPES

    typedef stream_file_reader< stream_op_type >    reader_t;
    typedef stream_file_writer< stream_op_type >    writer_t;
	typedef op_manager< sp_types_t >                op_manager_t;
    typedef slice_manager< sp_types_t >             slice_manager_t;
	
	typedef	slice_buffer< sp_types_t >              in_buffer_type;
	
	typedef stage< slice_type, stream_op_type, in_buffer_type > stage_type;
	typedef typename stage_type::job_type						job_type;
	typedef typename std::vector< stage_type* >::iterator		stage_iterator;
    
    typedef job_container< job_type >           jobs_type;

	processing_chain(
        stream_config& stream_config_,
        op_manager_t& op_manager_,
        slice_manager_t& slice_manager_,
        jobs_type& jobs_
        );

	void setup();
	
	void build_chain();
	void setup_chain();
    
    void setup_io();
    void setup_stages();
    
    void shutdown();
    
    void add_chain_op( chain_op_type* chain_op_ );

    std::vector< stream_op_type* >&    get_stream_ops();
    std::vector< chain_op_type* >&     get_chain_ops();
    
    size_t  push_pull();
    bool    step_and_wait( bool shutdown_mode = false );
    void    wait_for_job_completion( size_t jobs );

    bool    run( bool shutdown_mode = false );

    std::string get_chain_info();

    void            clear_stages();
    
    std::vector< std::string >  get_stage_names();

protected:
	job_type*		_get_job() { return 0; }
    std::string     _build_stream_op_chain( const std::string& ops );
    std::string     _build_chain_op_chain( const std::string& ops );

    std::vector< stream_op_type* >     _stream_ops;
    std::vector< chain_op_type* >      _chain_ops;
	std::vector< stage_type* >		_stages;
    
    stream_config&                  _stream_config;
	op_manager_t&					_op_manager;
    slice_manager_t&                _slice_manager;
    
    jobs_type&                      _jobs;

    reader_t*                       _reader;
    writer_t*                       _writer;
    
	std::vector< job_type* >		_idle_jobs;
	size_t							_number_of_threads;
    
    size_t                          _DEBUG_counter;
    size_t                          _progress_counter;
    
}; // class processing_chain



template< typename sp_types_t >
processing_chain< sp_types_t >::
processing_chain( stream_config& stream_config_, op_manager_t& op_manager_,
    slice_manager_t& slice_manager_, jobs_type& jobs_ )
	: _stream_config( stream_config_ )
    , _op_manager( op_manager_ )
    , _slice_manager( slice_manager_ )
    , _jobs( jobs_ )
    , _reader( 0 )
    , _writer( 0 )
    , _DEBUG_counter( 0 )
    , _progress_counter( 0 )
{}



template< typename sp_types_t >
size_t
processing_chain< sp_types_t >::
push_pull()
{
    //std::cout << "chain: push_pull" << std::endl;
    // push a new input slice
    bool more_input = _reader->has_more();
    if ( more_input )
    {
        slice_type* slice = _slice_manager.create();
        _stages.front()->push( slice );
    }
    
    size_t number_of_jobs = 0;
    // pull the slices through the processing chain
    stage_iterator it = _stages.begin(), it_end = _stages.end();
    for( ; it != it_end; ++it )
    {
        stage_type* s = *it;
        number_of_jobs += s->push_pull();
    }

    _jobs.start_round( number_of_jobs );
    //std::cout << "round jobs : " << number_of_jobs << std::endl;

    typename jobs_type::mt_pq_type& todo = _jobs.get_todo();

    size_t pushed_jobs = 0;
    job_type* j;
    for( it = _stages.begin(), it_end = _stages.end(); it != it_end; ++it )
    {
        stage_type* s = *it;
        j = s->get_job();
        while( j )
        {   
            ++pushed_jobs;
            todo.push( j );
            j = s->get_job();
        }
    }
    return number_of_jobs;
    //std::cout << "todo size " << todo.size() << " pushed " << pushed_jobs << std::endl;
}



template< typename sp_types_t >
bool
processing_chain< sp_types_t >::
step_and_wait( bool shutdown_mode )
{
    size_t job_num = push_pull();
    
    if ( job_num == 0 )
        return false;

    wait_for_job_completion( job_num );
    
    ++_progress_counter;
    if ( ! (_progress_counter & 1023 ) )
    {
        std::cout << ".";
    }
    
    return shutdown_mode ? true : _reader->has_more();
}



template< typename sp_types_t >
void
processing_chain< sp_types_t >::
wait_for_job_completion( size_t num )
{
    typename jobs_type::mt_pq_type& todo = _jobs.get_todo();

    job_type* j = 0;
    bool completed = false;
    do
    {
        completed = _jobs.wait_and_pop_completed( j );
        
        assert( j );
        stage_type* stage = j->get_stage();
        stage->set_job( j );

        j = stage->get_job();
        while( j )
        {
            todo.push( j );
            j = stage->get_job();
        }
        --num;
    }
    while ( ! completed );
}



template< typename sp_types_t >
bool
processing_chain< sp_types_t >::
run( bool shutdown_mode )
{
    typename jobs_type::mt_pq_type& todo = _jobs.get_todo();

    size_t active_jobs = push_pull();

    const size_t limit = 8;
    
    bool stop = false;
    job_type* j = 0;
    while ( ! stop )
    {
        _jobs.wait_and_pop_completed( j );
        assert( j );
        stage_type* stage = j->get_stage();
        stage->set_job( j );
        --active_jobs;

        j = stage->get_job();
        while( j )
        {
            todo.push( j );
            j = stage->get_job();
        }

        if ( active_jobs < limit )
        {
            active_jobs = push_pull();
            #if 0
            std::cout
                << "round : active jobs " << active_jobs 
                << " todo " << todo.size() 
                << std::endl;
            #endif
        }
        if ( ! shutdown_mode ) 
        {
            if ( active_jobs == 0 )
                return false;
            
            if ( ! _reader->has_more() )
                return false;
        }
        else
        {
            if ( ! push_pull() && todo.size() == 0 )
            {
                assert( ! _reader->has_more() );
                return false;
            }
        }
    }

    return shutdown_mode ? true : _reader->has_more();
}

template< typename sp_types_t >
void
processing_chain< sp_types_t >::
clear_stages()
{
    foreach_ptr( _stages, boost::mem_fn( & stage_type::clear_stage ) );
    foreach_ptr( _chain_ops, boost::mem_fn( & chain_op_type::clear_stage ) );
}




template< typename sp_types_t >
void
processing_chain< sp_types_t >::
shutdown()
{
    foreach_ptr( _chain_ops,    boost::mem_fn( &op_base_type::prepare_shutdown ) );
    foreach_ptr( _stream_ops,   boost::mem_fn( &op_base_type::prepare_shutdown ) );
}



template< typename sp_types_t >
void
processing_chain< sp_types_t >::
setup()
{
	build_chain();
	setup_chain();
	setup_io();
	setup_stages();
}



template< typename sp_types_t >
void
processing_chain< sp_types_t >::
build_chain()
{
    typedef std::ostream_iterator< std::string >    ostream_it;

    stream_options& opts    = _stream_config.get_options();

	std::list< std::string >    op_list;

    std::string chain       = opts.get( "chain" ).get_string();
    std::string stream_ops  = opts.get("stream-ops").get_string();
    std::string chain_ops   = opts.get("chain-ops").get_string();
    
    // setup up stream operator-chain
    if ( chain != "none" )
        stream_ops = _op_manager.get_chain( chain );
    else if ( stream_ops == "default" )
        stream_ops = _op_manager.get_default_stream_op_string();
           
    stream_ops = _build_stream_op_chain( stream_ops );
    opts.get( "stream-ops" ).set_string( stream_ops );

    if ( chain_ops == "default" )
        chain_ops = _op_manager.get_default_chain_op_string();
    
    // setup up chain operator-chain
    chain_ops = _build_chain_op_chain( chain_ops );
    opts.get( "chain-ops" ).set_string( chain_ops );
    
    assert( _reader );
    assert( _writer );
}


template< typename sp_types_t >
std::string
processing_chain< sp_types_t >::
_build_stream_op_chain( const std::string& ops )
{
    std::list< std::string > op_list;
    boost::split( op_list, ops, boost::is_any_of(" ") );

    if ( op_list.front() == "read" )
        op_list.insert( ++op_list.begin(), "chain-in" );
    else
    {
        op_list.push_front( "chain-in" );
        op_list.push_front( "read" );
    }

    if ( op_list.back() == "write" )
        op_list.insert( --op_list.end(), "chain-out" );
    else
    {
        op_list.push_back( "chain-out" );
        op_list.push_back( "write" );
    }

    std::string used_ops = "";

    std::list< std::string >::const_iterator 
        it      = op_list.begin(),
        it_end  = op_list.end();
    for( ; it != it_end; ++it )
    {
        const std::string& op_name = *it;
        used_ops += op_name;
        used_ops += " ";
        
        stream_op_type& op_exemplar    = *_op_manager.get_stream_op( op_name );
        stream_op_type* op_            = op_exemplar.clone();

        // we try to clone the op. for some this is not possible, so we 
        // use the exemplar. however, this means that it cannot be present
        // more than once in a processing chain
        if ( op_ == 0 )
            op_ = & op_exemplar;

        _stream_ops.push_back( op_ );

        if ( op_name == "read" )
            _reader = static_cast< reader_t* >( op_ );
        else if ( op_name == "write" )
            _writer = static_cast< writer_t* >( op_ );
    }
    
    return used_ops;
}


template< typename sp_types_t >
std::string
processing_chain< sp_types_t >::
_build_chain_op_chain(  const std::string& ops )
{
    if ( ops == "none" )
        return "none";

    std::list< std::string > op_list;
    boost::split( op_list, ops, boost::is_any_of(" ") );

    std::string used_ops = "";

    std::list< std::string >::iterator 
        it      = op_list.begin(),
        it_end  = op_list.end();
    for( ; it != it_end; ++it )
    {
        used_ops += *it;
        used_ops += " ";
        _chain_ops.push_back( _op_manager.get_chain_op( *it ) );
    }
    
    return used_ops;
}



template< typename sp_types_t >
void
processing_chain< sp_types_t >::
setup_io()
{
    data_set_header& h      = _stream_config.get_header();

    stream_options& opts    = _stream_config.get_options();
    size_t batch_size       = opts.get( "slice-size" ).get_int();
    
    
    // setup streams
    std::vector< size_t > streams;
    
    typename data_set_header::iterator 
        it = h.begin(), it_end = h.end();
    for( size_t index = 0; it != it_end; ++it, ++index )
    {
        stream_structure& s = (*it)->get_structure();
        streams.push_back( s.compute_size_in_bytes() );
        // std::cout << "structure " << index << " -> " 
        //    << s.compute_size_in_bytes() << std::endl;
    }

    _slice_manager.setup( streams );
}



template< typename sp_types_t >
void
processing_chain< sp_types_t >::
setup_chain()
{   
    // in this first step, we run it on the stream ops first, since they might
    // generate chain ops
    foreach_ptr( _stream_ops,   boost::mem_fn( &op_base_type::prepare_setup ) );
    foreach_ptr( _chain_ops,    boost::mem_fn( &op_base_type::prepare_setup ) );

    foreach_ptr( _chain_ops,    boost::mem_fn( &op_base_type::setup_negotiate ) );
    foreach_ptr( _stream_ops,   boost::mem_fn( &op_base_type::setup_negotiate ) );

    foreach_ptr( _chain_ops,    boost::mem_fn( &op_base_type::setup_finalize ) );
    foreach_ptr( _stream_ops,   boost::mem_fn( &op_base_type::setup_finalize ) );

    _stream_config.setup_input();

    foreach_ptr( _chain_ops,    boost::mem_fn( &op_base_type::setup_attributes ) );
    foreach_ptr( _stream_ops,   boost::mem_fn( &op_base_type::setup_attributes ) );

    _stream_config.setup_point();

    foreach_ptr( _chain_ops,    boost::mem_fn( &op_base_type::finalize_attributes ) );
    foreach_ptr( _stream_ops,   boost::mem_fn( &op_base_type::finalize_attributes ) );
    
    foreach_ptr( _chain_ops,    boost::mem_fn( &op_base_type::setup_accessors ) );
    foreach_ptr( _stream_ops,   boost::mem_fn( &op_base_type::setup_accessors ) );
}



template< typename sp_types_t >
void
processing_chain< sp_types_t >::
add_chain_op( chain_op_type* chain_op_ )
{
    assert( chain_op_ );
    
    const std::string& op_name = chain_op_->get_op_name();

    // update config
    stream_options& opts    = _stream_config.get_options();
    std::string chain_ops   = opts.get( "chain-ops" ).get_string();
    if ( chain_ops == "none" || chain_ops == "" )
        chain_ops = op_name;
    else
    {
        chain_ops += " ";
        chain_ops += op_name;
    }
    opts.get( "chain-ops" ).set_string( chain_ops );

    // add op ptr to container
    _chain_ops.push_back( chain_op_ );
}


template< typename sp_types_t >
void
processing_chain< sp_types_t >::
setup_stages()
{
    stream_options& opts    = _stream_config.get_options();
    _number_of_threads		= opts.get( "threads" ).get_int();
	
	typename stage_type::params	stage_params( _stream_ops, _number_of_threads );

	const size_t s = _stream_ops.size();
	for( size_t index = 0; index < s; ++index )
	{
		_stages.push_back( new stage_type( stage_params ) );
	}
    
	for( size_t index = 1; index < s; ++index )
	{
        stage_type* prev = _stages[ index - 1 ];
        stage_type* next = _stages[ index ];
        
        prev->set_next( next );
        next->set_previous( prev );
	}
    // we might need to resort the slices for writing out
    _stages.back()->set_test_bounds( true, false );   
}



template< typename sp_types_t >
std::vector< typename processing_chain< sp_types_t >::stream_op_type* >&
processing_chain< sp_types_t >::
get_stream_ops()
{
    return _stream_ops;
}


template< typename sp_types_t >
std::vector< typename processing_chain< sp_types_t >::chain_op_type* >&
processing_chain< sp_types_t >::
get_chain_ops()
{
    return _chain_ops;
}



template< typename sp_types_t >
std::string
processing_chain< sp_types_t >::
get_chain_info()
{
    std::string info = "";
    
    typename std::vector< stage_type* >::iterator
        it      = _stages.begin(),
        it_end  = _stages.end();
    for( ; it != it_end; ++it )
    {
        info += (*it)->get_info_string() + "\n";
    }
    return info;
}



template< typename sp_types_t >
std::vector< std::string >
processing_chain< sp_types_t >::
get_stage_names()
{
    std::vector< std::string > stage_names;
    stage_iterator it = _stages.begin(), it_end = _stages.end();
    for( ; it != it_end; ++it )
    {
        stage_names.push_back( (*it)->get_name() );
    }
    return stage_names;
}


} // namespace stream_process

#endif

