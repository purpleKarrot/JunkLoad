#ifndef __STREAM_PROCESS__FIND_ATTRIBUTE__HPP__
#define __STREAM_PROCESS__FIND_ATTRIBUTE__HPP__

#include <stream_process/point_set.hpp>
#include <stream_process/options_base.hpp>
#include <stream_process/attribute_helper.hpp>

namespace stream_process
{

class find_mode
{
public:
    find_mode( const options_map& options_ );

    template< typename T >
    void find( const std::string& value );
    
    template< typename T >
    void find_min_max();

    template< typename T >
    void find_avg();

    template< typename T >
    void find_top( const std::string& top_number_ );

protected:

    void print_usage();

    const options_map&      _options;
    point_set               _point_set;
    attribute               _attr;
    
    std::string             _filename;

}; // class find_attribute



template< typename T >
void
find_mode::
find( const std::string& value_string )
{
    if ( value_string == "min" || value_string == "max" )
    {
        find_min_max< T >();
        return;
    }
    else if ( value_string == "avg" )
    {
        find_avg< T >();
        return;
    }
    else if ( value_string.substr( 0, 3 ) == "top" )
    {
        find_top< T >( value_string );
        return;
    }
    
    const T& value = boost::lexical_cast< T >( value_string );
    
    const point_set_header& h           = _point_set.get_header();

    const size_t number_of_points       = h.get_number_of_vertices();
        
    const size_t array_size             = _attr.get_number_of_elements();
    const size_t offset                 = _attr.get_offset();
    
    attribute_helper helper;
    
    std::vector< size_t > found_in( array_size, 0 );
    
    size_t found = 0;
    for( size_t index = 0; index < number_of_points; ++index )
    {
        const stream_data* point = _point_set[ index ];
        for( size_t a = 0; a < array_size; ++a )
        {
            const T& t = helper.get_attribute< T >( point, offset, index, a );

            if ( t == value )
            {
                ++found;
                ++found_in[ a ];
                std::cout << index << ": " << _attr.get_name();
                if ( array_size != 1 )
                {
                    std::cout << "[" << a << "]";
                }
                std::cout << " = " << t << std::endl;
            
            }
        
        }
    
    }
    
    if ( found == 0 )
    {
        std::cout << "\nattribute '" << _attr.get_name() << "' with value " << value 
            << " could not be found." << std::endl;
    }
    else
    {
        std::cout << "\nfound " << found << " occurences of '" << _attr.get_name()
            << "' with value " << value << "." << std::endl;
        if ( array_size != 1 )
        {
            for( size_t index = 0; index < array_size; ++index )
            {
                std::cout << "found " << found_in[ index ]
                    << " occurences in " << _attr.get_name() << "[" << index << "]\n";
            }
        }
        std::cout << std::endl;
    }
}



template< typename T >
void
find_mode::
find_min_max()
{
    const point_set_header& h           = _point_set.get_header();

    const size_t number_of_points       = h.get_number_of_vertices();
        
    const size_t array_size             = _attr.get_number_of_elements();
    const size_t offset                 = _attr.get_offset();
    
    attribute_helper helper;
    
    T min_, max_;
    {
        const stream_data* point = _point_set[ 0 ];
        const T& t = helper.get_attribute< T >( point, offset, 0, 0 );
        min_ = t;
        max_ = t;
    }

    size_t found = 0;
    for( size_t index = 0; index < number_of_points; ++index )
    {
        const stream_data* point = _point_set[ index ];
        for( size_t a = 0; a < array_size; ++a )
        {
            const T& t = helper.get_attribute< T >( point, offset, index, a );

            if ( t > max_ )
                max_ = t;
            if ( t < min_ )
                min_ = t;
        }
    
    }

    std::cout
        << "\nstream data set '" << _filename
        << "' contains the following extrema for attribute '"
        << _attr.get_name() << "'\n"
        << "\nminimum " << min_ << "\nmaximum " << max_ << "\n"
        << std::endl;
    
}


template< typename T >
void
find_mode::
find_avg()
{
    const point_set_header& h           = _point_set.get_header();

    const size_t number_of_points       = h.get_number_of_vertices();
        
    const size_t array_size             = _attr.get_number_of_elements();
    const size_t offset                 = _attr.get_offset();
    
    attribute_helper helper;
    
    double sum = 0;

    size_t found = 0;
    for( size_t index = 0; index < number_of_points; ++index )
    {
        const stream_data* point = _point_set[ index ];
        for( size_t a = 0; a < array_size; ++a )
        {
            const T& t = helper.get_attribute< T >( point, offset, index, a );
            sum += t;
        }
    
    }

    std::cout
        << "\nstream data set '" << _filename
        << "' has an arithmetic mean of " << sum / (double) number_of_points
        << " for attribute '"
        << _attr.get_name() << "'\n"
        << std::endl;
    
}



template< typename T >
void
find_mode::
find_top( const std::string& top_number_ )
{
    std::string num  = top_number_.substr( 3, std::string::npos );
    size_t top_num   = boost::lexical_cast< size_t >( num );

    const point_set_header& h           = _point_set.get_header();

    const size_t number_of_points       = h.get_number_of_vertices();
        
    const size_t array_size             = _attr.get_number_of_elements();
    const size_t offset                 = _attr.get_offset();
    
    attribute_helper helper;
    
    std::priority_queue< T, std::vector< T >, std::less< T > >      min_;
    std::priority_queue< T, std::vector< T >, std::greater< T > >   max_;

    for( size_t index = 0; index < number_of_points; ++index )
    {
        const stream_data* point = _point_set[ index ];
        for( size_t a = 0; a < array_size; ++a )
        {
            const T& t = helper.get_attribute< T >( point, offset, index, a );
            
            min_.push( t );
            max_.push( t );
            if ( min_.size() >= top_num )
            {
                min_.pop();
                max_.pop();
            }

        }
    
    }
    
    std::deque< T >    out_;
    
    std::cout
        << "\nstream data set '" << _filename
        << "' has the following " << top_number_ << "s for attribute '"
        << _attr.get_name() << "'\n\nminimum " << top_number_ << "\n\n";
    while( ! min_.empty() )
    {
        out_.push_front( min_.top() );
        min_.pop();
    }
    
    typename std::deque< T >::const_iterator out_it = out_.begin();
    for( ; out_it != out_.end(); ++out_it )
    {
        std::cout << *out_it << "\n";
    }
    out_.clear();
    
    
    std::cout << "\nmaximum " << top_number_ << "\n\n";

    while( ! max_.empty() )
    {
        out_.push_front( max_.top() );
        max_.pop();
    }

    for( out_it = out_.begin(); out_it != out_.end(); ++out_it )
    {
        std::cout << *out_it << "\n";
    }

    out_.clear();

    std::cout << std::endl;
    
}


} // namespace stream_process

#endif

