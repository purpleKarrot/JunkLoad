#ifndef __STREAM_PROCESS__INSPECT__HPP__
#define __STREAM_PROCESS__INSPECT__HPP__

#include <stream_process/point_set.hpp>
#include <stream_process/options_base.hpp>

#include <stream_process/attribute_helper.hpp>

#include <iostream>

namespace stream_process
{

class list_mode
{
public:
    list_mode( const options_map& omap );

    void print_point( std::ostream& os, size_t index );

    void print_attribute( std::ostream& os, const std::string& name,
        size_t point_index ) const;

protected:
    void print_usage();
    
    const options_map&      _options;
    point_set               _point_set;

}; // class inspect

} // namespace stream_process

#endif

