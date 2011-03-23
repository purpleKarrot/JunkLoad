#ifndef __STREAM_PROCESS__STREAM_FILE_WRITER__HPP__
#define __STREAM_PROCESS__STREAM_FILE_WRITER__HPP__

#include <stream_process/stream_config.hpp>
#include <stream_process/stream_data.hpp>

#include <stream_process/foreach.hpp>
#include <stream_process/functors.hpp>

#include <stream_process/reindex_map.hpp>

#include <stream_process/element_writer.hpp>
#include <stream_process/vertex_element_writer.hpp>
#include <stream_process/face_element_writer.hpp>

#include <stream_process/file_suffix_helper.hpp>

#include <iostream>
#include <fstream>
#include <list>
#include <queue>
#include <vector>

namespace stream_process
{
template< typename operator_base_t >
class stream_file_writer : public operator_base_t
{
public:
    typedef operator_base_t                 super;
    typedef typename super::sp_types_t      sp_types_t;

    STREAM_PROCESS_TYPES
    
    typedef element_writer< sp_types_t >        element_writer_type;
    typedef vertex_element_writer< sp_types_t > vertex_writer_type;
    typedef face_element_writer< sp_types_t >   face_writer_type;
   
    stream_file_writer();
    virtual ~stream_file_writer();
    
    virtual void push( slice_type* data_slice_ );

    virtual void prepare_setup();
    virtual void setup_attributes();
    virtual void finalize_attributes();
    virtual void setup_accessors();

    virtual void prepare_shutdown();
    
    virtual operator_base_t* clone();

    virtual bool is_multi_threadable() const    { return false; }
    virtual bool needs_bounds_checking() const  { return false; }

protected:
    virtual void _write_out_slices();
    virtual void _write_out_vertices();
    virtual void _write_out_faces();
    
    void _HACK_fix_faces();
    
    std::ofstream               _out_stream;
    size_t                      _number_of_points_written;
    size_t                      _output_point_size_in_bytes;
    size_t                      _next_slice_number;
    
    size_t                      _last_vertex_index;
    
    std::deque< slice_type* >   _out_buffer;
    std::deque< slice_type* >   _face_out_buffer;
    
    attribute_accessor< size_t >  _get_index;

    reindex_map< size_t >       _reindex_map;
    
    vertex_index_tracker*       _vertex_index_tracker;
    
    bool                        _has_faces;

    vertex_writer_type*         _vertex_writer;
    face_writer_type*           _face_writer;

}; // class stream_file_writer



template< typename operator_base_t >
stream_file_writer< operator_base_t >::
stream_file_writer()
    : _number_of_points_written( 0 )
    , _output_point_size_in_bytes( 0 )
    , _next_slice_number( 0 )
    , _last_vertex_index( 0 )
    , _vertex_index_tracker( 0 )
    , _has_faces( false )
    , _vertex_writer( 0 )
    , _face_writer( 0 )
{}



template< typename operator_base_t >
stream_file_writer< operator_base_t >::
~stream_file_writer()
{
    if ( _vertex_writer )
        delete _vertex_writer;
    if ( _face_writer )
        delete _face_writer;

}



template< typename operator_base_t >
void
stream_file_writer< operator_base_t >::
prepare_setup()
{
    const stream_options& options_  = super::_stream_config->get_options();

    std::string filename_base = options_.get( "output-filename" ).get_string();

    #if 0
    _out_stream.open( data_file.c_str() );
    if ( ! _out_stream.is_open() )
    {
        throw exception(
            std::string( "opening data file " ) + data_file 
            + " failed.",
            SPROCESS_HERE 
            );
    }
    #else

    data_set_header& header     = super::_stream_config->get_header();
    data_set_header& in_header  = super::_stream_config->get_input_header();

    // vertices
    std::string vertex_suffix   = file_suffix_helper::get_vertex_suffix();
    std::string vertex_file     = filename_base + vertex_suffix;
    data_element& vertices      = header.get_vertex_element();
    
    _vertex_writer = new vertex_writer_type( vertices, vertex_file );
    
    #endif
    
    _vertex_index_tracker = & super::_slice_manager->get_vertex_index_tracker();
    
    _vertex_writer->set_slice_manager( super::_slice_manager );
    
    
    // faces 
    // FIXME
    _has_faces = options_.get( "process-faces" ).get_bool();
    
    if ( ! _has_faces )
        return;
    
    std::string face_suffix = file_suffix_helper::get_face_suffix();
    std::string face_file   = filename_base + face_suffix;
    data_element& faces     = header.get_face_element();
    
    _face_writer = new face_writer_type( faces, face_file );
    _face_writer->set_slice_manager( super::_slice_manager );
    
}



template< typename operator_base_t >
void
stream_file_writer< operator_base_t >::
setup_attributes()
{
    stream_structure& vs = super::_stream_config->get_vertex_structure();

	super::read( vs, "point_index",	_get_index );

    assert( _vertex_writer );
    _vertex_writer->setup_attributes( *this, vs );

    if ( ! _face_writer )
        return;
    
    stream_structure& fs = super::_stream_config->get_face_structure();
    _face_writer->setup_attributes( *this, fs );

}



template< typename operator_base_t >
void
stream_file_writer< operator_base_t >::
finalize_attributes()
{
    _vertex_writer->setup();
    
    if ( _face_writer )
        _face_writer->setup();
}



template< typename operator_base_t >
void
stream_file_writer< operator_base_t >::
setup_accessors()
{
	_get_index.setup();

    _vertex_writer->setup_accessors();

    if ( _face_writer )
        _face_writer->setup_accessors();
}



template< typename operator_base_t >
void
stream_file_writer< operator_base_t >::
push( slice_type* slice )
{
    assert( slice );
    assert( slice->get_slice_number() == _next_slice_number );
    ++_next_slice_number;
    
    // remove slice from stream and put into write buffer
    super::_slice_manager->remove( slice );
    _vertex_writer->push_back( slice );

    // write out ready slices
    while ( ( slice = _vertex_writer->front() ) )
    {
        _vertex_writer->pop_front();
        
        if ( _face_writer )
            _face_writer->push_back( slice );
        else
        {
            super::_slice_manager->destroy( slice );
        }
    }
    
    if ( ! _face_writer )
        return;

    slice = _face_writer->front();
    while( slice )
    {
        _face_writer->pop_front();
        super::_slice_manager->destroy( slice );

        slice = _face_writer->front();
    }
}



template< typename operator_base_t >
void
stream_file_writer< operator_base_t >::
_write_out_slices()
{
}



template< typename operator_base_t >
void
stream_file_writer< operator_base_t >::
_write_out_vertices()
{
#if 0
    slice_type* slice = _out_buffer.front();
    while( slice && super::_slice_manager->is_safe_to_stream_out( slice ) )
//        && /* DEBUG */ _out_buffer.size() > 1 )
    {
        assert( slice->get_slice_number() == _next_slice_number );
        
        // write out slice data
        const typename slice_type::container_type& vertices = slice->get_stream( 0 );

        _vertex_writer->write_and_update_indices( vertices );

        _next_slice_number = slice->get_slice_number() + 1;
        
        if ( 0 ) //_next_slice_number % 100 == 1 )
        {
            std::cout
                << "write: wrote ";
            slice->print();
            std::cout << std::endl;
        }

        _out_buffer.pop_front();
        _face_out_buffer.push_back( slice );

        slice = _out_buffer.empty() ? 0 : _out_buffer.front();
    }



#if 0
	size_t ss = slice->size();
	std::cout
		<< "writing " << ss << " points, " 
		<< _output_point_size_in_bytes * ss << " bytes of slice "
        << slice->get_slice_number() 
		<< std::endl;
#endif
#endif
}



template< typename operator_base_t >
void
stream_file_writer< operator_base_t >::
_write_out_faces()
{

// super::_slice_manager->destroy( slice );

}


template< typename operator_base_t >
void
stream_file_writer< operator_base_t >::
prepare_shutdown()
{
    _vertex_writer->test_and_write_shutdown();

    if ( ! _face_writer )
    {
        slice_type* slice = _vertex_writer->front();
        while( slice )
        {
            _vertex_writer->pop_front();
            super::_slice_manager->destroy( slice );
            slice = _vertex_writer->front();
        }
    }
    else
    {
        slice_type* slice = _vertex_writer->front();
        while( slice )
        {
            _vertex_writer->pop_front();
            _face_writer->push_back( slice );
            slice = _vertex_writer->front();
        }
        
        _face_writer->test_and_write();

        slice = _face_writer->front();
        while( slice )
        {
            _face_writer->pop_front();
            super::_slice_manager->destroy( slice );
            slice = _face_writer->front();
        }
    }

    const size_t vwritten = _vertex_writer->get_number_of_elements_written();

    std::cout
        << "write: vertices written "
        << vwritten
        << std::endl;
    
    delete _vertex_writer;
    _vertex_writer = 0;

    stream_config& cfg      = *super::_stream_config;
    stream_options& opts    = cfg.get_options();

    data_set_header& h      = cfg.get_header();
    h.set_number_of_vertices( vwritten );

    const size_t fwritten =
        _face_writer ? _face_writer->get_number_of_elements_written() : 0;

    if ( ! _has_faces )
        h.set_number_of_faces( 0 );
    else
    {
        h.set_number_of_faces( fwritten );
        delete _face_writer;
        _face_writer = 0;
    }

    std::string outfile = opts.get( "output-filename" ).get_string();
    h.write_to_file( outfile );
    
    std::cout
        << "wrote output to '" << outfile
        << file_suffix_helper::get_header_suffix()
        << "' (header) and '"
        << outfile
        << file_suffix_helper::get_vertex_suffix()
        <<"' (vertices)";

    if ( fwritten )
    {
        std::cout << " and '"
            << outfile 
            << file_suffix_helper::get_face_suffix()
            << "' (faces)";
    }

    std::cout << "." << std::endl;
}



template< typename operator_base_t >
typename stream_file_writer< operator_base_t >::super*
stream_file_writer< operator_base_t >::
clone()
{
    return 0;
}



template< typename operator_base_t >
void
stream_file_writer< operator_base_t >::
_HACK_fix_faces()
{
    // we copy the face file and fix the indices of the duplicate vertices
    stream_config& cfg          = *super::_stream_config;
    stream_options& opts        = cfg.get_options();

    mapped_data_set& input_data_set = cfg.get_input_data_set();

    if ( ! input_data_set.get_header().has_faces() )
        return;

    std::cout << "copying faces..." << std::endl;

    const std::string in_file_name  = opts.get( "input-filename" ).get_string()
        + file_suffix_helper::get_face_suffix();
    const std::string out_file_name = opts.get( "output-filename" ).get_string()
        + file_suffix_helper::get_face_suffix();
    
    // copy in to out file and map outfile
    if ( boost::filesystem::exists( out_file_name ) )
    {
        boost::filesystem::remove( out_file_name );
    }
    boost::filesystem::copy_file( in_file_name, out_file_name );     
    
    // setup face accessor
    data_element& face_element = input_data_set.get_face_element();
    attribute_accessor< vec3ui >    get_vertex_indices;
    get_vertex_indices.set_offset( 
        face_element
        .get_structure()
        .get_attribute( "vertex_indices" )
        .get_offset()
        );
    
    // 

    std::cout << "fixing faces ... " << std::endl;
    
    // setup face reading
    boost::iostreams::mapped_file face_map( out_file_name );
    char* data = face_map.data() + face_element.get_offset();
    size_t face_size_in_bytes = face_element.get_size_in_bytes();

    size_t number_of_faces = face_element.size();

    // reindex

    for( size_t index = 0; index < number_of_faces; ++index, data += face_size_in_bytes )
    {
        vec3ui& indices = get_vertex_indices( 
            reinterpret_cast< stream_data* >( data ) 
            );
            
        for( size_t j = 0; j < 3; ++j )
        {
            //std::cout << "face " << index << " index " << j << ": " << indices[j];
            indices[ j ] = 
                static_cast< uint32_t >( _reindex_map[ indices[ j ] ] );
            //std::cout << " -> " << indices[j] << std::endl;
        }
    }
    std::cout << "faces file: " << out_file_name << std::endl;

}

} // namespace stream_process

#endif

