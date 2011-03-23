#ifndef __STREAM_PROCESS__PIPELINE__HPP__
#define __STREAM_PROCESS__PIPELINE__HPP__

#include <stream_process/stream_process_types.hpp>

#include <stream_process/stream_config.hpp>

#include <stream_process/pipeline_configurator.hpp>

#include <stream_process/stream_file_reader.hpp>
#include <stream_process/stream_file_writer.hpp>

namespace stream_process
{

template< typename sp_types_t >
class pipeline
{
public:
    STREAM_PROCESS_TYPES
    
    typedef op_manager< sp_types >          factory_type;

    typedef std::vector< stream_op_type* >  stream_operators;
    typedef std::vector< chain_op_type* >   chain_operators;
    
    typedef stream_file_reader< stream_op_type >    stream_reader;
    typedef stream_file_writer< stream_op_type >    stream_writer;
    
    
    pipeline( stream_config& config_, factory_type& factory_ );
    ~pipeline();
    
    void add_operator( stream_op_type* op_ );
    void add_operator( chain_op_type* op_ );
    
    stream_operators&           get_stream_operators();
    const stream_operators&     get_stream_operators() const;

    chain_operators&            get_chain_operators();
    const chain_operators&      get_chain_operators() const;
    
    stream_reader*              get_reader();
    
protected: // functions
    void _setup_reader_writer_pointers();
    
protected: // variables
    stream_operators    _stream_operators;
    chain_operators     _chain_operators;

    stream_reader*      _reader;
    stream_writer*      _writer;

}; // class pipeline

#define SP_TEMPLATE_STRING	template< typename sp_types_t >
#define SP_CLASS_NAME		pipeline< sp_types_t >


SP_TEMPLATE_STRING
SP_CLASS_NAME::pipeline( stream_config& config_, factory_type& factory_ )
    : _stream_operators()
    , _chain_operators()
    , _reader( 0 )
    , _writer( 0 )
{
        std::cout << "setting up stream operator pipeline." << std::endl;

    typedef pipeline_configurator< sp_types, SP_CLASS_NAME > configurator_type;
    configurator_type cfg( config_, *this, factory_ );

        std::cout << "successfully setup processing pipeline." << std::endl;

    _setup_reader_writer_pointers();

}



SP_TEMPLATE_STRING
SP_CLASS_NAME::~pipeline()
{
    typename stream_operators::iterator 
        sit     = _stream_operators.begin(),
        sit_end = _stream_operators.end();
    for( ; sit != sit_end; ++sit )
    {
        delete *sit;
    }
    
    typename chain_operators::iterator
        cit     = _chain_operators.begin(),
        cit_end = _chain_operators.end();
    for( ; cit != cit_end; ++cit )
    {
        delete *cit;
    }
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
add_operator( stream_op_type* op_ )
{
    if ( op_ == 0 )
    {
        throw exception( "attempt to add nullpointer operator to pipeline.",
            SPROCESS_HERE );
    }
    _stream_operators.push_back( op_ );
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
add_operator( chain_op_type* op_ )
{
    if ( op_ == 0 )
    {
        throw exception( "attempt to add nullpointer operator to pipeline.",
            SPROCESS_HERE );
    }
    _chain_operators.push_back( op_ );
}




SP_TEMPLATE_STRING
void
SP_CLASS_NAME::_setup_reader_writer_pointers()
{
    // store the reader and writer operators directly as members
    _reader = dynamic_cast< stream_file_reader< stream_op_type >* >( 
        _stream_operators.front() );
    
    if ( _reader == 0 )
    {
        throw exception( "attempt to cast first operator to reader failed - the processing pipeline seems to be invalid.",
            SPROCESS_HERE );
    }
    _writer = dynamic_cast< stream_file_writer< stream_op_type >* >(
        _stream_operators.back() );
        
    if ( _writer == 0 )
        throw exception( "attempt to cast last operator to writer failed - the processing pipeline seems to be invalid.",
            SPROCESS_HERE );
}



SP_TEMPLATE_STRING
typename SP_CLASS_NAME::stream_operators&
SP_CLASS_NAME::get_stream_operators()
{
    return _stream_operators;
}




SP_TEMPLATE_STRING
const typename SP_CLASS_NAME::stream_operators&
SP_CLASS_NAME::get_stream_operators() const
{
    return _stream_operators;
}



SP_TEMPLATE_STRING
typename SP_CLASS_NAME::chain_operators&
SP_CLASS_NAME::get_chain_operators()
{
    return _chain_operators;
}



SP_TEMPLATE_STRING
const typename SP_CLASS_NAME::chain_operators&
SP_CLASS_NAME::get_chain_operators() const
{
    return _chain_operators;
}



SP_TEMPLATE_STRING
typename SP_CLASS_NAME::stream_reader*
SP_CLASS_NAME::get_reader()
{
    return _reader;
}



#if 0


SP_TEMPLATE_STRING
void
SP_CLASS_NAME::run()
{
    // FIXME boundary checks!

    while( _reader->has_more() )
    {
        typename std::vector< stream_op_type* >::iterator
            it      = _stream_operators.begin(),
            prev_it = _stream_operators.begin(),
            it_end  = _stream_operators.end();

        (*it)->push( 0 );
        for( ++it ; it != it_end; ++it, ++prev_it )
        {
            stream_op_type* op    = *it;
            stream_op_type* prev  = *prev_it;
        
            while( slice_type* slice_ = prev->top() )
            {
                std::cout << "innerest" << std::endl;
                prev->pop();
                op->push( slice_ );
            }
        }
    }
    
    foreach_ptr( _stream_operators, 
        boost::mem_fn( & stream_op_type::clear_stage ) );
    foreach_ptr( _chain_operators, 
        boost::mem_fn( & chain_op_type::clear_stage ) );

    {
        typename std::vector< stream_op_type* >::iterator
            it      = _stream_operators.begin(),
            prev_it = _stream_operators.begin(),
            it_end  = _stream_operators.end();

        for( ++it ; it != it_end; ++it, ++prev_it )
        {
            stream_op_type* op    = *it;
            stream_op_type* prev  = *prev_it;
        
            while( slice_type* slice_ = prev->top() )
            {
                prev->pop();
                op->push( slice_ );
            }
        }
    }
    
    foreach_ptr( _chain_operators,
        boost::mem_fn( &op_base_type::prepare_shutdown ) );
    foreach_ptr( _stream_operators,
        boost::mem_fn( &op_base_type::prepare_shutdown ) );
}
#endif


#undef SP_TEMPLATE_STRING
#undef SP_CLASS_NAME

} // namespace stream_process

#endif

