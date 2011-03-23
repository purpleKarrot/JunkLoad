#ifndef __STREAM_PROCESS__STREAM_FILE_READER__HPP__
#define __STREAM_PROCESS__STREAM_FILE_READER__HPP__

#include <stream_process/stream_process_types.hpp>

#include <stream_process/input_converter.hpp>
#include <stream_process/stream_config.hpp>

#include <stream_process/input_indexer.hpp>
#include <stream_process/vertex_indexer.hpp>
#include <stream_process/input_duplicate_remover.hpp>
#include <stream_process/vertex_bounds.hpp>

#include <stream_process/input_mutator.hpp>
#include <stream_process/file_suffix_helper.hpp>

#include <stream_process/element_reader.hpp>
#include <stream_process/face_element_reader.hpp>

#include <boost/filesystem.hpp>
#include <boost/iostreams/device/mapped_file.hpp>

#include <iostream>
#include <fstream>
#include <vector>

namespace stream_process
{

template< typename operator_base_t >
class stream_file_reader : public operator_base_t
{
public:
    typedef operator_base_t                 super;
    typedef typename super::sp_types_t      sp_types_t;

    STREAM_PROCESS_TYPES 
    
    typedef element_reader< sp_types_t >    element_reader_type;
    typedef input_converter< sp_types_t >   input_converter_type;
    typedef input_indexer< sp_types_t >     input_indexer_type;
    typedef vertex_indexer< sp_types_t >    vertex_indexer_type;
    typedef vertex_bounds< sp_types_t >     vertex_bounds_type;
    typedef input_duplicate_remover< sp_types_t >    duplicate_remover_type;

    typedef input_mutator< sp_types_t >     input_mutator_t;

    stream_file_reader();
    ~stream_file_reader();

    virtual void prepare_setup();
    virtual void setup_finalize();

    virtual void setup_attributes();
    virtual void finalize_attributes();

    virtual void setup_accessors();
    
    virtual void add_config_options();
    
    virtual bool has_more()
    {
        return _number_of_points_read < _number_of_input_points;
    }

    virtual void push( slice_type* data_slice_ );

    virtual operator_base_t* clone();

    virtual bool is_multi_threadable() const    { return false; }
    virtual bool needs_bounds_checking() const  { return false; }
    
    virtual void prepare_shutdown(); // FIXME DOES NOT BELONG HERE

protected:
    virtual void _read_vertices( slice_type* slice_ );
    virtual void _remove_duplicate_vertices( stream_container_type& vertices );

    size_t          _slice_number;

    // vertex 
    char*           _in_data;

    size_t          _input_point_size_in_bytes;
    size_t          _input_batch_size_in_bytes;
    size_t          _batch_size;

    size_t          _number_of_input_points;
    size_t          _number_of_points_read;
    
    bool            _has_faces;
    bool            _remove_duplicates;
    
    input_mutator_t _input_mutator; 
    
    // vertex input handling
    element_reader_type*    _vertex_reader;

    input_converter_type    _vertex_converter;
    vertex_indexer_type     _vertex_indexer;
    vertex_bounds_type      _vertex_bounds;
    duplicate_remover_type  _duplicate_remover;
        
}; // class stream_file_reader

#define SP_TEMPLATE_TYPES           template< typename operator_base_t >
#define SP_CLASS_NAME               stream_file_reader< operator_base_t >

template< typename operator_base_t >
stream_file_reader< operator_base_t >::
stream_file_reader()
    : super()
    , _in_data( 0 )
    , _batch_size( 1000 )
    , _input_point_size_in_bytes( 0 )
    , _input_batch_size_in_bytes( 0 )
    , _number_of_input_points( 0 )
    , _number_of_points_read( 0 )
    , _slice_number( 0 )
    , _has_faces( false )
    , _remove_duplicates( true )
    , _vertex_indexer( "point_index" )
{}



template< typename operator_base_t >
stream_file_reader< operator_base_t >::
~stream_file_reader()
{
    if ( _in_data )
        delete _in_data;
    // FIXME std::cout << "removed " << _input_mutator.removed_points.size() << " duplicate points." << std::endl;
}



template< typename operator_base_t >
void
stream_file_reader< operator_base_t >::
push( slice_type* slice )
{
    if ( slice == 0 )
    {
        slice = super::_slice_manager->create();
    }

    _read_vertices( slice );

    typename slice_type::container_type& vertex_stream = slice->get_stream( 0 );

    if ( vertex_stream.empty() )
    {
        super::_slice_manager->destroy( slice );
        return;
    }

    super::_out_buffer.push_back( slice );


    #if 0
    std::cout << "read: read slice " << _slice_number << ", and " 
        << _number_of_points_read << " points in total "
        << std::endl;
    #endif
    slice->set_slice_number( _slice_number );
    ++_slice_number;
}



template< typename operator_base_t >
void
stream_file_reader< operator_base_t >::
_read_vertices( slice_type* slice )
{
    assert( slice );
    stream_container_type& vertex_stream = slice->get_stream( 0 );

    char* data          = _vertex_reader->data();
    size_t batch_size   = _vertex_reader->increment( _batch_size );

    if ( vertex_stream.size() != batch_size )
        super::_slice_manager->resize( slice->get_stream( 0 ), 0, batch_size );

    assert( vertex_stream.size() == batch_size );

    _vertex_converter.convert( data, slice->get_stream( 0 ) );
    
    _number_of_points_read += batch_size;
    
    if ( _number_of_points_read == _number_of_input_points )
        slice->set_is_last();

    // index points and remove duplicates
    //_input_mutator( slice );
    
    boost::mutex::scoped_lock lock( _vertex_indexer.get_tracker().get_mutex() );
    
    _vertex_indexer.index_and_track( vertex_stream );

    if ( _remove_duplicates )
    {
        _remove_duplicate_vertices( vertex_stream );
    }

    _vertex_bounds.determine_bounds( slice );

}



SP_TEMPLATE_TYPES
void
SP_CLASS_NAME::_remove_duplicate_vertices( stream_container_type& vertices )
{
    typedef typename duplicate_remover_type::vertex_iterators   vi_type;
    typedef typename vi_type::iterator                          vi_iter;
    typedef typename stream_container_type::iterator            stream_iter;

    vi_type& vi = _duplicate_remover.detect_duplicates( vertices );
        
    if ( vi.empty() )
        return;
        
    vi_iter
        it      = vi.begin(),
        it_end  = vi.end();
    for( ; it != it_end; ++it )
    {
        stream_iter sit     = *it;
        stream_data* vertex = *sit;
        
        // remove the vertex from the stream
        vertices.erase( sit );
        
        // dealloc memory for vertex
        super::_slice_manager->destroy_stream_data( vertex, 0 );
    }
    
    vi.clear();

}





template< typename operator_base_t >
void
stream_file_reader< operator_base_t >::
prepare_setup()
{
    stream_config& cfg          = *super::_stream_config;
    stream_options& opts        = cfg.get_options();

    // FIXME
    std::string filename_base = opts.get( "input-filename" ).get_string();
    mapped_data_set& input_data_set = cfg.setup_input_data_set( filename_base );

    point_set_header& in_header = cfg.get_input_header();
    point_structure& in_point   = in_header.get_vertex_structure();
	point_set_header& header	= cfg.get_header();
    
    _vertex_reader = new element_reader_type( in_header.get_vertex_element(), 
        filename_base );
    
    std::cout
        << "\n[input vertex structure]\n" 
        << in_header.get_vertex_structure()
        << std::endl;
            
    _input_point_size_in_bytes  = in_point.compute_size_in_bytes();   
    _number_of_input_points = in_header.get_number_of_vertices();
    
    _in_data = input_data_set.get_vertex_map().data();
    
	// setup static data in header
	header.set_aabb_min< double >( in_header.get_aabb_min< double >() );
	header.set_aabb_max< double >( in_header.get_aabb_max< double >() );
	header.set_transform< double >( in_header.get_transform< double >() );

    _remove_duplicates = ! cfg.get_options().get( "keep-duplicates" ).get_bool();

    vertex_index_tracker& vit = super::_slice_manager->get_vertex_index_tracker();
    
    _vertex_indexer.set_index_tracker( vit );
    
    if ( _remove_duplicates )
        _duplicate_remover.set_index_tracker( vit );
    
    _has_faces = in_header.has_faces();
}



template< typename operator_base_t >
void
stream_file_reader< operator_base_t >::
setup_finalize()
{
	stream_options& opts	= super::_stream_config->get_options();
    _batch_size = opts.get( "slice-size" ).get_int();

    _input_batch_size_in_bytes  = _input_point_size_in_bytes * _batch_size;

}



template< typename operator_base_t >
void
stream_file_reader< operator_base_t >::
setup_attributes()
{
    stream_config& cfg              = *super::_stream_config;
    data_set_header& in_header      = cfg.get_input_header();
    stream_structure& vs            = cfg.get_vertex_structure();

    _vertex_indexer.setup_attributes( *this, vs );
    _vertex_bounds.setup_attributes( *this, vs );

    if ( _remove_duplicates )
        _duplicate_remover.setup_attributes( *this, vs );

}



template< typename operator_base_t >
void
stream_file_reader< operator_base_t >::
setup_accessors()
{
    _vertex_indexer.setup_accessors();
    _vertex_bounds.setup_accessors();

    if ( _remove_duplicates )
        _duplicate_remover.setup_accessors();

}



template< typename operator_base_t >
void
stream_file_reader< operator_base_t >::
finalize_attributes()
{
    stream_config& cfg              = *super::_stream_config;
    data_set_header& in_header      = cfg.get_input_header();
    data_set_header& header         = cfg.get_header();
    
    bool endian_cv = in_header.get_data_is_big_endian() !=
        header.get_data_is_big_endian();

    // setup vertex input cv
    stream_data_structure& in_vs    = in_header.get_vertex_structure();
    stream_data_structure& vs       = header.get_vertex_structure();
    
    _vertex_converter.setup( in_vs, vs, endian_cv );

}



template< typename operator_base_t >
void
stream_file_reader< operator_base_t >::
add_config_options()
{
	stream_options& opts	= super::_stream_config->get_options();

    option o;
    o.setup_int( "slice-size", 1000 );
    o.set_short_name( 's' );
    o.set_help_text( "number of points per slice (batch)" );
    opts.add_option( o, "processing" );

    // misc 
    o.setup_bool( "keep-duplicates", false );
    o.set_help_text( "keep vertex duplicates in stream (warning: SVDs might fail.)" );
    opts.add_option( o, "processing" );

}



template< typename operator_base_t >
typename stream_file_reader< operator_base_t >::super*
stream_file_reader< operator_base_t >::
clone()
{
    return 0;
}


template< typename operator_base_t >
void
stream_file_reader< operator_base_t >::
prepare_shutdown()
{
    _in_data = 0;
}


#if 0
template< typename operator_base_t >
void
stream_file_reader< operator_base_t >::
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
    boost::filesystem::copy_file( in_file_name, out_file_name );     
    
    if ( _input_mutator.removed_points.empty() )
        return;

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

    // setup reindex map

    for( size_t index = 0; index < number_of_faces; ++index, data += face_size_in_bytes )
    {
        vec3ui& indices = get_vertex_indices( 
            reinterpret_cast< stream_data* >( data ) 
            );
            
        for( size_t j = 0; j < 3; ++j )
        {
            //std::cout << "face " << index << " index " << j << ": " << indices[j];
//            indices[ j ] = 
//                static_cast< uint32_t >( reindex_map_ptr[ indices[ j ] ].second );
            //std::cout << " -> " << indices[j] << std::endl;
        }
    }

}

#endif

#undef SP_CLASS_NAME
#undef SP_TEMPLATE_TYPES


} // namespace stream_process

#endif

