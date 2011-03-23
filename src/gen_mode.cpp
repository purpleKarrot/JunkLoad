#include <stream_process/gen_mode.hpp>

#include <stream_process/point_set_generator.hpp>
#include <stream_process/sort_data_set.hpp>
#include <stream_process/file_suffix_helper.hpp>

#include <boost/lexical_cast.hpp>

namespace stream_process
{

gen_mode::gen_mode( const options_map& omap )
    : _options( omap )
{
    size_t      number_of_points = 100000;
    std::string model_type  = "sphere";
    std::string filename    = "sphere";
    
    const size_t param_size = _options.get_number_of_positionals();
    
    if ( param_size > 0 )
    {
        model_type = _options[ 0 ];
    }
    if ( param_size > 1 )
    {
        number_of_points = boost::lexical_cast< size_t >( _options[ 1 ] );
    }
    if ( param_size > 2 )
    {
        filename = _options[ 2 ];
    }

    size_t number_of_threads = 8;

    options_map::const_iterator it = _options.find( "threads" );
    if ( it == _options.end() )
    {
        it = _options.find( "t" );
    }
    if ( it != _options.end() && it->second != "" )
    {
        number_of_threads = boost::lexical_cast< size_t >( it->second );
    }
    
    std::string unsorted_filename = filename + ".unsorted.tmp";
    point_set_generator psgen;
    
    if ( model_type == "sphere" )
    {
        psgen.generate_unit_sphere( unsorted_filename, number_of_points );
    }
    else
    {
        throw exception(
            std::string( "error: generator for the requested model type '" )
               + model_type + "' could not be found.", 
            SPROCESS_HERE
        );
    }
    
    sort_data_set::params params;
    params.in_name              = unsorted_filename;
    params.out_name             = filename;
    params.tmp_name             = filename + ".sort_reindex_map.tmp";
    params.build_index_map      = false;
    params.number_of_threads    = number_of_threads;
    params.sort_attribute       = "position";
    
    sort_data_set sort( params );
    
    std::cout
        << "\ngen: generated point data set from basic model " << model_type << ".\n\n" 
        << "header file " << filename << file_suffix_helper::get_header_suffix() << "\n" 
        << "data file   " << filename << file_suffix_helper::get_vertex_suffix() << "\n"
        << std::endl;
}


void
gen_mode::print_usage()
{
    std::cout << "\nstream_process gen generates data sets.\n\n";
    std::cout << "gen usage: ./sp --gen [model_type] [number_of_vertices] [filename]" << std::endl;
    std::cout << "example:   ./sp --gen sphere 1000000 test_sphere \n" << std::endl;
}



} // namespace stream_process

