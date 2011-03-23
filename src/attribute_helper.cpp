#include <stream_process/attribute_helper.hpp>

#include <boost/lexical_cast.hpp>

namespace stream_process
{

std::string
attribute_helper::get_data_as_string( const stream_data* point, 
    const attribute& attr, const std::string& seperator_ )
{
    std::string result;

    switch(  attr.get_data_type_id() )
    {
        case SP_FLOAT_32:
            result += get_attribute_string< float >( point, attr, seperator_ );
            break;
        case SP_FLOAT_64:
            result += get_attribute_string< double >( point, attr, seperator_  );
            break;
        case SP_UINT_8:
            result += get_attribute_string< uint8_t >( point, attr, seperator_  );
            break;
        case SP_INT_8:
            result += get_attribute_string< int8_t >( point, attr, seperator_  );
            break;
        case SP_UINT_16:
            result += get_attribute_string< uint16_t >( point, attr, seperator_  );
            break;
        case SP_INT_16:
            result += get_attribute_string< int16_t >( point, attr, seperator_  );
            break;
        case SP_UINT_32:
            result += get_attribute_string< uint32_t >( point, attr, seperator_  );
            break;
        case SP_INT_32:
            result += get_attribute_string< int32_t >( point, attr, seperator_  );
            break;
        case SP_UINT_64:
            result += get_attribute_string< uint64_t >( point, attr, seperator_  );
            break;
        case SP_INT_64:
            result += get_attribute_string< int64_t >( point, attr, seperator_  );
            break;
        case SP_UNKNOWN_DATA_TYPE:
            result += "<unknown or custom data type>";
            break;
        default:
            throw exception( "error while trying to access attributes.",
                SPROCESS_HERE );
            break;
    }
    return result;
}


std::string
attribute_helper::get_as_string( const point_set& ps, size_t point_index, 
    const std::string& attribute_name )
{
    const stream_data_structure& structure
        = ps.get_header().get_vertex_structure();
    
    const attribute& attr       = structure.get_attribute( attribute_name );
    const stream_data* point    = ps[ point_index ];

    std::string result = attribute_name;
    while( result.size() < 20 )
        result += " ";

    switch(  attr.get_data_type_id() )
    {
        case SP_FLOAT_32:
            result += get_attribute_string< float >( point, attr );
            break;
        case SP_FLOAT_64:
            result += get_attribute_string< double >( point, attr );
            break;
        case SP_UINT_8:
            result += get_attribute_string< uint8_t >( point, attr );
            break;
        case SP_INT_8:
            result += get_attribute_string< int8_t >( point, attr );
            break;
        case SP_UINT_16:
            result += get_attribute_string< uint16_t >( point, attr );
            break;
        case SP_INT_16:
            result += get_attribute_string< int16_t >( point, attr );
            break;
        case SP_UINT_32:
            result += get_attribute_string< uint32_t >( point, attr );
            break;
        case SP_INT_32:
            result += get_attribute_string< int32_t >( point, attr );
            break;
        case SP_UINT_64:
            result += get_attribute_string< uint64_t >( point, attr );
            break;
        case SP_INT_64:
            result += get_attribute_string< int64_t >( point, attr );
            break;
        case SP_UNKNOWN_DATA_TYPE:
            result += "<unknown or custom data type>";
            break;
        default:
            throw exception( "error while trying to access attributes.",
                SPROCESS_HERE );
            break;
    }
    return result;
}




} // namespace stream_process

