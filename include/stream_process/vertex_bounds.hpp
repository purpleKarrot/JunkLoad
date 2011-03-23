#ifndef __STREAM_PROCESS__VERTEX_BOUNDS__HPP__
#define __STREAM_PROCESS__VERTEX_BOUNDS__HPP__

#include <stream_process/stream_process_types.hpp>

namespace stream_process
{

template< typename sp_types_t >
class vertex_bounds
{
public:
    STREAM_PROCESS_TYPES

    void determine_bounds( slice_type* slice_ )
    {
        stream_container_type& stream_ = slice_->get_stream( 0 );
        
        stream_data* min_   = *stream_.begin();
        sp_float_type min_z = _get_position( min_ ).z();

        stream_data* max_   = *(--stream_.end() );
        sp_float_type max_z = _get_position( max_ ).z();
        
        assert( min_z <= max_z );
        
        slice_->set( min_z, max_z, min_z, max_z );
    
    }

    void setup_attributes( op_base_type& op, stream_structure& structure_ )
    {
        op.read( structure_, "position", _get_position );
    }
    
    void setup_accessors()
    {
        _get_position.setup();
    }
    
protected:
    attribute_accessor< vec3 >      _get_position;

protected:

}; // class vertex_bounds

} // namespace stream_process

#endif

