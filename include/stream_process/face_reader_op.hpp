#ifndef __STREAM_PROCESS__FACE_READER_OP__HPP__
#define __STREAM_PROCESS__FACE_READER_OP__HPP__

#include <stream_process/stream_process_types.hpp>

#include <stream_process/input_converter.hpp>
#include <stream_process/stream_config.hpp>

#include <stream_process/input_indexer.hpp>
#include <stream_process/file_suffix_helper.hpp>

#include <stream_process/element_reader.hpp>
#include <stream_process/face_element_reader.hpp>

#include <stream_process/face_bounds.hpp>

#include <iostream>
#include <vector>

namespace stream_process
{

template< typename operator_base_t >
class face_reader_op : public operator_base_t
{
public:
    typedef operator_base_t                 super;
    typedef typename super::sp_types_t      sp_types_t;

    STREAM_PROCESS_TYPES 
    
    typedef element_reader< sp_types_t >    element_reader_type;
    typedef input_converter< sp_types_t >   input_converter_type;
    typedef input_indexer< sp_types_t >     input_indexer_type;
    typedef vertex_indexer< sp_types_t >    vertex_indexer_type;
    typedef slice_manager< sp_types_t >     slice_manager_type;
    typedef face_bounds< sp_types_t >       face_bounds_type;

    face_reader_op();
    ~face_reader_op();

    virtual void push( slice_type* data_slice_ );

    virtual void prepare_setup();
    virtual void setup_finalize();

    virtual void setup_attributes();
    virtual void finalize_attributes();

    virtual void setup_accessors();
    
    virtual void add_config_options();
    
    virtual operator_base_t* clone() { return 0; }

    virtual bool is_multi_threadable() const    { return false; }
    virtual bool needs_bounds_checking() const  { return true; }
    
    virtual void prepare_shutdown(); // FIXME DOES NOT BELONG HERE

protected:
    virtual void _read_faces( size_t number_of_faces = 1000 );
    virtual void _setup_faces_for_slice( slice_type* slice_ );

    // face input handling
    element_reader_type*        _face_reader;

    input_converter_type        _converter;
    input_indexer_type          _indexer;

    vertex_index_tracker*       _vertex_index_tracker;
    
    face_bounds_type            _face_bounds;
    
    std::deque< stream_data* >  _face_buffer;
    stream_container_type       _tmp_stream;
    
    std::deque< slice_type* >   _tmp_buffer;


    char*           _in_data;

    size_t          _input_face_size_in_bytes;
    size_t          _batch_size;

    size_t          _number_of_input_faces;
    size_t          _number_of_faces_read;
    
    attribute_accessor< size_t >            _get_vertex_index;
    attribute_accessor< vec3ui >            _get_vertex_indices;
    attribute_accessor< ptr_vec3 >          _get_vertex_ptrs;

    size_t          _max_vertex_index;
    
    size_t          _DEBUG_total_faces;


}; // class face_reader_op

#define SP_CLASS_NAME      face_reader_op< operator_base_t >
#define SP_TEMPLATE_STRING template< typename operator_base_t >

SP_TEMPLATE_STRING
SP_CLASS_NAME::
face_reader_op()
    : super()
    , _face_reader( 0 )
    , _converter()
    , _indexer()
    , _vertex_index_tracker( 0 )
    , _face_buffer()
    , _tmp_stream()
    , _tmp_buffer()
    , _in_data( 0 )
    , _input_face_size_in_bytes( 0 )
    , _batch_size( 3000 )
    , _number_of_input_faces( 0 )
    , _number_of_faces_read( 0 )
    , _get_vertex_indices()
    , _get_vertex_ptrs()
    , _max_vertex_index( 0 )
    , _DEBUG_total_faces( 0 )
{}



SP_TEMPLATE_STRING
SP_CLASS_NAME::
~face_reader_op()
{
    assert( _face_buffer.empty() );
    assert( _tmp_buffer.empty() );
    assert( _tmp_stream.empty() );
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
push( slice_type* slice_ )
{

    boost::mutex::scoped_lock lock( _vertex_index_tracker->get_mutex() );

    _tmp_buffer.push_back( slice_ );

    // _out_buffer 
    if ( _face_reader->has_more() && _face_buffer.size() < 5000 )
    {
        _read_faces();
    }
    
    slice_ = _tmp_buffer.front();
    while( slice_ )
    {
        slice_type* last_slice = _tmp_buffer.back();

        stream_container_type& vertices = last_slice->get_stream( 0 );

        assert( ! vertices.empty() );
        const size_t latest_vertex_index  = _get_vertex_index( vertices.back() );

        #if 0
        std::cout
            << "latest vindex " << latest_vertex_index 
            << ", max face vindex " << _max_vertex_index
            << std::endl;
        #endif
        
        if ( latest_vertex_index < _max_vertex_index )
            break;

        _setup_faces_for_slice( slice_ );
        
        _tmp_buffer.pop_front();
        
        //std::cout << "while slice end " << std::endl;
    
        slice_ = _tmp_buffer.empty() ? 0 : _tmp_buffer.front();
    }
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
_setup_faces_for_slice( slice_type* slice_ )
{
    stream_container_type& vertices = slice_->get_stream( 0 );
    stream_container_type& faces    = slice_->get_stream( 1 );
    
    const size_t max_vertex_index  = _get_vertex_index( vertices.back() );

    bool stop = false;
    while ( ! stop )
    {
        size_t min_index = 0;
        size_t max_index = 0;
        
        if ( _face_buffer.empty() )
            break;
        //assert( _face_buffer.size() );

        typename std::deque< stream_data* >::iterator
            it = _face_buffer.begin(), it_end = _face_buffer.end();
        for( ; it != it_end; ++it )
        {
            vec3ui& vi      = _get_vertex_indices( *it );
            ptr_vec3& ptrs  = _get_vertex_ptrs( *it );

            assert( 
                vi( 0 ) != std::numeric_limits< uint32_t >::max() &&
                vi( 1 ) != std::numeric_limits< uint32_t >::max() &&
                vi( 2 ) != std::numeric_limits< uint32_t >::max()
                );

            min_index = vi.find_min();

            const size_t max_ = vi.find_max();
            if ( max_index < max_ )
                max_index = max_;

            if ( min_index > max_vertex_index || max_index > 
                _vertex_index_tracker->get_max_index() )
            {
                stop = true;
                break;
            }

            // replace indices to duplicate vertices
            for( size_t index = 0; index < 3; ++index )
            {
                const uint32_t orig     = vi( index );
                
                const vertex_index_tracker::index_ref& ir
                    = _vertex_index_tracker->get( orig );

                const uint32_t clean    = ir.index;
                
                // replace index if necessary
                if ( orig != clean )
                    vi( index ) = clean;

                // copy vertex ptr into face
                ptrs( index )   = ir.ptr;
            }

            assert( 
                vi( 0 ) != std::numeric_limits< uint32_t >::max() &&
                vi( 1 ) != std::numeric_limits< uint32_t >::max() &&
                vi( 2 ) != std::numeric_limits< uint32_t >::max()
                );
            //std::cout << "read face: " << vi << std::endl;

                
            faces.push_back( *it );
            _face_buffer.pop_front();
        }

        if ( it == it_end )
        {
            assert( _face_buffer.empty() );
            if ( _face_reader->has_more() )
                _read_faces( 5000 );
        }
    }

    _face_bounds.update_bounds( slice_ );
    
    super::_out_buffer.push_back( slice_ );

    #if 0
    std::cout
        << "slice " << slice_->get_slice_number()
        << " has " << slice_->get_stream( 0 ).size() << " vertices, "
        << slice_->get_stream( 1 ).size() << " faces." 
        << std::endl;
    #endif
}




SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
_read_faces( size_t number_of_faces )
{
    char* data          = _face_reader->data();
    size_t batch_size   = _face_reader->increment( number_of_faces );
    
    // FIXME DEBUG
    _DEBUG_total_faces  += batch_size;
    if ( batch_size != number_of_faces )
        assert( ! _face_reader->has_more() );

    if ( _tmp_stream.size() != batch_size )
    {
        // stream_container, stream index, new_size
        super::_slice_manager->resize( _tmp_stream, 1, batch_size );
    }
    assert( _tmp_stream.size() == batch_size );

    _converter.convert( data, _tmp_stream );
    
    _number_of_faces_read += batch_size;
    
    _indexer.index( _tmp_stream );

    typename stream_container_type::iterator
        it = _tmp_stream.begin(), it_end = _tmp_stream.end();
    for( ; it != it_end; ++it )
    {
        vec3ui& vi  = _get_vertex_indices( *it );
        
        const size_t max_index = vi.find_max();
        if ( max_index > _max_vertex_index )
            _max_vertex_index = max_index;

        _face_buffer.push_back( *it );
    }
    
    _tmp_stream.clear();

    //std::cout << "read " << _face_buffer.size() << " faces " << std::endl;
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
prepare_setup()
{
    stream_config& cfg          = *super::_stream_config;
    stream_options& opts        = cfg.get_options();

    opts.get( "process-faces" ).set_bool( true );

    std::string filename_base = opts.get( "input-filename" ).get_string();
    mapped_data_set& input_data_set = cfg.get_input_data_set();

    data_set_header& in_header      = cfg.get_input_header();
    stream_structure& in_faces      = in_header.get_face_structure();

	data_set_header& header         = cfg.get_header();
    
    _face_reader = new element_reader_type( in_header.get_face_element(), 
        filename_base );
    
    std::cout
        << "\ninput face structure:\n" 
        << in_header.get_face_structure()
        << std::endl;
            
    _input_face_size_in_bytes       = in_faces.compute_size_in_bytes();   
    _number_of_input_faces          = in_header.get_number_of_faces();
    
    _in_data = input_data_set.get_face_map().data();
    
    bool has_faces = in_header.has_faces();
    if ( ! has_faces )
        throw exception( "face reader op requires input faces.", SPROCESS_HERE );
    
    _vertex_index_tracker = & super::_slice_manager->get_vertex_index_tracker();
    
}


SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
setup_finalize()
{}


SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
setup_attributes()
{
    stream_config& cfg              = *super::_stream_config;
    data_set_header& in_header      = cfg.get_input_header();

    // vertex
    stream_structure& vs            = cfg.get_vertex_structure();
    super::read( vs, "point_index", _get_vertex_index );

    // face
    stream_structure& fs            = cfg.get_face_structure();

    std::cout << fs << std::endl;
    
    super::write( fs, "vertex_indices", _get_vertex_indices );
    super::write( fs, "vertex_pointers", _get_vertex_ptrs, false );
    
    _indexer.setup_attributes( *this, fs, "face_index" );
    
    _face_bounds.setup_attributes( *this, vs, fs );
    
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
finalize_attributes()
{
    stream_config& cfg              = *super::_stream_config;
    data_set_header& in_header      = cfg.get_input_header();
    data_set_header& header         = cfg.get_header();
    
    bool endian_cv = in_header.get_data_is_big_endian() !=
        header.get_data_is_big_endian();

    // setup vertex input cv
    stream_data_structure& in_fs    = in_header.get_face_structure();
    stream_data_structure& fs       = header.get_face_structure();
    
    _converter.setup( in_fs, fs, endian_cv );
}


SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
setup_accessors()
{
    _get_vertex_index.setup();

    _get_vertex_indices.setup();
    _get_vertex_ptrs.setup();
    
    _indexer.setup_accessors();

    _face_bounds.setup_accessors();
}

SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
prepare_shutdown()
{}


SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
add_config_options()
{
	stream_options& opts	= super::_stream_config->get_options();

    option o;
    o.setup_bool( "process-faces", false )
        .set_help_text( "process faces" );
    
    opts.add_option( o, "processing" );
}



#undef SP_CLASS_NAME
#undef SP_TEMPLATE_STRING

} // namespace stream_process

#endif

