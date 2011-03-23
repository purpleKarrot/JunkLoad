#include <stream_process/preprocess.hpp>

#include <stream_process/ply_converter.hpp>
#include <stream_process/sort_data_set.hpp>
#include <stream_process/reindex_faces.hpp>

#include <stream_process/find_optimal_transform.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

namespace stream_process
{

preprocessor::preprocessor( const params& params_ )
    : _params( params_ )
{
    const std::string& src      = _params.source_file;
    const std::string& result   = _params.result_file;

    std::string unsorted_filename = src;

    // if source is ply, convert...
    if ( boost::algorithm::iends_with( src, ".ply" ) )
    {
        unsorted_filename = result + ".unsorted";

        ply_converter::params ply_params;
        ply_params.source_file = src;
        ply_params.target_file = unsorted_filename;
        ply_converter ply_cv( ply_params );
        
    }
    // TODO more formats

    std::string reindex_map = result + ".reindex_map";

    _apply_optimal_transform( unsorted_filename, params_.do_optimal_transform );
   
    // sort file
    {
        sort_data_set::params sort_params;
        sort_params.in_name             = unsorted_filename;
        sort_params.out_name            = result;
        sort_params.tmp_name            = reindex_map;
        sort_params.build_index_map     = true;
        sort_params.number_of_threads   = _params.number_of_threads;
        sort_params.sort_attribute      = "position";
        
        sort_data_set sort( sort_params );
    }

    bool has_faces = false;
    {
        data_set data_set_( unsorted_filename );
        has_faces = data_set_.get_header().has_faces();

        if ( has_faces )
        {
            const data_set_header& h        = data_set_.get_header();
            const stream_data_structure& fs = h.get_face_structure();
            const size_t num_faces          = h.get_number_of_faces();
        
            const attribute& attr = fs.get_attribute( "vertex_indices" );
        
            reindex_faces::params ri_params;
            ri_params.faces_file        = unsorted_filename + ".faces";
            ri_params.reindex_map       = reindex_map;
            ri_params.number_of_faces   = num_faces;
            ri_params.index_type        = attr.get_data_type_id();
            
            reindex_faces rif( ri_params );
            
            sort_data_set::params sort_params;
            sort_params.in_name             = unsorted_filename;
            sort_params.out_name            = result;
            sort_params.tmp_name            = unsorted_filename + ".tmp";
            sort_params.build_index_map     = false;
            sort_params.number_of_threads   = _params.number_of_threads;
            sort_params.sort_attribute      = "vertex_indices";
            
            sort_data_set sort( sort_params );
        }
    }
}



void
preprocessor::_apply_optimal_transform( const std::string& filename, 
    bool full_optimal_transform )
{
    data_set data_set_( filename );
    bool has_faces = data_set_.get_header().has_faces();

    attribute_accessor< vec3f > get_position;
    get_position.set_offset( 
        data_set_
            .get_vertex_element()
            .get_structure()
            .get_attribute( "position" )
            .get_offset()
        );

    const data_set_header& h = data_set_.get_header();

    if ( full_optimal_transform )
    {
        std::cout << "preprocessor: optimal transform in progress..." << std::endl;

        attribute_accessor< vec3f > get_position;
        get_position.set_offset( 
            data_set_
                .get_vertex_element()
                .get_structure()
                .get_attribute( "position" )
                .get_offset()
            );
            
        optimal_transform< vec3f, attribute_accessor< vec3f >, 
            mapped_data_element > ot;
            
        ot.set_accessor( get_position );
        ot.analyze( data_set_.get_vertex_map() );
        ot.apply( data_set_.get_vertex_map() );

        data_set_.get_header().set_transform( ot.get_transformation_matrix() );
        
        std::cout << "transform " << ot.get_transformation_matrix() << std::endl;
    }
    else
    {
        std::cout << "preprocessor: no optimal transform step." << std::endl;
                
        vec3f min = h.get_aabb_min< float >();
        vec3f max = h.get_aabb_max< float >();
        
        vec3f diag = max-min;
        size_t index = diag.find_max_index();
        
        if ( index != 2 )
        {
            std::cout << "swapping axes " << index << " and 2 " << std::endl;
            mapped_data_element& vertices = data_set_.get_vertex_map();
            mapped_data_element::iterator
                vit     = vertices.begin(),
                vit_end = vertices.end();
            for( ; vit != vit_end; ++vit )
            {
                vec3f& v  = get_position( *vit );
                //std::cout << "pre  " << v << std::endl;
                std::swap( v[ index ], v[ 2 ] );
                //std::cout << "post " << v << std::endl;
            }
        }
    }

    data_set_.compute_aabb();
    data_set_.get_header().write_to_file( filename );

}



} // namespace stream_process

