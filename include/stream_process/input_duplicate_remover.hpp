#ifndef __STREAM_PROCESS__INPUT_DUPLICATE_REMOVER__HPP__
#define __STREAM_PROCESS__INPUT_DUPLICATE_REMOVER__HPP__

#include <stream_process/stream_process_types.hpp>

#include <stream_process/reindex_map.hpp>

#include <iostream>

namespace stream_process
{

template< typename sp_types_t >
class input_duplicate_remover
{
public:
    STREAM_PROCESS_TYPES
    
    typedef typename std::vector< typename slice_type::iterator >
        vertex_iterators;
       
    input_duplicate_remover() : _vertex_index_tracker( 0 ) {}
    
    vertex_iterators&
    detect_duplicates( stream_container_type& stream_ )
    {
        typename slice_type::iterator
            it      = stream_.begin(),
            it_end  = stream_.end();
        for( ; it != it_end; ++it )
        {
            stream_data* point      = *it;
            const size_t index      = _get_index( point );
            
            const vec3& position    = _get_position( point );
            
            if ( position.z() == _last_position.z() )
            {
                if ( position.x() == _last_position.x()
                    && position.y() == _last_position.y() )
                {
                    _kill_list.push_back( it );
                    _vertex_index_tracker->overwrite( index, _original_index, _last_point );
                    std::cout << "input vertex " << index 
                        << " is a duplicate of " << _original_index 
                        << " and will be removed from the stream."
                        << std::endl;
                }
                else
                {
                    _original_index = index;
                    _last_position  = position;
                    _last_point     = point;
                }
            }
            else
            {
                _original_index = index;
                _last_position  = position;
                _last_point     = point;
            }
        }
        return _kill_list;
    }
    
    
    void remove_duplicates( stream_container_type& stream_ )
    {
        detect_duplicates( stream_ );
        
        if ( _kill_list.empty() )
            return;
        
        typename std::vector< typename slice_type::iterator >::iterator
            kit         = _kill_list.begin(), 
            kit_end     = _kill_list.end();
        for( ; kit != kit_end; ++kit )
        {
            stream_.erase( *kit );
        }

        _kill_list.clear();
    }

    
    void setup_attributes( op_base_type& op, stream_structure& structure_ )
    {
        op.read( structure_,    "position",     _get_position );
        op.read( structure_,    "point_index",  _get_index );
    }
    
    void setup_accessors()
    {
        _get_position.setup();
        _get_index.setup();
    }
    
    void set_index_tracker( vertex_index_tracker& tracker_ )
    {
        _vertex_index_tracker = &tracker_;
    }

protected:
    attribute_accessor< vec3 >      _get_position;
    attribute_accessor< size_t >    _get_index;
    
    size_t                          _original_index;
    vec3                            _last_position;
    stream_data*                    _last_point;
    
    vertex_index_tracker*           _vertex_index_tracker;

    vertex_iterators                _kill_list;


}; // class input_vertex_duplicate_remover

} // namespace stream_process

#endif

