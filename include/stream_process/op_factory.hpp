#ifndef __STREAM_PROCESS__OP_FACTORY__HPP__
#define __STREAM_PROCESS__OP_FACTORY__HPP__

#include <stream_process/stream_config.hpp>
#include <stream_process/operator_base.hpp>

namespace stream_process
{

template< typename operator_base_t >
class operator_factory_functor
{
public:
	operator_factory_functor( const stream_config& stream_config_ )
		: _stream_config( stream_config_ ) {}
	
	virtual operator_base_t* operator()() = 0;

protected:
	stream_config&      _stream_config;

}; // class operator_factory


template< typename operator_base_t, typename operator_t >
class op_factory : public operator_factory_functor< operator_base_t >
{
public:
	typedef operator_factory_functor< operator_base_t >	op_factory_t;
	
	op_factory( stream_config& stream_config_ )
		: op_factory_t( stream_config_ ) {}

	virtual operator_base_t* operator()()
	{
		return new operator_t( op_factory_t::_stream_config );
	}
	
}; // class op_factory


template< typename operator_base_t >
class operator_factory
{
public:
    typedef operator_factory_functor< operator_base_t >*    op_factory_t;

    operator_base_t*   construct( const std::string& name )
    {
        typename std::map< std::string, op_factory_t* >::iterator it = _ops.find( name );
        if ( it != _ops.end() )
        {
            return *(it->second)();
        }
        throw exception(
            std::string( "no factory for operator" ) + name + " found.",
            SPROCESS_HERE 
            );
    }

    void add_op_factory( const std::string& name, op_factory_t* op_factory_ )
    {
        assert( op_factory_ );
        
        typename std::map< std::string, op_factory_t* >::iterator it = _ops.find( name );
        if ( it == _ops.end() )
        {
            _ops[ name ] = op_factory_;
            return;
        }
        throw exception(
            std::string( "attempt to register a factory for " ) + name 
                + ": name taken.",
            SPROCESS_HERE 
            );
        
    }
    
protected:
    std::map< std::string, op_factory_t* >   _ops;

}; // class operator_factory

} // namespace stream_process

#endif

