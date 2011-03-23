#ifndef __STREAM_PROCESS__INPUT_INDEXER__HPP__
#define __STREAM_PROCESS__INPUT_INDEXER__HPP__

#include <stream_process/stream_process_types.hpp>

namespace stream_process
{

template< typename sp_types_t >
class input_indexer
{
public:
    STREAM_PROCESS_TYPES

    input_indexer() : _index( 0 ), _index_name( "index" ) {}
    
    input_indexer( const std::string& index_name )
        : _index( 0 )
        , _index_name( index_name )
    {}


    void index( typename slice_type::container_type& stream_ )
    {
        typename slice_type::iterator 
            it = stream_.begin(), it_end = stream_.end();
        for( ; it != it_end; ++it )
        {
            _get_index( *it ) = _index;
            ++_index;
        }
    }

    void setup_attributes( op_base_type& op, stream_structure& structure_,
        const std::string& name_ = "point_index" )
    {
        op.write( structure_, name_, _get_index, false );
    }
    
    void setup_accessors()
    {
        _get_index.setup();
    }
    
protected:
    std::string                     _index_name;
    size_t                          _index;
    attribute_accessor< size_t >    _get_index;
    
    
}; // class input_indexer

} // namespace stream_process

#endif

