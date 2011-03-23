#include "rt_struct_member_base.h"
#include "rt_struct_factory.h"

namespace stream_process
{

rt_struct_factory* rt_struct_member_base::factory = 0;

void 
rt_struct_member_base::set_factory( rt_struct_factory* factory_ )
{
    factory = factory_;
}

rt_struct_member_base::rt_struct_member_base( const std::string& name )
    : _name( name )
    , _offset( 0 - 1 )
{}


rt_struct_member_base::rt_struct_member_base()
    : _offset( 0 - 1 )
{}


void 
rt_struct_member_base::register_member( const std::string& name )
{
    assert( factory != 0 );
    factory->register_member( name, this );
}


void
rt_struct_member_base::set_name( const std::string& name_ )
{
    _name = name_;
}


void 
rt_struct_member_base::set_offset( size_t offset_ )
{
    _offset = offset_;    
    //std::cout << "set offset for " << _name << " to " << _offset << std::endl;
}


const std::string&  
rt_struct_member_base::get_name() const
{
    return _name;
}



const size_t        
rt_struct_member_base::get_offset() const
{
    return _offset;
}



} // namespace stream_process
