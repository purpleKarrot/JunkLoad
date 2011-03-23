#include <stream_process/list_mode.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

namespace stream_process
{

list_mode::list_mode( const options_map& omap )
    : _options( omap )
{
    std::string filename;
    int index_begin = -1;
    int index_end   = -1;
    
    const size_t pos_size = omap.get_number_of_positionals();
    if ( ! pos_size )
    {
        print_usage();
        std::cout << "error: filename was not specified.\n" << std::endl;
        return;
    }
    
    filename = omap[ 0 ];
    
    if ( pos_size > 1 )
    {
        std::string index_string = omap[ 1 ];
        size_t idx;
        try
        {
            std::vector< std::string > tokens;
            boost::split( tokens, index_string, boost::is_any_of( "- ") );
            idx = boost::lexical_cast< size_t >( tokens[ 0 ] );
            index_begin = idx;
            if ( tokens.size() == 1 )
            {
                index_end = index_begin + 1;
            }
            else
            {
                idx = boost::lexical_cast< size_t >( tokens[ 1 ] );
                index_end = idx + 1;
            }
        }
        catch( ... )
        {
            print_usage();
            std::cout << "error: parsing arguments failed." << std::endl;
            return;
        }
    }


    _point_set.load( filename );
    
    if ( index_begin == -1 )
    {
        // print all points
        index_begin = 0; 
        index_end = _point_set.get_header().get_number_of_vertices();
    }
    else
    {
        // print range
        if ( index_begin > index_end )
        {
            std::swap( index_begin, index_end );
        }
        size_t vnum = _point_set.get_header().get_number_of_vertices();
        if ( vnum == 0 )
        {
            throw exception( "empty data set.", SPROCESS_HERE );
        }
        else if ( index_end >= static_cast< int >( vnum ) )
        {
            index_end = vnum - 1;
        }
        
    }

    // print out points
    for( int i = index_begin; i < index_end; ++i )
    {
        print_point( std::cout, i );
    }

}


void
list_mode::
print_usage()
{
    std::cout << "\nstream process inspect prints the contents of a binary file to the screen.\n";
    std::cout << "\ninspect usage: ./sp --list <filename> [index or index-range]" << std::endl;
    std::cout << "examples:      ./sp --list bunny 23      - prints point 23 in the bunny data set\n";
    std::cout << "               ./sp --list bunny 5-15    - prints points 5 to 15\n";
    std::cout << "               ./sp --list bunny         - prints all points in data set\n";
    std::cout << std::endl;
}


void
list_mode::
print_point( std::ostream& os, size_t point_index )
{
    const data_set_header& h        = _point_set.get_header();
    const stream_data_structure& ps = h.get_vertex_structure();

    attribute_helper helper;

    os << "point_index:        " << point_index << "\n";
    point_structure::const_iterator it = ps.begin(), it_end = ps.end();
    for( ; it != it_end; ++it )
    {
        const attribute& attr = **it;
        os << helper.get_as_string( _point_set, point_index, attr.get_name() );
        os << "\n";
    }
    os << "\n";

}



void
list_mode::print_attribute( std::ostream& os, const std::string& name,
    size_t point_index ) const
{
    attribute_helper helper;
    os << helper.get_as_string( _point_set, point_index, name );
}




} // namespace stream_process

