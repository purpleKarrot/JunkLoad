#ifndef __STREAM_PROCESS__PIPELINE_RUNNER_SINGLE_THREADED__HPP__
#define __STREAM_PROCESS__PIPELINE_RUNNER_SINGLE_THREADED__HPP__

#include <stream_process/stream_process_types.hpp>
#include <stream_process/simple_slice_tracker.hpp>
#include <stream_process/safe_stream_op_wrapper.hpp>

namespace stream_process
{

template< typename sp_types_t, typename pipeline_t >
class pipeline_runner_single_threaded
{
public:
    STREAM_PROCESS_TYPES
    
    typedef pipeline_t                              pipeline_type;
    typedef typename pipeline_t::stream_operators   stream_operators;
    typedef typename pipeline_t::chain_operators    chain_operators;
    typedef typename pipeline_t::stream_reader      stream_reader_type;

    typedef simple_slice_tracker< sp_types_t >      tracker_type;
    typedef std::vector< tracker_type* >            trackers;
    
    typedef safe_stream_op_wrapper< sp_types_t >    safe_stream_op_type;
    typedef std::vector< safe_stream_op_type* >     safe_stream_ops;
    
    pipeline_runner_single_threaded( pipeline_type& pipeline_ );
    ~pipeline_runner_single_threaded();
    
    int run();

protected: // functions
    void _run();
    void _run_all();
    
    inline void _run_stream_op( const size_t index_ );
    
    void    _signal_clear_stage();
    void    _signal_shutdown();
    
    void _run_debug();


protected: // variables
    pipeline_type&      _pipeline;

    stream_operators&   _stream_operators;
    chain_operators&    _chain_operators;
   
    stream_reader_type* _reader;

    safe_stream_ops     _safe_stream_ops;

}; // class pipeline_runner_single_threaded


#define SP_TEMPLATE_TYPES   template< typename sp_types_t, typename pipeline_t >
#define SP_CLASS_NAME       pipeline_runner_single_threaded< sp_types_t, pipeline_t >


SP_TEMPLATE_TYPES
SP_CLASS_NAME::pipeline_runner_single_threaded( pipeline_type& pipeline_ )
    : _pipeline( pipeline_ )
    , _stream_operators( _pipeline.get_stream_operators() )
    , _chain_operators( _pipeline.get_chain_operators() )
    , _reader( _pipeline.get_reader() )
{
    stream_operators& sops = _pipeline.get_stream_operators();
    
    typename stream_operators::iterator
        it      = sops.begin(),
        it_end  = sops.end();
    for( ; it != it_end; ++it )
    {
        stream_op_type& sop         = **it;
        safe_stream_op_type* ssop   = 0;

        if ( sop.needs_bounds_checking() )
        {
            ssop = new safe_stream_op_type( sop );
        }

        _safe_stream_ops.push_back( ssop );
    }
}



SP_TEMPLATE_TYPES
SP_CLASS_NAME::~pipeline_runner_single_threaded()
{
    typename safe_stream_ops::iterator
        it      = _safe_stream_ops.begin(),
        it_end  = _safe_stream_ops.end();
    for( ; it != it_end; ++it )
    {
        safe_stream_op_type* ssop = *it;
        if ( ssop )
            delete ssop;
    }
}



SP_TEMPLATE_TYPES
int
SP_CLASS_NAME::run()
{
    #if 0
        _run_debug();
    #else

        assert( _reader );
        while( _reader->has_more() )
        {
            _stream_operators.front()->push( 0 );
            _run();
        }

        _signal_clear_stage();
        
        typename safe_stream_ops::iterator 
            it      = _safe_stream_ops.begin(),
            it_end  = _safe_stream_ops.end();
        for( ; it != it_end; ++it )
        {
            safe_stream_op_type* sop = *it;
            if ( sop )
            {
                sop->enable_clear_stage();
                sop->run_clear_stage();
            }
        }

        _run_all();
        
        _signal_shutdown();

    #endif

    return 0;
}



SP_TEMPLATE_TYPES
void
SP_CLASS_NAME::_signal_clear_stage()
{
    foreach_ptr( _stream_operators, 
        boost::mem_fn( & stream_op_type::clear_stage ) );
    foreach_ptr( _chain_operators, 
        boost::mem_fn( & chain_op_type::clear_stage ) );

}


SP_TEMPLATE_TYPES
void
SP_CLASS_NAME::_signal_shutdown()
{
    foreach_ptr( _chain_operators,
        boost::mem_fn( &op_base_type::prepare_shutdown ) );
    foreach_ptr( _stream_operators,
        boost::mem_fn( &op_base_type::prepare_shutdown ) );
}



SP_TEMPLATE_TYPES
void
SP_CLASS_NAME::_run_debug()
{
    while( _reader->has_more() )
    {
        _reader->push( 0 );
    }
        

    const size_t size = _stream_operators.size();
    for( size_t index = 1; index < size; ++index )
    {
        _run_stream_op( index );
    }
    
    _signal_clear_stage();

    for( size_t index = 1; index < size; ++index )
    {
        _run_stream_op( index );
    }

    _signal_shutdown();
    
}




template< typename S0, typename S1 >
bool pop_while( S0& current, S1& prev )
{
    typedef typename S0::slice_type     slice_type;

//#define DEBUG_PRINT_OUT

        #ifdef DEBUG_PRINT_OUT
            std::cout << "attempt to push slices "
                << " from " << prev.get_op_name() 
                << " to " << current.get_op_name()
                << std::endl;
            if ( prev.top() == 0 )
            {
                std::cout << "failed." << std::endl;
            }
        #endif

    bool any_slice = false;
    while( slice_type* slice_ = prev.top() )
    {
        prev.pop();
        current.push( slice_ );
        any_slice = true;
            #ifdef DEBUG_PRINT_OUT
                    std::cout << "pushed slice " << slice_->get_slice_number()
                        << " from " << prev.get_op_name() 
                        << " to " << current.get_op_name()
                        << std::endl;
            #endif
    }
    return any_slice;
}


SP_TEMPLATE_TYPES
inline void
SP_CLASS_NAME::_run_stream_op( const size_t index )
{
#if 0
    assert( index > 0 );
    const size_t prev = index - 1;

    bool go_on = true;
    
    while( go_on )
    {
        go_on = pop_while( 
            *_stream_operators[ index ], 
            *_stream_operators[ prev ] 
            );
    }
#endif
    
#if 1
    assert( index > 0 );
    const size_t prev = index - 1;

    bool go_on = true;
    
    while( go_on )
    {

        if ( _safe_stream_ops[ index ] )
        {
            if ( _safe_stream_ops[ prev ] )
            {
                safe_stream_op_type* cur    = _safe_stream_ops[ index ];
                safe_stream_op_type* pre    = _safe_stream_ops[ prev ];
                assert( pre && cur );
                go_on = pop_while( *cur, *pre );
            }
            else
            {
                safe_stream_op_type* cur    = _safe_stream_ops[ index ];
                stream_op_type* pre         = _stream_operators[ prev ];
                assert( pre && cur );
                go_on = pop_while( *cur, *pre );
            }
        }
        else
        {
            if ( _safe_stream_ops[ prev ] )
            {
                stream_op_type* cur         = _stream_operators[ index ];
                safe_stream_op_type* pre    = _safe_stream_ops[ prev ];
                assert( pre && cur );
                go_on = pop_while( *cur, *pre );
            }
            else
            {
                stream_op_type* cur         = _stream_operators[ index ];
                stream_op_type* pre         = _stream_operators[ prev ];
                assert( pre && cur );
                go_on = pop_while( *cur, *pre );
            }
        }

    }
#endif
}



SP_TEMPLATE_TYPES
void
SP_CLASS_NAME::_run()
{
    const size_t size = _stream_operators.size();
    bool go_on = true;
    for( size_t index = 1; go_on && index < size; ++index )
    {
        const size_t prev = index - 1;

        if ( _safe_stream_ops[ index ] )
        {
            if ( _safe_stream_ops[ prev ] )
            {
                safe_stream_op_type* cur    = _safe_stream_ops[ index ];
                safe_stream_op_type* pre    = _safe_stream_ops[ prev ];
                assert( pre && cur );
                go_on = pop_while( *cur, *pre );
            }
            else
            {
                safe_stream_op_type* cur    = _safe_stream_ops[ index ];
                stream_op_type* pre         = _stream_operators[ prev ];
                assert( pre && cur );
                go_on = pop_while( *cur, *pre );
            }
        }
        else
        {
            if ( _safe_stream_ops[ prev ] )
            {
                stream_op_type* cur         = _stream_operators[ index ];
                safe_stream_op_type* pre    = _safe_stream_ops[ prev ];
                assert( pre && cur );
                go_on = pop_while( *cur, *pre );
            }
            else
            {
                stream_op_type* cur         = _stream_operators[ index ];
                stream_op_type* pre         = _stream_operators[ prev ];
                assert( pre && cur );
                go_on = pop_while( *cur, *pre );
            }
        }
    }
}



SP_TEMPLATE_TYPES
void
SP_CLASS_NAME::_run_all()
{
    const size_t size = _stream_operators.size();
    for( size_t index = 1; index < size; ++index )
    {
        const size_t prev = index - 1;

        if ( _safe_stream_ops[ index ] )
        {
            if ( _safe_stream_ops[ prev ] )
            {
                safe_stream_op_type* cur    = _safe_stream_ops[ index ];
                safe_stream_op_type* pre    = _safe_stream_ops[ prev ];
                assert( pre && cur );
                pop_while( *cur, *pre );
            }
            else
            {
                safe_stream_op_type* cur    = _safe_stream_ops[ index ];
                stream_op_type* pre         = _stream_operators[ prev ];
                assert( pre && cur );
                pop_while( *cur, *pre );
            }
        }
        else
        {
            if ( _safe_stream_ops[ prev ] )
            {
                stream_op_type* cur         = _stream_operators[ index ];
                safe_stream_op_type* pre    = _safe_stream_ops[ prev ];
                assert( pre && cur );
                pop_while( *cur, *pre );
            }
            else
            {
                stream_op_type* cur         = _stream_operators[ index ];
                stream_op_type* pre         = _stream_operators[ prev ];
                assert( pre && cur );
                pop_while( *cur, *pre );
            }
        }
    }
}

#undef SP_TEMPLATE_TYPES
#undef SP_CLASS_NAME

} // namespace stream_process

#endif

