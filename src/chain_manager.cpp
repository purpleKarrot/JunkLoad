#include "chain_manager.hpp"

#include <iostream>

#include "operator_factory.hpp"
#include "stream_op.h"

#include "Log.h"

#include "rt_struct_member_base.h"
#include "unix_utils.h"

#include "Utils.h"

#include "built_in_operators.hpp"
// TODO remove hardcodedness, make general in-out interface
#include "read_op.h"
#include "write_op.h"

#include "op_algorithm.hpp"

namespace stream_process
{

/*
chain_manager::chain_manager( int argc, const char** argv )
    : _options( *new options() )
    , _point_factory( 0 )
    , _point_info( 0 )
    , _active_set()
    , _stream_operators()
    , _chain_operators()
    , _config()
	, _stream_op_list()
	, _stream_op_factories()
	, _chain_op_factories()
    , _largest_index_in_active_set( 0 )
    , _largest_index_in_data_set( 0 )
    , _mapped_point_data( 0 )
    , _preprocess_shared_data( 0 )
{
    _options.setup( argc, argv );
}
*/


chain_manager::chain_manager( stream_processing_options& options_, 
    rt_struct_factory* factory )
    : _options( options_ )
    , _point_factory( factory )
    , _point_info( 0 )
    , _active_set()
    , _stream_operators()
    , _chain_operators()
    , _config()
	, _stream_op_list()
	, _stream_op_factories()
	, _chain_op_factories()
    , _largest_index_in_active_set( 0 )
    , _largest_index_in_data_set( 0 )
    , _input_mapped_point_data( 0 )
    , _preprocess_shared_data( 0 )
{}



chain_manager::~chain_manager()
{
    delete _point_factory;
}



void
chain_manager::setup()
{
    _setup();
}



void
chain_manager::_setup()
{
    _setup_logging();
    
    if ( ! _point_info )
    {
        // if we have the input point data map, we clone the point info
        if ( _input_mapped_point_data )
            _point_info = new point_info( _input_mapped_point_data->get_point_info() );
        else
            _point_info = new point_info();
    }
    // set static vars
    if ( ! _point_factory ) 
    {
        _point_factory = new rt_struct_factory( *_point_info );
    }

    _setup_static_members();
    
    _setup_stage_0();
    _setup_stage_1();
    _setup_stage_2();

    _stream_operators.check_requirements();
    
    _chain_operators.prepare_processing();
    _stream_operators.prepare_processing();
}



void 
chain_manager::process()
{
    LOGINFO << "\nprocessing...\n" << std::endl;
    // FIXME
    read_op* read_op_ = static_cast< read_op* >( _stream_operators.get_head() );
    while( ! read_op_->has_read_all_points_from_input() )
    {
        _stream_operators.pull_push();
    }
    _stream_operators.clear_stage();
    
    _prepare_shutdown();
}



bool
chain_manager::process( size_t points )
{
    LOGINFO << "\nprocessing...\n" << std::endl;
    _largest_index_in_data_set = _point_info->get_point_count() - 1;

    // FIXME
    read_op* read_op_ = static_cast< read_op* >( _stream_operators.get_head() );

    size_t processed = 0;
    while( ! read_op_->has_read_all_points_from_input() && processed < points )
    {
        _stream_operators.pull_push();
        ++processed;         
    }

    bool done = read_op_->has_read_all_points_from_input();
    if ( done )
    {
        // we are done...
        _stream_operators.clear_stage();

        _prepare_shutdown();
    } 
    
    return done;
}



void
chain_manager::process_sorted_points()
{
    read_op* read_op_ = static_cast< read_op* >( _stream_operators.get_head() );

    while ( ! read_op_->has_read_all_points_from_input() )
    {
        _stream_operators.pull_push();
    }

    _stream_operators.clear_stage();
    _prepare_shutdown();

    return;
}



void
chain_manager::_setup_static_members()
{
    // FIXME

    op_algorithm::_config = & _options._options_map;

    rt_struct_user::set_factory( _point_factory );
    rt_struct_member_base::set_factory( _point_factory );

    op_common::set_active_set( &_active_set );
    
    stream_op::set_chain_manager( this );
    stream_op::set_chain_config( & _options._options_map );
    stream_op::set_point_info( _point_info ); 

    active_set_op::set_chain_config( & _options._options_map );

    _active_set._chain_operators = & _chain_operators;

    op_algorithm::_chain_manager = this;
}



void 
chain_manager::_setup_stage_0()
{
    LOGINFO << "\noperator chain: " << std::endl;

    // FIXME make configable
	var_map::iterator option_it = _options._options_map.find( "disable-stats" );
	if ( option_it == _options._options_map.end() )
	{
		create_chain_op( "stats" );
	}
	
    // FIXME TODO stream_op chain analysis and optimization
    // -> radius op if normals exist ... 

    std::vector< std::string >::const_iterator it 
        = _stream_op_list.begin();
    std::vector< std::string >::const_iterator ite 
        = _stream_op_list.end();
    for( ; it != ite; ++it )
    {
        //create operator instance and insert it into op chain
        _stream_operators.push_back_operator( _create_stream_op( *it ) );
    }

    LOGINFO << std::endl << "\noptions: " << std::endl;

    // setup stage0 - reserve simple members, setup defaults, negotiate
    _point_factory->setup_stage_0();
    _chain_operators.setup_stage_0();
    _stream_operators.setup_stage_0();
}



void
chain_manager::_setup_stage_1()
{
    // setup stage1 - reserve dependent members / from negotiation results
    _point_factory->setup_stage_1();
    _chain_operators.setup_stage_1();
    _stream_operators.setup_stage_1();
}    



void
chain_manager::_setup_stage_2()
{
    // setup stage2 - set offsets to members
    _point_factory->setup_stage_2();
    _chain_operators.setup_stage_2();
    _stream_operators.setup_stage_2();
}



void 
chain_manager::register_stream_op_factory( const std::string& name,
    stream_op_factory* factory )
{
    _stream_op_factories[ name ] = factory;
    if( ! factory->get_options().empty ) 
    {
        _options.add_op_desc( name, & factory->get_options()._op_options );
    }

}



void 
chain_manager::register_chain_op_factory( const std::string& name,
    chain_op_factory* factory )
{
    _chain_op_factories[ name ] = factory;
    if( ! factory->get_options().empty ) 
    {
        _options.add_op_desc( name, & factory->get_options()._op_options );
    }

}



void
chain_manager::setup_options()
{
    _options.post_setup();
    
    _options.load_op_chain_config();
    const std::vector< std::vector< std::string > >& op_configs 
        = _options.get_op_configs_from_cfg_file();
    
    std::vector< std::vector< std::string > >::const_iterator it 
        = op_configs.begin();
    std::vector< std::vector< std::string > >::const_iterator itend 
        = op_configs.end();
    if ( it == itend )
    {
        // op config from cmdline
        _options.get_operator_chain( _stream_op_list );
    }
    else
    {
        // op config from file 
        for( ; it != itend; ++it )
        {
            const std::string& opname = (*it)[0];
            _stream_op_list.push_back( opname );

            std::map< std::string, stream_op_factory* >::iterator fit 
                = _stream_op_factories.find( opname );

            if ( fit != _stream_op_factories.end() )
            {
                (*fit).second->get_options().add_op_config( 
                    (*it), _options._options_map 
                    );
            }
            boost::program_options::notify( _options._options_map );
        }
    }
    //_options.print_options();
}



void
chain_manager::_setup_logging()
{
    // setup logging
    int loglevel = _options._options_map[ "log-level" ].as< int >();
    switch( loglevel )
    {
        case 1:
            Log::initialize( Log::ERROR );
            break;
        case 2:
            Log::initialize( Log::WARNING );
            break;
        case 3:
            Log::initialize( Log::INFO );
            break;
        case 4:
            Log::initialize( Log::VERBOSE );
            break;
        case 5:
            Log::initialize( Log::DEBUG );
            break;
        default:
            Log::initialize( Log::INFO );
            break;
    }
    
    //_print_banner();

    // write out pid
	try
	{
		const std::string& infile 
			= _options._options_map[ "in-file" ].as< std::string >();

		size_t pid = unix_utils::write_pid( infile + ".pid" );
		LOGINFO << "pid: " << pid << std::endl;
	}
	catch ( boost::bad_any_cast& e )
	{
		throw exception( "input file name was not specified.",
			SPROCESS_HERE );
	}
}



stream_op* 
chain_manager::_create_stream_op( const std::string& op_name )
{
    LOGINFO << "  + " << op_name <<std::endl;
    std::map< std::string, stream_op_factory* >::iterator it = 
        _stream_op_factories.find( op_name );
    if ( it == _stream_op_factories.end() )
    {
        exception_message msg;
        msg << "Cannot create operator of type " << op_name << " because there " 
            << "is no factory is availabe for that type.";
        STREAM_EXCEPT( msg.str().c_str() );
    }
    return (*it).second->create();
}




active_set_op*
chain_manager::create_chain_op( const std::string& op_name )
{
    LOGINFO << "  - " << op_name <<std::endl;
    std::map< std::string, chain_op_factory* >::iterator it = 
        _chain_op_factories.find( op_name );
    if ( it == _chain_op_factories.end() )
    {
        exception_message msg;
        msg << "Cannot create operator of type " << op_name << " because there " 
            << "is no factory is availabe for that type.";
        STREAM_EXCEPT( msg.str().c_str() );
    }
    active_set_op* chain_op_ = (*it).second->create();
    // FIXME
    if ( op_name == "tree" )
    {
        _active_set._tree_op = (tree_op< pr_kd_tree_node >*) chain_op_;
    }
    else if ( op_name == "stats" )
    {
        _active_set._stats_op = (stats_op*) chain_op_;
    }

    // add op to the chain operator container
    _chain_operators.push_back( chain_op_ );

    return chain_op_;
}



stream_operators&
chain_manager::get_stream_operators()
{
    return _stream_operators;
}



const stream_operators&
chain_manager::get_stream_operators() const
{
    return _stream_operators;
}



chain_operators&
chain_manager::get_chain_operators()
{
    return _chain_operators;
}



const chain_operators&
chain_manager::get_chain_operators() const
{
    return _chain_operators;

}



void
chain_manager::set_preprocess_shared_data( preprocess_shared_data* pp_data )
{
    _preprocess_shared_data = pp_data;
}



void
chain_manager::set_input_mapped_point_data( mapped_point_data* mapped_point_data_ )
{
    assert( _input_mapped_point_data == 0 );
    _input_mapped_point_data = mapped_point_data_;
}



mapped_point_data*
chain_manager::get_input_mapped_point_data()
{
    return _input_mapped_point_data;
}



preprocess_shared_data&
chain_manager::get_preprocess_shared_data()
{
    if ( ! _preprocess_shared_data )
    {
        throw exception( "preprocessing is disabled, pp_data is not available.",
            SPROCESS_HERE );
    }
    return *_preprocess_shared_data;
}



const preprocess_shared_data&
chain_manager::get_preprocess_shared_data() const
{
    if ( ! _preprocess_shared_data )
    {
        throw exception( "preprocessing is disabled, pp_data is not available.",
            SPROCESS_HERE );
    }
    return *_preprocess_shared_data;
}



void
chain_manager::notify_on_insertion( point_insertion_notification_target* target )
{
    if ( target == 0 )
    {
        throw exception( "tried registering nullpointer for insertion notification.",
            SPROCESS_HERE );
    }

    // FIXME
    read_op* read_op_ = static_cast< read_op* >( _stream_operators.get_head() );
    read_op_->register_insertion_notification_target( target );
}



void
chain_manager::notify_on_release( point_release_notification_target* target )
{
    if ( target == 0 )
    {
        throw exception( "tried registering nullpointer for release notification.",
            SPROCESS_HERE );
    }

    // FIXME 
    write_op* write_op_ = static_cast< write_op* >( _stream_operators.get_tail() );
    write_op_->register_release_notification_target( target );
}



void
chain_manager::_prepare_shutdown()
{
    _chain_operators.prepare_shutdown();
       
    _stream_operators.prepare_shutdown();

    std::cout << "done." << std::endl;
}



} // namespace stream_process

