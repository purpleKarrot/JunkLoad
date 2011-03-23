#ifndef __STREAM_PROCESS__PIPELINE_CONFIGURATOR__HPP__
#define __STREAM_PROCESS__PIPELINE_CONFIGURATOR__HPP__

#include <algorithm>
#include <boost/algorithm/string.hpp>

#include <stream_process/stream_process_types.hpp>

#include <stream_process/op_manager.hpp>

namespace stream_process
{

template< typename sp_types_t, typename pipeline_t >
class pipeline_configurator
{
public:
    STREAM_PROCESS_TYPES
    
    typedef op_manager< sp_types >                  factory_type;

    typedef pipeline_t                              pipeline_type;
    typedef typename pipeline_t::stream_operators   stream_operators;
    typedef typename pipeline_t::chain_operators    chain_operators;

    
    pipeline_configurator( 
        stream_config&      config_,
        pipeline_type&      pipeline_,
        factory_type&       factory_ 
    );
    
protected: // variables
    stream_config&                  _config;
    pipeline_type&                  _pipeline;
    factory_type&                   _factory;
    
    std::list< std::string >        _stream_op_names;
    std::list< std::string >        _chain_op_names;
   
    stream_operators&               _stream_operators;
    chain_operators&                _chain_operators;
    
protected: // functions
    void _setup_stream_operator_pipeline();
    void _fix_stream_op_name_list();
    
    void _setup_chain_operator_pipeline();

    template< typename operator_t >
    void _setup_instances( std::list< std::string >& op_name_list );

    template< typename operator_t >
    std::string _get_final_op_name_string( std::vector< operator_t* >& ops_ );
    
    void _store_final_pipe_in_config( stream_config& cfg_ );
    
    void _run_operator_setup_stages();
    
}; // class pipeline_configurator

#define SP_TEMPLATE_STRING  template< typename sp_types_t, typename pipeline_t >
#define SP_CLASS_NAME       pipeline_configurator< sp_types_t, pipeline_t >

SP_TEMPLATE_STRING
SP_CLASS_NAME::pipeline_configurator( 
        stream_config&      config_,
        pipeline_type&      pipeline_,
        factory_type&       factory_ 
    )
    : _config( config_ )
    , _pipeline( pipeline_ )
    , _factory( factory_ )
    , _stream_op_names()
    , _chain_op_names()
    , _stream_operators( _pipeline.get_stream_operators() )
    , _chain_operators( _pipeline.get_chain_operators() )
{
    _setup_stream_operator_pipeline();
    _setup_chain_operator_pipeline();

    _store_final_pipe_in_config( config_ );

    _run_operator_setup_stages();
    
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::_setup_stream_operator_pipeline()
{
    stream_options& options = _config.get_options();
    
    std::string in_op_names = options.get( "stream-ops" ).get_string();
    const std::string op_chain = options.get( "chain" ).get_string();
    
    if ( op_chain != "none" )
    {
        in_op_names = _factory.get_chain( op_chain );

        std::cout
            << "  note: using the stream operator preset pipeline '" 
            << op_chain << "'." << std::endl;
    }
    else if ( in_op_names == "default" )
    {
        in_op_names = _factory.get_default_stream_op_string();

        std::cout
            << "  note: using the default stream operator pipeline." 
            << std::endl;
    }

    boost::split( _stream_op_names, in_op_names, boost::is_any_of(" ") );
    
    _fix_stream_op_name_list();
    
    _setup_instances< stream_op_type >( _stream_op_names );

}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::_setup_chain_operator_pipeline()
{
    stream_options& options = _config.get_options();
    
    std::string in_op_names = options.get( "chain-ops" ).get_string();
    
    if ( in_op_names == "default" )
    {
        std::cout << "  note: using the default chain operator pipeline." << std::endl;
        in_op_names = _factory.get_default_chain_op_string();
    }
    
    if ( in_op_names == "none" )
        return;

    boost::split( _chain_op_names, in_op_names, boost::is_any_of(" ") );
    
    _setup_instances< chain_op_type >( _chain_op_names );

}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::_fix_stream_op_name_list()
{
    // add read, write and chain-in/out operators (specification of which is 
    // optional for the sake of convenience )
    
    if ( _stream_op_names.front() == "read" )
        _stream_op_names.insert( ++_stream_op_names.begin(), "chain-in" );
    else
    {
        _stream_op_names.push_front( "chain-in" );
        _stream_op_names.push_front( "read" );
    }

    if ( _stream_op_names.back() == "write" )
        _stream_op_names.insert( --_stream_op_names.end(), "chain-out" );
    else
    {
        _stream_op_names.push_back( "chain-out" );
        _stream_op_names.push_back( "write" );
    }

}



SP_TEMPLATE_STRING
template< typename operator_t >
void
SP_CLASS_NAME::_setup_instances( std::list< std::string >& op_name_list_ )
{
    typedef operator_t&     op_ref;
    typedef operator_t*     op_ptr;

    std::list< std::string >::const_iterator 
        it      = op_name_list_.begin(),
        it_end  = op_name_list_.end();
    for( ; it != it_end; ++it )
    {
        operator_t* op          = 0;
        const std::string& name = *it;
        
        _factory.get_operator( name, op );
        _pipeline.add_operator( op );
    }
}



SP_TEMPLATE_STRING
template< typename operator_t >
std::string
SP_CLASS_NAME::
_get_final_op_name_string( std::vector< operator_t* >& ops_ )
{
    std::string op_names;

    typename std::vector< operator_t* >::const_iterator
        it     = ops_.begin(),
        it_end = ops_.end();
    for( ; it != it_end; ++it )
    {
        operator_t& op = **it;
        op_names += op.get_op_name() + " ";
    }
    
    return op_names;
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::_store_final_pipe_in_config( stream_config& cfg_ )
{
    stream_options& options = cfg_.get_options();

    std::string sop_names = _get_final_op_name_string( _stream_operators );
    std::cout << "\n[stream operator pipeline]\n" << sop_names << "\n";

    options.get( "stream-ops" ).set_string( sop_names );

    std::string cop_names = _get_final_op_name_string( _chain_operators );
    if ( ! cop_names.empty() )
        std::cout << "[chain operator pipeline]\n" << cop_names << "\n";
    
    options.get( "chain-ops" ).set_string( cop_names );

}



template< typename T, typename functor_t >
inline void
foreach_pointer( T& t, const functor_t& func )
{
    std::for_each( t.begin(), t.end(), func );
};



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
_run_operator_setup_stages()
{
    // in this first step, we run it on the stream ops first, since they might
    // generate chain ops
    stream_operators& s     = _stream_operators;
    chain_operators& c      = _chain_operators;

    assert( ! s.empty() );
    
    foreach_ptr( s, boost::mem_fn( &op_base_type::prepare_setup ) );
    foreach_ptr( c, boost::mem_fn( &op_base_type::prepare_setup ) );

    foreach_ptr( c, boost::mem_fn( &op_base_type::setup_negotiate ) );
    foreach_ptr( s, boost::mem_fn( &op_base_type::setup_negotiate ) );

    foreach_ptr( c, boost::mem_fn( &op_base_type::setup_finalize ) );
    foreach_ptr( s, boost::mem_fn( &op_base_type::setup_finalize ) );

    _config.setup_input();

    foreach_ptr( c, boost::mem_fn( &op_base_type::setup_attributes ) );
    foreach_ptr( s, boost::mem_fn( &op_base_type::setup_attributes ) );

    _config.setup_point();

    foreach_ptr( c, boost::mem_fn( &op_base_type::finalize_attributes ) );
    foreach_ptr( s, boost::mem_fn( &op_base_type::finalize_attributes ) );
    
    foreach_ptr( c, boost::mem_fn( &op_base_type::setup_accessors ) );
    foreach_ptr( s, boost::mem_fn( &op_base_type::setup_accessors ) );
}






#undef SP_TEMPLATE_STRING
#undef SP_CLASS_NAME

} // namespace stream_process

#endif

