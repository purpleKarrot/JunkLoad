#ifndef __STREAM_PROCESS__OPERATOR_BASE__HPP__
#define __STREAM_PROCESS__OPERATOR_BASE__HPP__

#include <stream_process/stream_point.h>
#include <stream_process/attribute_accessor.hpp>

#include <iostream>

namespace stream_process
{

class data_structure;
class stream_config;
class stream_options;

template< typename T > class stream_manager;
template< typename T > class op_manager;
template< typename T > class slice_manager;

template< typename stream_process_types >
class operator_base : public stream_process_types
{
public:
    typedef stream_process_types        sp_types_t;
    typedef sp_types_t                  sp_types;
    typedef stream_manager< sp_types >  stream_manager_type;
    typedef slice_manager< sp_types >   slice_manager_type;
    
    operator_base() {}
    virtual ~operator_base() {}

    // setup functions - will be called in this order:
    // - prepare_setup      - open in/out files, add chain ops, ...
    // - setup_negotiate    - init options or config values that might be changed in other ops
    // - setup_finalize     - read final config values 
    // - setup_attributes   
    // - finalize_attributes
    // -- point structure will be set up and finalized
    // - setup_accessors 

    // if an operator needs to create another op 
    virtual void prepare_setup() {}

    // negotiate stream and op config 
    virtual void setup_negotiate()	{}
    // setup according to negotiated settings
    virtual void setup_finalize()	{}

    // reserve the attributes created in this algorithm
    virtual void setup_attributes() {}
    virtual void finalize_attributes() {}
    // setup the accessors with the final point structure
    virtual void setup_accessors()	{}
    
    // shutdown functions - will be called in this order
    // - clear stage
    // ... finish processing of all points
    // - prepare shutdown
    
    virtual void clear_stage()      {}
    virtual void prepare_shutdown() {}
    
    
    virtual bool is_multi_threadable() const { return false; }
    
	// overwite in subclass to add command line options for an operator
	// WARNING: it is your responsibility that there are no name clashes.
	virtual void add_config_options()	{};
	
	template< typename T >
	const attribute& read( point_structure& ps, const std::string& name, 
		attribute_accessor< T >& acc_, size_t array_size = 0 );
	
	template< typename T >
	attribute& write( point_structure& ps, const std::string& name, 
		 attribute_accessor< T >& acc_, bool is_output_ = true, 
         size_t array_size = 0 ); // array size must default to 0 for legacy reasons FIXME
	
    virtual void set_globals( stream_config* config_,
        stream_manager_type* stream_manager_, 
        slice_manager_type* slice_manager_ );
        
    virtual void set_op_name( const std::string& op_name_ );
    virtual const std::string& get_op_name() const;

protected:
    stream_config*          _stream_config;
    stream_manager_type*    _stream_manager;
    slice_manager_type*     _slice_manager;
    
    std::string                     _op_name;

	std::list< std::string >        _read_attributes;
	std::list< std::string >        _written_attributes;

}; // class operator_base



template< typename stream_process_types >
template< typename T >
const attribute&
operator_base< stream_process_types >::
read( point_structure& ps, const std::string& name, 
	attribute_accessor< T >& acc_, size_t array_size )
{
    #if 0
    std::cerr << "op " << _op_name << " reads " << name << std::endl;
    #endif
	_read_attributes.push_back( name );
    
    if ( ! ps.has_attribute( name ) )
    {
        std::string msg = "operator '";
        msg += _op_name;
        msg += "' requested non-existent attribute '";
        msg += name;
        msg += "'.";
        throw exception( msg, SPROCESS_HERE );
        
    }
    
    assert( ps.has_attribute( name ) );
	return acc_.setup_attribute( ps, name, false, array_size );
}



template< typename stream_process_types >
template< typename T >
attribute&
operator_base< stream_process_types >::
write(  point_structure& ps, const std::string& name,
	attribute_accessor< T >& acc_, bool is_output_, size_t array_size )
{
    #if 0
    std::cerr << "op " << _op_name << " writes " << name << std::endl;
    #endif
	_written_attributes.push_back( name );
	return acc_.setup_attribute( ps, name, is_output_, array_size );
}



template< typename stream_process_types >
void
operator_base< stream_process_types >::set_globals(
    stream_config*          config_,
    stream_manager_type*    stream_manager_, 
    slice_manager_type*     slice_manager_ 
    )
{
    _stream_config  = config_;
    _stream_manager = stream_manager_;
    _slice_manager  = slice_manager_;
}



template< typename stream_process_types >
void
operator_base< stream_process_types >::
set_op_name( const std::string& op_name_ )
{
    _op_name = op_name_;
}



template< typename stream_process_types >
const std::string&
operator_base< stream_process_types >::
get_op_name() const
{
    return _op_name;
}



} // namespace stream_process

#endif

