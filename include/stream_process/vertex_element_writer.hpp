#ifndef __STREAM_PROCESS__VERTEX_ELEMENT_WRITER__HPP__
#define __STREAM_PROCESS__VERTEX_ELEMENT_WRITER__HPP__

#include <stream_process/element_writer.hpp>
#include <stream_process/slice_manager.hpp>

namespace stream_process
{
template< typename sp_types_t >
class vertex_element_writer : public element_writer< sp_types_t >
{
public:
    STREAM_PROCESS_TYPES

    typedef element_writer< sp_types_t >        super;
    typedef slice_manager< sp_types >           slice_manager_type;

    vertex_element_writer( data_element& data_element_,
        const std::string& filename );
        
    ~vertex_element_writer();
    
    void set_slice_manager( slice_manager_type* slice_man_ );
    
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

    void setup_attributes( op_base_type& op, stream_structure& structure_ )
    {
        op.read( structure_, "point_index", _get_index );
    }
    
    void setup_accessors()
    {
        _get_index.setup();
    }
    
    void test_and_write();
    void test_and_write_shutdown();

    bool _clear_stage;

protected:
    void _write_and_update_indices( const stream_container_type& stream_ );
    
    
    attribute_accessor< size_t >    _get_index;
    vertex_index_tracker*           _vertex_index_tracker;
    slice_manager_type*             _slice_manager;
    
    // stores slice until ready to write out
    std::deque< slice_type* >       _tmp_buffer;
    // stores slices where the vertices have been written out already
    std::deque< slice_type* >       _out_buffer;
    
    
}; // class vertex_element_writer



template< typename sp_types_t >
vertex_element_writer< sp_types_t >::
vertex_element_writer( data_element& data_element_, const std::string& filename )
    : super( data_element_, filename )
    , _vertex_index_tracker( 0 )
    , _clear_stage( false )
{}


template< typename sp_types_t >
vertex_element_writer< sp_types_t >::
~vertex_element_writer()
{
    assert( _tmp_buffer.empty() );
    assert( _out_buffer.empty() );
}



template< typename sp_types_t >
void
vertex_element_writer< sp_types_t >::
test_and_write()
{
    boost::mutex::scoped_lock lock( _vertex_index_tracker->get_mutex() );

    assert( _slice_manager );

    slice_type* slice = _tmp_buffer.empty() ? 0 : _tmp_buffer.front();
    while( slice && _slice_manager->is_safe_to_stream_out( slice ) )
    {
#if 0
        std::cout
            << "vertex writer: test and write slice " 
            << slice->get_slice_number() << std::endl;
#endif
        // write out slice data
        _write_and_update_indices( slice->get_stream( 0 ) );

        _tmp_buffer.pop_front();
        _out_buffer.push_back( slice );

        slice = _tmp_buffer.empty() ? 0 : _tmp_buffer.front();
    }

}



template< typename sp_types_t >
void
vertex_element_writer< sp_types_t >::
test_and_write_shutdown()
{
    boost::mutex::scoped_lock lock( _vertex_index_tracker->get_mutex() );

    assert( _slice_manager );

    slice_type* slice = _tmp_buffer.empty() ? 0 : _tmp_buffer.front();
    while( slice )
    {
#if 0
        std::cout
            << "vertex writer: test and write slice " 
            << slice->get_slice_number() << std::endl;
#endif
        // write out slice data
        _write_and_update_indices( slice->get_stream( 0 ) );

        _tmp_buffer.pop_front();
        _out_buffer.push_back( slice );

        slice = _tmp_buffer.empty() ? 0 : _tmp_buffer.front();
    }

}



template< typename sp_types_t >
void
vertex_element_writer< sp_types_t >::
_write_and_update_indices(
    const typename slice_type::container_type& stream_ )
{
    size_t& written = super::_number_of_elements_written;

    typename slice_type::const_iterator
        it      = stream_.begin(),
        it_end  = stream_.end();
    for( ; it != it_end; ++it, ++written )
    {
        // write index to index_map FIXME
        const size_t index = _get_index( *it );

        _vertex_index_tracker->set_out_index( index, written );

        super::_out_stream.write( (const char*) *it, super::_size_in_bytes );

        assert( super::_out_stream.good() );
    }
}




template< typename sp_types_t >
void
vertex_element_writer< sp_types_t >::
set_slice_manager( slice_manager_type* slice_man_  )
{
    assert( slice_man_ );
    _slice_manager          = slice_man_;
    _vertex_index_tracker   = & _slice_manager->get_vertex_index_tracker();
}


} // namespace stream_process

#endif

