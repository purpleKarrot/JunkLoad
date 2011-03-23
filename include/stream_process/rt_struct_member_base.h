#ifndef __VMML__RT_STRUCT_MEMBER_BASE__H__
#define __VMML__RT_STRUCT_MEMBER_BASE__H__

#include <string>

namespace stream_process
{

class rt_struct_factory;

class rt_struct_member_base
{
public:
    rt_struct_member_base();
    rt_struct_member_base( const std::string& name );
    
    void set_offset( size_t offset_ );   
    void set_name( const std::string& name_ );
    
    void register_member( const std::string& name );

    static rt_struct_factory* factory;
    static void set_factory( rt_struct_factory* factory_ );
    
    const std::string&   get_name() const;
    const size_t         get_offset() const;

protected:
    friend class rt_struct;
    std::string _name;
    size_t _offset;

};


} // namespace stream_process

#endif

