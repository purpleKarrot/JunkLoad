#ifndef __STREAM_PROCESS__SORT_KEY_TRANSFORM__HPP__
#define __STREAM_PROCESS__SORT_KEY_TRANSFORM__HPP__

#include <sys/types.h>

#include <string>

namespace stream_process
{
class mapped_point_data;
class sort_range;

class sort_key_transform
{
public:
    sort_key_transform( mapped_point_data& mapped_point_data_, size_t offset_ );
    sort_key_transform( const sort_key_transform& source_ );

    void transform();
    void transform( const sort_range& range_ );
    void transform_back();
    void transform_back( const sort_range& range_ );
    
protected:
    std::string _debug_print_mask( const uint32_t& bitmask ) const;

    mapped_point_data&      _mapped_point_data;
    size_t                  _offset;

    bool                    _transform_to; // transform_to 
    
}; // class sort_key_transform

} // namespace stream_process

#endif

