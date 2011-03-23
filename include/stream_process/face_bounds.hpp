#ifndef __STREAM_PROCESS__FACE_BOUNDARIES__HPP__
#define __STREAM_PROCESS__FACE_BOUNDARIES__HPP__

#include <stream_process/stream_process_types.hpp>

#include <stream_process/attribute_accessor.hpp>

namespace stream_process
{

template< typename sp_types_t >
class face_bounds : public sp_types_t
{
public:
    STREAM_PROCESS_TYPES

    void update_bounds( slice_type* slice_ )
    {
        assert( slice_ );
        stream_container_type& vertices = slice_->get_stream( 0 );
        stream_container_type& faces    = slice_->get_stream( 1 );
        
        uint32_t min_index = _get_vertex_index( vertices.front() );
        uint32_t max_index = _get_vertex_index( vertices.back() );

        stream_data* min_vertex = vertices.front();
        stream_data* max_vertex = vertices.back();
        
        typename stream_container_type::const_iterator
            it = faces.begin(), it_end = faces.end();
        for( ; it != it_end; ++it )
        {
            stream_data* f      = *it;
            const vec3ui& vi    = _get_vertex_indices( f );
            
            for( size_t index = 0; index < 3; ++index )
            {
                const uint32_t idx = vi( index );
                
                if ( idx < min_index )
                {
                    min_index   = idx;
                    min_vertex  = _get_vertex_ptrs( f )( index );
                }

                if ( idx > max_index )
                {
                    max_index   = idx;
                    max_vertex  = _get_vertex_ptrs( f )( index );
                }
            }
        }
    
        assert( min_vertex && max_vertex );

        sp_float_type minz = _get_vertex_position( min_vertex ).z();
        sp_float_type maxz = _get_vertex_position( max_vertex ).z();
        
        slice_->update_ring_one( minz, maxz );

        slice_->set_max_index( max_index );

    }
    
    
    void setup_attributes( op_base_type& op,
        stream_structure& vertex_structure,
        stream_structure& face_structure
        )
    {
        op.read( vertex_structure, "position", _get_vertex_position );
        op.read( vertex_structure, "point_index", _get_vertex_index );

        op.read( face_structure, "vertex_indices", _get_vertex_indices );
        op.read( face_structure, "vertex_pointers", _get_vertex_ptrs );
    }
        
    void setup_accessors()
    {
        _get_vertex_position.setup();
        _get_vertex_index.setup();
        
        _get_vertex_indices.setup();
        _get_vertex_ptrs.setup();
    }
        
    
protected:
    // vertex attributes
    attribute_accessor< vec3 >      _get_vertex_position;
    attribute_accessor< size_t >    _get_vertex_index;

    // face attributes
    attribute_accessor< vec3ui >    _get_vertex_indices;
    attribute_accessor< ptr_vec3 >  _get_vertex_ptrs;

}; // class face_boundaries

} // namespace stream_process

#endif

