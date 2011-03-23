#include <stream_process/sort_mode.hpp>

#include <stream_process/sort_data_set.hpp>

namespace stream_process
{

sort_mode::sort_mode( const options_map& options_ )
    : _options( options_ )
{
    std::string input_file;
    std::string output_file;
    size_t      number_of_threads = 8;
    
    if ( _options.get_number_of_positionals() < 2 )
    {
        print_usage();
        throw exception( "sort requires the base filenames of two data sets as parameters.",
            SPROCESS_HERE );
    }
    
    input_file  = _options[ 0 ];
    output_file = _options[ 1 ];
    
    std::string thread_num = "";
    
    options_map::const_iterator it = _options.find( "threads" );
    if ( it != _options.end() )
    {
        thread_num = it->second;
    }
    else
    {
        it = _options.find( "t" );
        if ( it != _options.end() )
        {
            thread_num = it->second;
        }
    }
    if ( thread_num != "" )
    {
        number_of_threads = boost::lexical_cast< size_t >( thread_num );
    }
    
    sort_data_set::params params;
    params.in_name              = input_file;
    params.out_name             = output_file;
    params.tmp_name             = input_file + ".sort_reindex_map.tmp";
    params.build_index_map      = true;
    params.number_of_threads    = number_of_threads;
    params.sort_attribute       = "position";

    sort_data_set sort( params );
    
    std::cout
        << "stream process sort: completed.\n\n"
        << "unsorted input:     " << input_file         << "\n"
        << "sorted output:      " << output_file        << "\n"
        << "sort-reindex-map:   " << params.tmp_name   
        << std::endl;
}



void
sort_mode::print_usage()
{
    std::cout << "\nstream_process sort sorts an data set.\n\n";
    std::cout << "sort usage: ./sp --sort <in_filename> <out_filename>" << std::endl;
    std::cout << "example:    ./sp --sort my_data_set   my_sorted_data_set \n" << std::endl;
}

} // namespace stream_process

