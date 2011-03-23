#ifndef __STREAM_PROCESS__VERTEX_INDEXER__HPP__
#define __STREAM_PROCESS__VERTEX_INDEXER__HPP__

#include <stream_process/input_indexer.hpp>

namespace stream_process
{

template< typename sp_types_t >
class vertex_indexer : public input_indexer< sp_types_t >
{
public:
    STREAM_PROCESS_TYPES

    typedef input_indexer< sp_types_t >     super;

    vertex_indexer() : super(), _vertex_index_tracker( 0 ) {}
    
    vertex_indexer( const std::string& index_name )
        : super( index_name )
        , _vertex_index_tracker( 0 )
    {}

    void set_index_tracker( vertex_index_tracker& tracker_ )
    {
        _vertex_index_tracker = &tracker_;
    }
    
    void index_and_track( typename slice_type::container_type& stream_ )
    {
        typename slice_type::iterator 
            it = stream_.begin(), it_end = stream_.end();
        for( ; it != it_end; ++it )
        {
            super::_get_index( *it ) = super::_index;

            _vertex_index_tracker->add( super::_index, *it );

            ++super::_index;
        }
        
    }

    inline vertex_index_tracker& get_tracker()
    {
        assert( _vertex_index_tracker );
        return *_vertex_index_tracker;
    }

protected:
    vertex_index_tracker*   _vertex_index_tracker;

}; // class vertex_indexer

} // namespace stream_process

#endif

