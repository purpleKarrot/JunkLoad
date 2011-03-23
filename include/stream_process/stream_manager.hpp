#ifndef __STREAM_PROCESS__STREAM_MANAGER__HPP__
#define __STREAM_PROCESS__STREAM_MANAGER__HPP__

#include <stream_process/stream_options.hpp>
#include <stream_process/stream_config.hpp>

#include <stream_process/op_manager.hpp>
#include <stream_process/processing_chain.hpp>

//FIXME #include <stream_process/preprocessing_manager.hpp>

#include <stream_process/preprocess.hpp>

#include <stream_process/thread_manager.hpp>

#include <stream_process/job.hpp>

#include <stream_process/bounds.hpp>
#include <stream_process/data_slice.hpp>
#include <stream_process/slice_manager.hpp>

#include <stream_process/stream_process_types.hpp>

#include <stream_process/exception.hpp>

#include <stream_process/functors.hpp>
#include <stream_process/mt_queue.hpp>

#include <stream_process/foreach.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/pool/pool.hpp>

// new
#include <stream_process/pipeline.hpp>
#include <stream_process/pipeline_runner_single_threaded.hpp>


#include <string>
#include <iostream>

namespace stream_process
{

template< typename sp_types_t >
class stream_manager : public sp_types_t
{
public:
	STREAM_PROCESS_TYPES

    typedef io_shared_data< sp_types >                  io_shared_data_t;
    
	typedef processing_chain< sp_types_t >              chain_type;
    typedef typename chain_type::job_type               job_type;

	typedef op_manager< sp_types_t >                    op_manager_t;
    typedef slice_manager< sp_types_t >                 slice_manager_t;

    typedef thread_manager< sp_types_t, chain_type >    thread_manager_type;
    typedef typename thread_manager_type::jobs_type     jobs_type;
    
    typedef pipeline< sp_types >                        pipeline_type;


    stream_manager();
    ~stream_manager();
    
    void setup( int argc, const char* argv[] );
    bool setup2( int argc, const char* argv[], bool old_mt_ = false );
    
    int process();
    int process2();
    
    stream_config&              get_stream_config();
    op_manager< sp_types_t >&   get_op_manager();
	
    // processing chain forwards
    void add_chain_op( chain_op_type* chain_op_ );
    std::vector< chain_op_type* >& get_chain_ops();

protected:
    void _setup();

    void _process_single_threaded();
	size_t _st_process_jobs();

    void _process_multi_threaded();
    
    void _preprocess();
    
    void _list_chains();
    
    stream_options              _options;
    stream_config               _config;

    //FIXME preprocessing_manager*      _preprocessing_manager;
    slice_manager_t             _slice_manager;
    op_manager_t                _op_manager;
    
    boost::pool<>*              _point_factory;
    boost::pool<>*              _job_pool;

    jobs_type                   _jobs;
   
	chain_type                  _processing_chain;
    thread_manager_type*        _thread_manager;
    
    bool                        _multi_threading;
    
    // new
    pipeline< sp_types >*       _pipeline;
        
}; // class stream_manager



template< typename sp_types_t >
stream_manager< sp_types_t >::
stream_manager()
    : _options()
    , _config( _options )
    //FIXME, _preprocessing_manager( 0 )
    , _slice_manager()
    , _op_manager( *this, _config, _slice_manager )
	, _processing_chain( _config, _op_manager, _slice_manager, _jobs )
    , _thread_manager( 0 )
    , _multi_threading( false )
    , _pipeline( 0 )
{
    get_data_type_id_from_type< sp_float_type > sp_type;
    get_data_type_id_from_type< hp_float_type > hp_type;
    _config.set_types( sp_type(), hp_type() );
}



template< typename sp_types_t >
stream_manager< sp_types_t >::
~stream_manager()
{
    if ( _pipeline )
        delete _pipeline;
    
    // FIXME if ( _preprocessing_manager )
    //    delete _preprocessing_manager;
}



template< typename sp_types_t >
bool
stream_manager< sp_types_t >::
setup2( int argc, const char* argv[], bool old_mt_ )
{
    // parse commandline
    std::string error;
    bool ok = _options.parse_cmdline( argc, argv, error );
    
    if ( ! ok )
    {
        // an error occurred during cmdline parsing.

        // if list-chains is specified, we ignore the error and simply
        // print out the possible chains
        if ( _options.get( "list-chains" ).get_bool() == true )
        {
            _op_manager.print_available_chains( std::cout );
            return false;
        }
        // otherwise print the error message and a short howto on how to use the  
        // stream process commandline tool.
        else 
        {
            _options.print_usage();
            std::cout
                << "parsing command line options failed:\n\n"
                << error
                << std::endl;
            return false;
        }
    }
    
    _preprocess();
    _options.finalize();
    
    size_t num_threads  = _options.get( "threads" ).get_int();
    _multi_threading    = num_threads > 1;
    
    if ( _multi_threading )
    {
        std::cout
            << "multi-threading enabled, " << num_threads << " threads."
            << std::endl;

        _processing_chain.setup();
        _setup();

        _config.print_structures();
    }
    else
    {
        // setup the pipeline
        _pipeline = new pipeline_type( _config, _op_manager );

        // setup the slices (pooled memory for streaming data)
        _slice_manager.setup( _config );

        std::cout << "multi-threading disabled." << std::endl;
    }

    _options.print_values();

    return true;

}




template< typename sp_types_t >
int
stream_manager< sp_types_t >::
process2()
{
    if ( _pipeline )
    {
        assert( _pipeline );

        typedef pipeline_runner_single_threaded< 
            sp_types,
            pipeline_type
        > pipe_runner;
        
        pipe_runner pr( *_pipeline );
        return pr.run();
    }
    else
    {
        return process();
            
    }
}







template< typename sp_types_t >
void
stream_manager< sp_types_t >::
setup( int argc, const char* argv[] )
{
    try
    {
        std::string error;
        if ( ! _options.parse_cmdline( argc, argv, error ) )
        {
            if ( _options.get( "list-chains" ).get_bool() )
            {
                typedef std::map< std::string, std::string > chains_map;
                const chains_map& chains = _op_manager.get_chains();
                chains_map::const_iterator
                    it      = chains.begin(),
                    it_end  = chains.end();
                for( ; it != it_end; ++it )
                {
                    std::cout << it->first << ": " << it->second << "\n\n";
                }
                std::cout << std::endl;
            }
            else
            {
            _options.print_usage();
            std::cout
                << "parsing command line options failed:\n\n"
                << error
                << std::endl;
            }
            exit( 1 );
        }
    
        _preprocess();
        _options.finalize();
        _processing_chain.setup();
        _setup();

        _config.print_structures();

        _options.print_values();
    }
    catch( exception& e )
    {
        std::cout
            << "\n\nfatal error: "
            << e.what() 
            << std::endl;
        exit( 2 );
    }
    
}



template< typename sp_types_t >
void
stream_manager< sp_types_t >::
_setup()
{
    size_t num_threads = _options.get( "threads" ).get_int();

    _thread_manager = new thread_manager_type( _jobs, num_threads );
    _thread_manager->set_stages( _processing_chain.get_stage_names() );

    if ( num_threads > 1 )
    {
        _multi_threading = true;
    }
    else
    {
        _multi_threading = false;
    }
}



template< typename sp_types_t >
size_t
stream_manager< sp_types_t >::
_st_process_jobs()
{
    jobs_type& jobs = _thread_manager->get_jobs();
    job_type* j;

    size_t completed = 0;

    while( jobs.get_todo().try_pop( j ) )
    {
        (*j)();
        j->get_stage()->set_job( j );
        ++completed;
        j = j->get_stage()->get_job();
        while( j )
        {
            jobs.get_todo().push( j );
            j = j->get_stage()->get_job();
        }
    }
    return completed;
}




template< typename sp_types_t >
void
stream_manager< sp_types_t >::
_process_single_threaded()
{
    typedef typename thread_manager_type::jobs_type     jobs_type;
    jobs_type& jobs = _thread_manager->get_jobs();
    
    size_t num = 0;
    job_type* j = 0;

    bool stop = false;
    while( ! stop )
    {
        assert( num == 0 );
        num = _processing_chain.push_pull();

        if ( num == 0 )
            stop = true;
        else
        {
            num -= _st_process_jobs();
        }
    }
    _processing_chain.clear_stages();

    stop = false;
    while( ! stop )
    {
        assert( num == 0 );
        num = _processing_chain.push_pull();

        if ( num == 0 )
            stop = true;
        else
        {
            num -= _st_process_jobs();
        }
    }
    
    //_thread_manager->shutdown();
    _processing_chain.shutdown();
}




template< typename sp_types_t >
void
stream_manager< sp_types_t >::
_process_multi_threaded()
{
    typedef typename thread_manager_type::jobs_type     jobs_type;
    jobs_type& jobs = _thread_manager->get_jobs();
    
    _thread_manager->start();

    bool stop = false;
    while( ! stop )
    {
        stop = ! _processing_chain.step_and_wait( false );
        //stop = ! _processing_chain.run( false );
    }
    _processing_chain.clear_stages();

    stop = false;
    while( ! stop )
    {
        stop = ! _processing_chain.step_and_wait( true );
        //stop = ! _processing_chain.run( true );
    }
    
    _thread_manager->shutdown();
    _processing_chain.shutdown();
}



template< typename sp_types_t >
int
stream_manager< sp_types_t >::
process()
{
    if ( _options.get( "pre-only" ).get_bool() )
        return 0;

    if ( _multi_threading )
    {
        _process_multi_threaded();
    }
    else
        _process_single_threaded();
    return 0;
}



template< typename sp_types_t >
void
stream_manager< sp_types_t >::
_preprocess()
{
    std::string in_file = _options.get( "input-filename" ).get_string();

    if ( boost::algorithm::iends_with( in_file, ".ply" ) ) //|| suffix == ".txt" || suffix == ".obj" )
    {

        std::string new_name = in_file.substr( 0, in_file.size() - 4 );

        preprocessor::params ppp;
        ppp.source_file             = in_file;
        ppp.result_file             = new_name;
        ppp.do_optimal_transform    = _options.get( "optimal-transform" ).get_bool();
        ppp.number_of_threads       = _options.get( "threads" ).get_int();
        if ( ppp.number_of_threads < 1 )
        {
            _options.get( "threads" ).set_int( 1 );
            ppp.number_of_threads = 1;
        }

        preprocessor pp( ppp );

        _options.get( "input-filename" ).set_string( new_name );

        if ( ! _options.get( "output-filename" ).was_set_by_user() )
        {
            _options.get( "output-filename" ).set_string( new_name + "_out" );
        }
    }
    
#if 0
    std::string in_file = _options.get( "input-filename" ).get_string();
    size_t index = in_file.find_last_of( '.' );

    std::string base, suffix;

    if ( index != std::string::npos )
    {
        base    = in_file.substr( 0, index );
        suffix  = in_file.substr( index, std::string::npos );

        if ( suffix == ".ply" ) //|| suffix == ".txt" || suffix == ".obj" )
        {
            size_t number_of_threads = _options.get("threads").get_int();

            trip::preprocessor  pp;
            pp.process_sp( in_file, true, number_of_threads );

            _options.get( "input-filename" ).set_string( base );

            if ( ! _options.get( "output-filename" ).was_set_by_user() )
                _options.get( "output-filename" ).set_string( base + ".out" );
        }
    }

#endif
    #if 0
    assert( 0 && "REIMPLEMENT!" );
    // TODO make operator
    std::string in_file = _options.get( "input-filename" ).get_string();
    size_t index = in_file.find_last_of( '.' );

    std::string base, suffix;

    if ( index != std::string::npos )
    {
        base    = in_file.substr( 0, index );
        suffix  = in_file.substr( index, std::string::npos );

        if ( suffix == ".ply" || suffix == ".txt" || suffix == ".obj" )
        {
            _preprocessing_manager = new preprocessing_manager();
        }
    }
    
    if ( ! _preprocessing_manager )
        return;
        
    mapped_point_data mpd;
    _preprocessing_manager->set_mapped_point_data( &mpd );
    _preprocessing_manager->setup( in_file, base, true );
    _preprocessing_manager->process_single_threaded();
    mpd.recompute_aabb();
    
    _options.get( "input-filename" ).set_string( base );
    if ( ! _options.get( "output-filename" ).was_set_by_user() )
        _options.get( "output-filename" ).set_string( base + ".out" );
    
    if ( _preprocessing_manager )
    {
        delete _preprocessing_manager;
        _preprocessing_manager = 0;
    }
    #endif
}



template< typename sp_types_t >
op_manager< sp_types_t >&
stream_manager< sp_types_t >::
get_op_manager()
{
    return _op_manager;
}


template< typename sp_types_t >
stream_config&
stream_manager< sp_types_t >::
get_stream_config()
{
    return _config;
}



template< typename sp_types_t >
void
stream_manager< sp_types_t >::
add_chain_op( chain_op_type* chain_op_ )
{
    _processing_chain.add_chain_op( chain_op_ );
}



template< typename sp_types_t >
std::vector< typename stream_manager< sp_types_t >::chain_op_type* >&
stream_manager< sp_types_t >::
get_chain_ops()
{
	return _processing_chain.get_chain_ops();
}


} // namespace stream_process

#endif

