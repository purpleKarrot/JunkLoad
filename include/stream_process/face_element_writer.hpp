#ifndef __STREAM_PROCESS__FACE_ELEMENT_WRITER__HPP__
#define __STREAM_PROCESS__FACE_ELEMENT_WRITER__HPP__

#include <stream_process/element_writer.hpp>

namespace stream_process
{
template< typename sp_types_t >
class face_element_writer : public element_writer< sp_types_t >
{
public:
    STREAM_PROCESS_TYPES

    typedef element_writer< sp_types_t >        super;
    typedef slice_manager< sp_types >           slice_manager_type;

    face_element_writer( data_element& data_element_,
        const std::string& filename );

    void push_back( slice_type* slice_ )
    {
        _tmp_buffer.push_back( slice_ );
        test_and_write();
    }

    slice_type* front() const
    {
        return _out_buffer.empty() ? 0 : _out_buffer.front();
    }

    void pop_front()
    {
        assert( ! _out_buffer.empty() );
        _out_buffer.pop_front();
    }
    
    void test_and_write()
    {
        assert( _slice_manager );

        slice_type* slice = _tmp_buffer.empty() ? 0 : _tmp_buffer.front();
        while( slice 
            && slice->get_max_index() < _vertex_index_tracker->get_out_index_bound() )
        {
#if 0
            std::cout
                << "face writer: test and write slice " 
                << slice->get_slice_number() << std::endl;
#endif
            _tmp_buffer.pop_front();

            // write out slice data
            _update_and_write_faces( slice->get_stream( 1 ) );

            _out_buffer.push_back( slice );

            slice = _tmp_buffer.empty() ? 0 : _tmp_buffer.front();
        }
    }

    void setup_attributes( op_base_type& op, stream_structure& structure_ )
    {
        op.read( structure_, "vertex_indices", _get_vertex_indices );
        op.read( structure_, "face_index", _get_face_index );
    }
    
    void setup_accessors()
    {
        _get_vertex_indices.setup();
        _get_face_index.setup();
    }

    void set_slice_manager( slice_manager_type* slice_man_ );

protected:
    void _update_and_write_faces( stream_container_type& faces );

    attribute_accessor< vec3ui >    _get_vertex_indices;
    attribute_accessor< size_t >    _get_face_index;

    vertex_index_tracker*           _vertex_index_tracker;
    slice_manager_type*             _slice_manager;

    // stores slice until ready to write out
    std::deque< slice_type* >       _tmp_buffer;
    // stores slices where the vertices have been written out already
    std::deque< slice_type* >       _out_buffer;

}; // class face_element_writer


template< typename sp_types_t >
face_element_writer< sp_types_t >::
face_element_writer( data_element& data_element_, const std::string& filename )
    : super( data_element_, filename )
    , _vertex_index_tracker( 0 )
{}


template< typename sp_types_t >
void
face_element_writer< sp_types_t >::
set_slice_manager( slice_manager_type* slice_man_  )
{
    assert( slice_man_ );
    _slice_manager          = slice_man_;
    _vertex_index_tracker   = & _slice_manager->get_vertex_index_tracker();
}



template< typename sp_types_t >
void
face_element_writer< sp_types_t >::
_update_and_write_faces( stream_container_type& stream_ )
{
    boost::mutex::scoped_lock( _vertex_index_tracker->get_mutex() );

    size_t& written = super::_number_of_elements_written;

#if 0
    std::cout << "slice_face_write " 
        << _get_face_index( stream_.front() ) << " to " 
        << _get_face_index( stream_.back() ) 
        << ", size " << stream_.size()
        << " dis " << std::distance( stream_.begin(), stream_.end() )
        << std::endl;
#endif

    typename slice_type::const_iterator
        it = stream_.begin(), it_end = stream_.end();
    for( ; it != it_end; ++it, ++written )
    {
        // write index to index_map FIXME
        vec3ui& vi  = _get_vertex_indices( *it );

#if 0
        vec3ui DEBUGsik = vi;

        std::cout << "fixing face " << _get_face_index( *it ) 
            << ", " << vi << ", " << DEBUGsik 
            << ", written " << written << std::endl;
            
        assert( _get_face_index( *it ) == written );
#endif
        
        assert( vi( 0 ) != (std::numeric_limits< uint32_t >::max)()
            && vi( 1 ) != (std::numeric_limits< uint32_t >::max)()
            && vi( 2 ) != (std::numeric_limits< uint32_t >::max)()
            );
        
        vi( 0 )     = _vertex_index_tracker->get_out_index( vi( 0 ) );
        vi( 1 )     = _vertex_index_tracker->get_out_index( vi( 1 ) );
        vi( 2 )     = _vertex_index_tracker->get_out_index( vi( 2 ) );
        
#if 0
        if (  vi( 0 ) == std::numeric_limits< uint32_t >::max() 
            || vi( 1 ) == std::numeric_limits< uint32_t >::max() 
            || vi( 2 ) == std::numeric_limits< uint32_t >::max() 
            )
        {
            std::cout << "face " << _get_face_index( *it ) << " is broken." << std::endl;
        }
#endif
        assert( vi( 0 ) != (std::numeric_limits< uint32_t >::max)()
            && vi( 1 ) != (std::numeric_limits< uint32_t >::max)()
            && vi( 2 ) != (std::numeric_limits< uint32_t >::max)()
            );

        super::_out_stream.write( (const char*) *it, super::_size_in_bytes );

        assert( super::_out_stream.good() );
    }
}


} // namespace stream_process

#endif

