#include <stream_process/diff_mode.hpp>

namespace stream_process
{

diff_mode::diff_mode( const options_map& omap )
    : _options( omap )
{
    std::string file0, file1;
    
    const size_t pos_size = _options.get_number_of_positionals();
    if ( pos_size < 2 )
    {
        print_usage();
        throw exception( "diff requires two filenames as parameters.", 
            SPROCESS_HERE );
    }
    
    file0 = _options[ 0 ];
    file1 = _options[ 1 ];
    
    _point_sets[ 0 ].load( file0 );
    _point_sets[ 1 ].load( file1 );

    const point_set_header& h0 = _point_sets[ 0 ].get_header();
    const point_set_header& h1 = _point_sets[ 1 ].get_header();

    size_t size0 = h0.get_number_of_vertices();
    size_t size1 = h1.get_number_of_vertices();

    size_t size = std::min( size0, size1 );
       
    const stream_data_structure& ps0 = h0.get_vertex_structure();
    const stream_data_structure& ps1 = h1.get_vertex_structure();

    std::cout << "comparing '" << file0 << "' and '" << file1 <<"'";
    if ( size0 == size1 )
        std::cout << " with " << size << " points each." << std::endl;
    else
        std::cout << " with " << size0 << " resp. " << size1 << " points." << std::endl;

    
    std::list< std::string >  shared_attribs;
    std::list< std::string >  unique_attribs0;
    std::list< std::string >  unique_attribs1;
    
    point_structure::const_iterator ait = ps0.begin(), ait_end = ps0.end();
    for( ; ait != ait_end; ++ait )
    {
        const attribute& attr = **ait;
        if ( ps1.has_attribute( attr.get_name() ) )
        {
            const attribute& a1 = ps1.get_attribute( attr.get_name() );
            
            if ( attr.get_data_type_id() == a1.get_data_type_id() 
                && attr.get_number_of_elements() == a1.get_number_of_elements() )
            {
                shared_attribs.push_back( attr.get_name() );
            }
            else
            {
                unique_attribs0.push_back( attr.get_name() );
                unique_attribs1.push_back( attr.get_name() );
            }
        }
        else
            unique_attribs0.push_back( attr.get_name() );
    }
    
    ait = ps1.begin(); 
    ait_end = ps1.end();
    for( ; ait != ait_end; ++ait )
    {
        const attribute& attr = **ait;
        if ( ! ps0.has_attribute( attr.get_name() ) )
            unique_attribs1.push_back( attr.get_name() );
    }
    
    unique_attribs0.sort();
    unique_attribs1.sort();
    
    if ( ! unique_attribs0.empty() )
    {
        std::cout
            << "the following attributes are unique to or of a different type in "
            << file0 << ": ";
        std::list< std::string >::const_iterator
            lit     = unique_attribs0.begin(), 
            lit_end = unique_attribs0.end();
        for( ; lit != lit_end; ++lit )
        {
            if ( lit != unique_attribs0.begin() )
                std::cout << ", ";
            std::cout << *lit;
        }
        std::cout << std::endl;
    }
    if ( ! unique_attribs1.empty() )
    {
        std::cout
            << "the following attributes are unique to or of a different type in "
            << file1 << ": ";
        std::list< std::string >::const_iterator
            lit     = unique_attribs1.begin(), 
            lit_end = unique_attribs1.end();
        for( ; lit != lit_end; ++lit )
        {
            if ( lit != unique_attribs0.begin() )
                std::cout << ", ";
            std::cout << *lit;
        }
        std::cout << std::endl;
    }
    {
        attribute_helper helper;

        std::string diff_string = "";
        bool global_eq = true;
        std::list< std::string >::const_iterator it, it_end = shared_attribs.end();
        for( size_t index = 0; index < size; ++index )
        {
            bool are_eq = true;
            diff_string = "";
            std::string index_string = boost::lexical_cast< std::string > ( index );
            for( it = shared_attribs.begin(); it != it_end; ++it )
            {
                if ( ! are_equal( *it, index ) )
                {
                    are_eq      = false;
                    global_eq   = false;
                    diff_string += index_string;
                    diff_string += " > ";
                    diff_string += helper.get_as_string( _point_sets[ 0 ], 
                        index, *it );
                    diff_string += "\n";
                    diff_string += index_string;
                    diff_string += " < ";
                    diff_string += helper.get_as_string( _point_sets[ 1 ], 
                        index, *it );
                    diff_string += "\n";
                }
            }
            if ( ! are_eq )
            {
                std::cout << diff_string << std::endl;
            }
        }
        if ( global_eq )
        {
            std::cout
                << "no differences between '" << file0 << "' and '" << file1
                << "' found. " << std::endl; 
        }
    }



}



void
diff_mode::
print_usage()
{
    std::cout << "\nstream_process diff displays the differences between two files.\n\n";
    std::cout << "diff usage: ./sp --diff <filename0> <filename1>" << std::endl;
    std::cout << "example:    ./sp --diff bunny bunny_test\n" << std::endl;
}



} // namespace stream_process

