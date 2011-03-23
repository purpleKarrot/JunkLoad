#include "processing_manager.hpp"

#include "threading_functor.hpp"
#include "chain_manager.hpp"
#include "preprocessing_manager.hpp"
#include "built_in_operators.hpp"
#include "string_utils.h"

namespace stream_process
{
processing_manager::processing_manager( int argc, const char* argv[] )
    : _chain_manager( 0 )
    , _preprocessing_manager( 0 )
    , _mapped_point_data( 0 )
    , _options()
    , _do_preprocessing( false )
    , _do_streamprocessing( true )
{   
    _setup_internals();
    
    _options.setup( argc, argv );
    _setup_stage_0();

    // basic sanity check of args
    if ( ! _options.check() )
    {
        exit( 1 );
    }

    _setup_stage_1();

}


processing_manager::~processing_manager()
{
    delete _chain_manager;
    delete _preprocessing_manager;
    delete _mapped_point_data;
}



void
processing_manager::_setup_stage_0()
{
    // order is important -> FIXME refactor

    // create stream processor instance
    _chain_manager          = new chain_manager( _options );
    
    // add factories for the built-in operators to stream processor
    built_in_operators built_in_ops( *_chain_manager );
       
    // FIXME remove from chain mgr -> indep of main/pre process
    // configure program options
    _chain_manager->setup_options();
}


void
processing_manager::_setup_stage_1()
{
    // order is important -> FIXME refactor

    if ( _options.has_key( "sorting-only" ) )
    {
        bool sort_only = _options.get_value< bool >( "sorting-only" );
        if ( sort_only )
        {
            _do_preprocessing = true;
            _do_streamprocessing = false;
        }
    }
    //bool multi_threaded_mode = _options.get_value< bool >( "multi-threaded" );
    bool req_preprocessing  = _does_require_preprocessing();

    if ( ! _options.has_key( "preprocess-data" ) )
    {
        _options.add_option("preprocess-data", req_preprocessing );
    }
    else
    {
        _options.get_value< bool >( "preprocess-data" ) = req_preprocessing;
    }
    
    if ( req_preprocessing )
    {
        _mapped_point_data = new mapped_point_data();
        
        // create preprocessor instance
        _preprocessing_manager  = new preprocessing_manager();
        _preprocessing_manager->set_mapped_point_data( _mapped_point_data );
        preprocess_shared_data& pp_data = 
            _preprocessing_manager->get_preprocess_shared_data();
        
        // setup the preprocessor
        const std::string in_file_name 
            = _options.get_value_string( "in-file" );
        std::string tmp_name = "tmp";
        
        
        _preprocessing_manager->setup(
            in_file_name, 
            tmp_name,
            _mapped_point_data
            );
        _chain_manager->set_preprocess_shared_data( &pp_data );
    }
    else
    {
        // since the main processing is not threaded (yet), we set 
        // mt to false if no preprocessing is required.
        _options.get_value< bool >( "multi-threaded" ) = false;
    }

    if ( _do_streamprocessing )
    {
        // setup the stream processor
        _chain_manager->set_input_mapped_point_data( _mapped_point_data );
        _chain_manager->setup();
    }
}



void
processing_manager::run()
{
    if ( _do_preprocessing && ! _do_streamprocessing )
    {
        _run_preprocessing_only();
        return;
    }

    //determine if we should run multi_threaded or not
    if ( ! _options.has_key( "multi-threaded" ) )
    {
        throw exception( "multi-threaded option missing in config", SPROCESS_HERE );
    }
    bool mt = _options.get_value< bool >( "multi-threaded" );
    
    if ( mt )
    {
        LOGINFO << "mode: multi-threaded." << std::endl;
        _run();
    }
    else
    {
        LOGINFO << "mode: single-threaded." << std::endl;
        _run_single_threaded();
    }
}



void
processing_manager::_run()
{
    threading_functor< preprocessing_manager > pptf( _preprocessing_manager );
    boost::thread preprocessing_thread( pptf );

    threading_functor< chain_manager > cmtf( _chain_manager );
    boost::thread main_processing_thread( cmtf );
    
    preprocessing_thread.join();
    main_processing_thread.join();
}



void
processing_manager::_run_single_threaded()
{
    if ( _preprocessing_manager )
        _preprocessing_manager->process_single_threaded();
    _chain_manager->process();
}



void
processing_manager::_run_preprocessing_only()
{
    threading_functor< preprocessing_manager > pptf( _preprocessing_manager );
    boost::thread preprocessing_thread( pptf );

    preprocessing_thread.join();
}



bool
processing_manager::_does_require_preprocessing()
{
    // we check if preprocessing was specified on the command line
    if ( _options.has_key( "preprocess-data" ) )
    {
        if ( _options.get_value< bool >( "preprocess-data" ) )
            return true;
        // else option exists but is set to false
    }
    bool does_req_preprocessing = false;
    
    if ( ! _options.has_key( "in-file" ) )
    {
        throw exception( "input data file not specified.", SPROCESS_HERE );
    }
    //_options.debug_determine_type( "in-file" );
    
    const std::string in_file_name 
        = _options.get_value_string( "in-file" );
        //= _options.get_value< std::string >( "in-file" );
        
    size_t dot_pos = in_file_name.find_last_of('.') + 1;
    
    if( dot_pos >= in_file_name.size() )
    {
        // no dot - cannot determine if preprocessing is needed from filename
        return does_req_preprocessing;
    }
    std::string suffix = in_file_name.substr( dot_pos );
    string_utils::to_lower_case( suffix );
    
    std::list< std::string >::const_iterator it 
        = std::find( 
            _preprocessing_suffixes.begin(), 
            _preprocessing_suffixes.end(), 
            suffix );
        
    if ( it != _preprocessing_suffixes.end() )
        does_req_preprocessing = true;
        
    return does_req_preprocessing;
}


void
processing_manager::_setup_internals()
{
    _preprocessing_suffixes.push_back( "ply" );
    _preprocessing_suffixes.push_back( "obj" );
    _preprocessing_suffixes.push_back( "txt" );
}


} // namespace stream_process

