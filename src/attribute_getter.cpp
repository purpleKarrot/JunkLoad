#include <stream_process/attribute_getter.hpp>

namespace stream_process
{

attribute_getter::attribute_getter()
    : _attribute( 0 )
{}


attribute_getter::attribute_getter( const attribute& attribute_ )
    : _attribute( &attribute_ )
{}


void
attribute_getter::set_attribute( const attribute& attribute_ )
{
    _attribute = &attribute_;
}



} // namespace stream_process

