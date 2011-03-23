#ifndef __STREAM_PROCESS__OP_MANAGER__HPP__
#define __STREAM_PROCESS__OP_MANAGER__HPP__

#include <stream_process/op_factory.hpp>

#include <stream_process/operator_base.hpp>
#include <stream_process/stream_operator.hpp>
#include <stream_process/chain_operator.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <list>
#include <map>
#include <string>
#include <fstream>
#include <set>

namespace stream_process
{

template< typename T > class stream_manager;

template< typename sp_types_t >
class op_manager
{
public:
    typedef sp_types_t                          sp_types;
	typedef operator_base< sp_types_t >         op_base_type;
	typedef stream_operator< op_base_type >     stream_op_type;
	typedef chain_operator< op_base_type >      chain_op_type;
    typedef stream_manager< sp_types_t >        stream_manager_type;
    typedef slice_manager< sp_types_t >         slice_manager_type;

    typedef operator_factory_functor< stream_op_type >  stream_op_factory_type; 
    typedef operator_factory_functor< chain_op_type >   chain_op_factory_type; 

    op_manager( stream_manager_type& spman_, stream_config& cfg_, 
        slice_manager_type& slice_manager_ );

    // operator chains ( = list of stream operators used for processing )
    void add_chain( const std::string& name, const std::string& chain );
    const std::string& get_chain( const std::string& chain_name );
    
    std::string get_default_stream_op_string() const;
    std::string get_default_chain_op_string() const;
    
    stream_op_type* get_stream_op( const std::string& name );
    chain_op_type*  get_chain_op( const std::string& name );
    
    void get_operator( const std::string& name_, stream_op_type*& op_ );
    void get_operator( const std::string& name_, chain_op_type*& op_ );

    // factory setup
    void add_stream_op( const std::string& name, stream_op_type* sop );
    void add_chain_op( const std::string& name, chain_op_type* cop );
    
    const std::map< std::string, std::string >& get_chains() const;
    
    void print_available_chains( std::ostream& os_ ) const;

protected:
    void _load_from_chain_cfg();

    stream_manager_type&    _stream_manager;
    stream_config&          _stream_config;
    slice_manager_type&     _slice_manager;

    std::map< std::string, stream_op_type* >    _stream_ops;
    std::map< std::string, chain_op_type* >     _chain_ops;
    
    std::map< std::string, std::string >        _chains;
    
    // some stream operators can be cloned and used multiple times in the 
    // same pipeline. others cannot. this checks that only cloneable operators
    // can be used multiple times.
    std::set< stream_op_type* >                 _used_originals;

}; // class op_manager

template< typename sp_types_t >
op_manager< sp_types_t >::
op_manager( stream_manager_type& spman_, stream_config& cfg_, 
    slice_manager_type& slice_manager_ )
    : _stream_manager( spman_ )
    , _stream_config( cfg_ )
    , _slice_manager( slice_manager_ )
{
    _load_from_chain_cfg();
}



template< typename sp_types_t >
void
op_manager< sp_types_t >::
add_stream_op( const std::string& name, stream_op_type* sop )
{
    _stream_ops[ name ] = sop;
    sop->set_globals( &_stream_config, &_stream_manager, &_slice_manager );
    sop->set_op_name( name );
    sop->add_config_options();
}



template< typename sp_types_t >
void
op_manager< sp_types_t >::
add_chain_op( const std::string& name, chain_op_type* cop )
{
    _chain_ops[ name ] = cop;
    cop->set_globals( &_stream_config, &_stream_manager, &_slice_manager );
    cop->set_op_name( name );
    cop->add_config_options();
}



template< typename sp_types_t >
void
op_manager< sp_types_t >::
add_chain( const std::string& name, const std::string& chain )
{
    _chains[ name ] = chain;
}


template< typename sp_types_t >
const std::string&
op_manager< sp_types_t >::
get_chain( const std::string& chain_name )
{
    std::map< std::string, std::string >::const_iterator
        it      = _chains.find( chain_name );
    if ( it != _chains.end() )
        return it->second;
    throw exception( chain_name + " is not a recognized name for a processing chain.", 
        SPROCESS_HERE );
}



template< typename sp_types_t >
void
op_manager< sp_types_t >::
get_operator( const std::string& name_, stream_op_type*& op_ )
{
    stream_op_type* original = get_stream_op( name_ );
    
    op_ = original->clone();
    if ( op_ != 0 )
    {
        return; // cloning worked, we can safely return the clone.
    }
    
    // cloning failed. 
    typename std::set< stream_op_type* >::iterator it
        = _used_originals.find( original );
    if ( it == _used_originals.end() )
    {
        // this is the first time this operator is used. return the original
        op_ = original;
        // and store the pointer for future checks.
        _used_originals.insert( original );
    }
    else
    {
        std::string msg = "attempt to use non-cloneable operator '";
        msg += original->get_op_name();
        msg += "' multiple times in the same pipeline.";
        throw exception( msg, SPROCESS_HERE );
    }
}



template< typename sp_types_t >
typename op_manager< sp_types_t >::stream_op_type*
op_manager< sp_types_t >::
get_stream_op( const std::string& name_ )
{
    typename std::map< std::string, stream_op_type* >::iterator
        it = _stream_ops.find( name_ );
    if ( it != _stream_ops.end() )
    {
        return it->second;
    }
    throw exception( name_ + " is not a recognized name for a stream operator.", 
        SPROCESS_HERE );
}



template< typename sp_types_t >
void
op_manager< sp_types_t >::
get_operator( const std::string& name_, chain_op_type*& op_ )
{
    op_ = get_chain_op( name_ );
}



template< typename sp_types_t >
typename op_manager< sp_types_t >::chain_op_type*
op_manager< sp_types_t >::
get_chain_op( const std::string& name_ )
{
    typename std::map< std::string, chain_op_type* >::iterator
        it = _chain_ops.find( name_ );
    if ( it != _chain_ops.end() )
    {
        return it->second;
    }
    throw exception( name_ + " is not a recognized name for a chain operator.", 
        SPROCESS_HERE );
}



template< typename sp_types_t >
std::string
op_manager< sp_types_t >::
get_default_stream_op_string() const
{
    return "read neighbor covariance normal radius curvature splat write";
}



template< typename sp_types_t >
std::string
op_manager< sp_types_t >::
get_default_chain_op_string() const
{
    stream_options& opts = _stream_config.get_options();
    
    #if 0
    bool stats = opts.get( "stats" ).get_bool();
    
    return stats ? "stats" : "none";
    #else
    return "none";
    #endif
}


template< typename sp_types_t >
void
op_manager< sp_types_t >::
_load_from_chain_cfg()
{
    std::string filename = "spchains.cfg";
    std::ifstream in( filename.c_str() );

    if ( ! in.is_open() )
        return;

    std::string line;
    std::vector< std::string > tokens;
    
    std::cout
        << "op_manager: adding chain definitions from '"
        << filename << "':" << std::endl;

    std::vector< std::string > chains;

    while( ! in.eof() )
    {
        std::getline( in, line );
        
        boost::split( chains, line, boost::is_any_of("#") );
        if ( ! chains.empty() )
            line = chains[0];

        chains.clear();
        boost::split( chains, line, boost::is_any_of(":") );
        if ( chains.size() > 1 )
        {
            boost::trim( chains[ 0 ] );
            boost::trim( chains[ 1 ] );
            add_chain( chains[ 0 ], chains[ 1 ] );
            std::cout
                << "op_manager: added chain '" << chains[ 0 ] 
                << ": '" << chains[ 1 ] << "'"
                << std::endl;
        }
        chains.clear();
    }

}


template< typename sp_types_t >
const std::map< std::string, std::string >&
op_manager< sp_types_t >::
get_chains() const
{
    return _chains;
}


template< typename sp_types_t >
void
op_manager< sp_types_t >::
print_available_chains( std::ostream& os_ ) const
{
    os_ << "available operator chains:\n\n";

    typename std::map< std::string, std::string >::const_iterator
        it      = _chains.begin(),
        it_end  = _chains.end();
    for( ; it != it_end; ++it )
    {
        os_ << "[ " << it->first << " ]\n"
            << "    " << it->second << "\n\n";
    }

}



#if 0
template< typename sp_types_t >
void
op_manager< sp_types_t >::
add_op( const std::string& op_name )
{
    typename std::map< std::string, stream_op_factory_t* >::iterator
        it      = _stream_op_factories.find( op_name );
    if ( it != _stream_op_factories.end() )
    {
        _stream_ops.push_back( *(it->second)() );
        return;
    }
    typename std::map< std::string, chain_op_factory_t* >::iterator
        c_it    = _chain_op_factories.find( op_name );
    if ( c_it != _chain_op_factories.end() )
    {
        _chain_ops.push_back( *(it->second)() );
    }
    
    throw exception( 
        std::string( "requested unknown operator '" ) + op_name + "'.",
        SPROCESS_HERE
        );
}



template< typename sp_types_t >
std::list< typename op_manager< sp_types_t >::stream_op_t* >&
op_manager< sp_types_t >::
get_stream_ops()
{
    return _stream_ops;
}

template< typename sp_types_t >
std::list< typename op_manager< sp_types_t >::chain_op_t* >&
op_manager< sp_types_t >::
get_chain_ops()
{
    return _chain_ops;
}
#endif

} // namespace stream_process

#endif

