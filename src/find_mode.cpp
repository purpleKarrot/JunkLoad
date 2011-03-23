#include <stream_process/find_mode.hpp>

namespace stream_process
{

find_mode::
find_mode( const options_map& options_ )
    : _options( options_ )
{
    std::string filename, attribute_name, attribute_value;
    
    if ( _options.get_number_of_positionals() < 3 )
    {
        print_usage();
        if ( _options.get_number_of_positionals() == 0 )
            return;
        throw exception( "invalid or missing parameters.", SPROCESS_HERE );
    }
    
    _filename       = _options[ 0 ];
    attribute_name  = _options[ 1 ];
    attribute_value = _options[ 2 ];
    
    _point_set.load( _filename );
    
    const data_set_header& h            = _point_set.get_header();
    const stream_data_structure& ps     = h.get_vertex_structure();
    
    if ( ! ps.has_attribute( attribute_name ) )
    {
        std::cout << "error : data set '" << _filename << " does not have "
            << "an attribute with name '" << attribute_name << "'."
            << std::endl;
        return;
    }
    
    _attr = ps.get_attribute( attribute_name );
    
    switch ( _attr.get_data_type_id() )
    {
        case SP_FLOAT_32:
            find< float >( attribute_value );
            break;
        case SP_FLOAT_64:
            find< double >( attribute_value );
            break;
        case SP_UINT_8:
            find< uint8_t >( attribute_value );
            break;
        case SP_INT_8:
            find< int8_t >( attribute_value );
            break;
        case SP_UINT_16:
            find< uint16_t >( attribute_value );
            break;
        case SP_INT_16:
            find< int16_t >( attribute_value );
            break;
        case SP_UINT_32:
            find< uint32_t >( attribute_value );
            break;
        case SP_INT_32:
            find< int32_t >( attribute_value );
            break;
        case SP_UINT_64:
            find< uint64_t >( attribute_value );
            break;
        case SP_INT_64:
            find< int64_t >( attribute_value );
            break;
        case SP_UNKNOWN_DATA_TYPE:
            throw exception( "error: attribute is of custom or unknown data type.",
                SPROCESS_HERE );
            return;
        default:
            throw exception( "error while comparing attributes", SPROCESS_HERE );
            return;
    }
    
    
    
}



void
find_mode::
print_usage()
{
    std::cout << "\nstream_process find looks for a value of an attribute.\n\n";
    std::cout << "find value usage: ./sp --find <filename> <attribute_name> <attribute_value>" << std::endl;
    std::cout << "example:          ./sp --find bunny normal 0.12345 \n" << std::endl;
    std::cout << "find max usage:   ./sp --find <filename> <attribute_name> max" << std::endl;
    std::cout << "example:          ./sp --find bunny radius max\n" << std::endl;
    std::cout << "find min usage:   ./sp --find <filename> <attribute_name> min" << std::endl;
    std::cout << "example:          ./sp --find bunny radius min\n" << std::endl;
    std::cout << "find top usage:   ./sp --find <filename> <attribute_name> top<number>" << std::endl;
    std::cout << "example:          ./sp --find bunny radius top10\n" << std::endl;
    std::cout << "find avg usage:   ./sp --find <filename> <attribute_name> avg" << std::endl;
    std::cout << "example:          ./sp --find bunny radius avg\n" << std::endl;
}

} // namespace stream_process

