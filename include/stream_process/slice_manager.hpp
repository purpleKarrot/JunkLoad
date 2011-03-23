#ifndef __STREAM_PROCESS__SLICE_MANAGER__HPP__
#define __STREAM_PROCESS__SLICE_MANAGER__HPP__

#include <stream_process/stream_process_types.hpp>
#include <stream_process/stream_data_manager.hpp>
#include <stream_process/slice_tracker.hpp>
#include <stream_process/vertex_index_tracker.hpp>
#include <stream_process/stream_stats.hpp>

#include <boost/pool/pool.hpp>

#include <stack>

namespace stream_process
{

template< typename sp_types_t >
class slice_manager : public sp_types_t
{
public:
    STREAM_PROCESS_TYPES
    
    typedef stream_data_manager< sp_types_t >       stream_data_manager_t;
    typedef slice_tracker< sp_types_t >             slice_tracker_type;
    typedef std::list< stream_data* >               stream_type;

    slice_manager();
    ~slice_manager();
    
    // computes the stream element sizes from config
    void setup( stream_config& config_ );
    // sets up only the slices with gives stream element sizes
    void setup( const std::vector< size_t >& streams_config );
    
    

    inline void add( slice_type* slice );
    inline void remove( slice_type* slice );
    inline bool is_safe_to_stream_out( slice_type* slice );
    
    void resize( stream_container_type& stream_,
        size_t stream_index, size_t target_size );
    
    inline slice_type* create();
    inline void destroy( slice_type* slice );
    
    inline stream_data* create_stream_data( size_t stream_index = 0 );
    inline void destroy_stream_data( stream_data* stream_data_,
        size_t stream_index );
    

    slice_tracker_type&     get_slice_tracker();
    vertex_index_tracker&   get_vertex_index_tracker();

protected:
    void _do_stats( slice_type* slice );

    // stream_data pools
    std::vector< stream_data_manager_t >        _pools;
    std::vector< stream_stats >                 _stream_stats;

    boost::pool<>                               _slice_factory;
    std::stack< slice_type* >                   _free_slices;

    slice_tracker_type                          _slice_tracker;
    vertex_index_tracker                        _vertex_index_tracker;
    
    
}; // class slice_dependency_manager



template< typename sp_types_t >
slice_manager< sp_types_t >::
slice_manager()
    : _slice_factory( sizeof( slice_type ) )
{}



template< typename sp_types_t >
slice_manager< sp_types_t >::
~slice_manager()
{
    _pools.clear();
    
    std::cout
        << "pool stats:\n";

    typename std::vector< stream_stats >::const_iterator
        it = _stream_stats.begin(), it_end = _stream_stats.end();
    for( size_t index = 0; it != it_end; ++it, ++index )
    {
        const stream_stats& ps = *it;
        std::cout << "stream " << index << " max " << ps.max() << ", current "
            << ps.current() << "\n";
    }
    std::cout << std::endl;
    
}



template< typename sp_types_t >
void
slice_manager< sp_types_t >::
setup( stream_config& config_ )
{
    // get the number of streams and compute size of the data element 
    // ( point, face, ... ) in each stream
    
    stream_options& options = config_.get_options();
    size_t batch_size       = options.get( "slice-size" ).get_int();
    
    std::vector< size_t > element_sizes;
    
    data_set_header& header = config_.get_header();
    typename data_set_header::iterator 
        it      = header.begin(),
        it_end  = header.end();
    for( size_t index = 0; it != it_end; ++it, ++index )
    {
        stream_structure& s = (*it)->get_structure();
        element_sizes.push_back( s.compute_size_in_bytes() );
        // std::cout << "structure " << index << " -> " 
        //    << s.compute_size_in_bytes() << std::endl;
    }

    setup( element_sizes );
}



template< typename sp_types_t >
void
slice_manager< sp_types_t >::
setup( const std::vector< size_t >& streams_ )
{
    assert( _pools.empty() );
    
    _pools.resize( streams_.size() );
    _stream_stats.resize( _pools.size() );
    
    typename std::vector< size_t >::const_iterator
        it = streams_.begin(), it_end = streams_.end();
    for( size_t index = 0; it != it_end; ++it, ++index )
    {
        _pools[ index ].init( *it );
    }
}



template< typename sp_types_t >
void
slice_manager< sp_types_t >::
add( slice_type* slice )
{
    _slice_tracker.insert( slice );
}


template< typename sp_types_t >
void
slice_manager< sp_types_t >::
remove( slice_type* slice )
{
    _slice_tracker.remove( slice );
}


template< typename sp_types_t >
bool
slice_manager< sp_types_t >::
is_safe_to_stream_out( slice_type* slice )
{
    return _slice_tracker.is_safe( slice );
}



template< typename sp_types_t >
inline typename slice_manager< sp_types_t >::slice_type*
slice_manager< sp_types_t >::
create()
{
#define SLICE_DEBUG

    slice_type* slice = 0;
#ifdef SLICE_DEBUG
    if ( ! _free_slices.empty() )
    {
        slice = _free_slices.top();
        _free_slices.pop();
    }
    else
    {
#endif     
        slice = reinterpret_cast< slice_type* >( _slice_factory.malloc() );
        new ( slice ) slice_type( _pools.size() );
#ifdef SLICE_DEBUG
    }
    
#endif    
    assert( slice );
    return slice;
}



template< typename sp_types_t >
inline void
slice_manager< sp_types_t >::
destroy( slice_type* slice )
{
    _do_stats( slice );

    const size_t s = _pools.size();
    for( size_t index = 1; index < s; ++index )
    {
        stream_data_manager_t& pool = _pools[ index ];
        stream_stats& ps            = _stream_stats[ index ];
        stream_container_type& sct  = slice->get_stream( index );
        
        while( ! sct.empty() )
        {
            pool.destroy( sct.front() );
            sct.pop_front();
            --ps;
        }
    }
    //_free_slices.push( slice );
    _slice_factory.free( slice );
}



template< typename sp_types_t >
inline void
slice_manager< sp_types_t >::
_do_stats( slice_type* slice )
{   
    for( size_t index = 0; index < _pools.size(); ++index )
    {
        _stream_stats[ index ] -= slice->get_stream( index ).size();
    }

    const size_t sn = slice->get_slice_number();

    if ( sn % 25 )
        return;
    // else print stats to console

    size_t vs = slice->get_stream( 0 ).size();
    size_t fs = _pools.size() > 1 ? slice->get_stream( 1 ).size() : 0;

    std::cout 
        << "slice " << sn << " completed: " << vs << " vertices";
    if ( fs )
    {
        std::cout << ", " << fs << " faces";
    }
    std::cout << ". Total: " << _stream_stats.front().total() << " vertices";
    if ( fs )
    {
        std::cout << ", " << _stream_stats[ 1 ].total() << " faces";
    }
    
    std::cout << " completed." << std::endl;
}



template< typename sp_types_t >
inline stream_data*
slice_manager< sp_types_t >::
create_stream_data( size_t stream_index )
{
    assert( stream_index < _pools.size() );
    _stream_stats[ stream_index ];
    return _pools[ stream_index ].create();   
}



template< typename sp_types_t >
inline void
slice_manager< sp_types_t >::
destroy_stream_data( stream_data* stream_data_, size_t stream_index )
{
    assert( stream_index < _pools.size() );
    --_stream_stats[ stream_index ];
    return _pools[ stream_index ].destroy( stream_data_ );   
}



template< typename sp_types_t >
void
slice_manager< sp_types_t >::
resize( stream_container_type& s, size_t stream_index, size_t target_size )
{
    assert( stream_index < _pools.size() );

    stream_data_manager_t&  sd_pool     = _pools[ stream_index ];
    stream_stats& stats                 = _stream_stats[ stream_index ];

    const size_t size_ = s.size();

    if ( size_ == target_size )
        return;

    else if ( size_ < target_size )
    {
        size_t diff = target_size - size_;
        for( size_t index = 0; index < diff; ++index )
        {
            s.push_back( sd_pool.create() );
            ++stats;
        }
    }
    else
    {
        size_t diff = size_ - target_size;
        for( size_t index = 0; index < diff; ++index )
        {
            sd_pool.destroy( s.back() );
            s.pop_back();
            --stats;
        }
    }
}



template< typename sp_types_t >
slice_tracker< sp_types_t >&
slice_manager< sp_types_t >::
get_slice_tracker()
{
    return _slice_tracker;
}



template< typename sp_types_t >
vertex_index_tracker&
slice_manager< sp_types_t >::
get_vertex_index_tracker()
{
    return _vertex_index_tracker;
}


} // namespace stream_process

#endif

