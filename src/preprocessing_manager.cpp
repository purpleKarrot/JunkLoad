#include "preprocessing_manager.hpp"

#include "mapped_point_data.hpp"
#include "source_data_loader.hpp"
#include "optimal_transform.hpp"

#include <cmath>

namespace stream_process
{

preprocessing_manager::preprocessing_manager()
    : _mapped_point_data( 0 )
    , _preprocess_shared_data()
    , _optimal_transform( 0 )
    , _sorting_controller( 0 )
{}



preprocessing_manager::~preprocessing_manager()
{
    delete _optimal_transform;
    delete _sorting_controller;
}



void
preprocessing_manager::setup( const std::string& in_file, 
    const std::string& sorted_file, bool apply_ot )
{
    // convert source data and copy binary data into mmapped file.
    assert( _mapped_point_data );
    source_data_loader loader;
    _mapped_point_data->setup_empty_point_info();


    std::cout
        << "\npreprocess: loading and parsing input data... (may take a while)"
        << std::endl;

    loader.load( in_file, sorted_file, *_mapped_point_data );


    std::cout
        << "preprocess: transforming and sorting the input data."
        << std::endl;
    
    if ( apply_ot )
    {
        // we transform the model so that the z axis is the dominant axis
        // to reduce memory usage during streaming

        if ( _optimal_transform )
            delete _optimal_transform;
        _optimal_transform = new optimal_transform( *_mapped_point_data );
        _optimal_transform->apply_optimal_transform();
        
        _preprocess_shared_data.set_transformation(
            _optimal_transform->get_transform()
            );
            
        _mapped_point_data->get_point_info().set_transform( 
            _optimal_transform->get_transform()
            );
    }

    _sorting_controller = new sorting_controller( * _mapped_point_data );
}



void
preprocessing_manager::process_multi_threaded()
{
    size_t point_count = _mapped_point_data->get_point_info().get_point_count();

    _sorting_controller->setup();
    _sorting_controller->sort_multi_threaded();
    size_t min_unsorted = _sorting_controller->compute_and_get_min_unsorted_index();
    while( min_unsorted < point_count )
    {
        min_unsorted = _sorting_controller->compute_and_get_min_unsorted_index();
        _preprocess_shared_data.set_min_unsorted_index( min_unsorted );       
        usleep( 25 );
    } 

    _mapped_point_data->write_header();

    
}



void
preprocessing_manager::process_single_threaded()
{
    _sorting_controller->setup();
    _sorting_controller->sort();


    #if 1
    size_t point_count = _mapped_point_data->get_point_info().get_point_count();
    LOGINFO 
        << "preprocessing finished: " 
        << point_count
        << " points sorted.\n" 
        << std::endl;
    #endif
    _mapped_point_data->write_header();
}


void
preprocessing_manager::set_mapped_point_data( mapped_point_data* mapped_point_data_ )
{
    _mapped_point_data = mapped_point_data_;
}



preprocess_shared_data&
preprocessing_manager::get_preprocess_shared_data()
{
    return _preprocess_shared_data;
}

} // namespace stream_process

