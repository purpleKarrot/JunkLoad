#include "rt_struct_user.h"

#include <sstream>

namespace stream_process
{
rt_struct_factory* rt_struct_user::_factory = 0;

const rt_struct_factory& 
rt_struct_user::get_factory() const
{
    return *_factory;
}



rt_struct_user::rt_struct_user()
{
    assert( _factory );
    _factory->register_user( this );
}


void 
rt_struct_user::set_factory( rt_struct_factory* factory )
{
    _factory = factory;
}



void 
rt_struct_user::_require( const rt_struct_member_base& member )
{
    _required_inputs.push_back( name_type_pair( member.get_name(), 
        SP_UNKNOWN_DATA_TYPE ) );
}



void 
rt_struct_user::_require( const rt_struct_member_base& member, 
    data_type_id type_ )
{
    _required_inputs.push_back( name_type_pair( member.get_name(), 
        type_ ) );
}




void 
rt_struct_user::setup_stage_0()
{}



void
rt_struct_user::setup_stage_1()
{}


  
void 
rt_struct_user::setup_stage_2()
{}



void 
rt_struct_user::set_name( const std::string& name )
{
    _name = name;
}




const std::string& 
rt_struct_user::get_name() const
{
    return _name;
}



const std::vector< rt_struct_user::name_type_pair >& 
rt_struct_user::get_required_inputs() const
{
    return _required_inputs;
}



const std::list< rt_struct_member_info* >& 
rt_struct_user::get_reserved_members() const
{
    return _members;
}


std::string 
rt_struct_user::print_member_status() const
{
    std::stringstream ss;
    ss << _name << ":\n";

    ss << "  requires:\n";
    std::vector< name_type_pair >::const_iterator req_it 
        = _required_inputs.begin();
    std::vector< name_type_pair >::const_iterator req_it_end 
        = _required_inputs.end();
    for( ; req_it != req_it_end; ++req_it )
    {
        ss << "    " << (*req_it).first << "\n";
    }

    ss << "  provides:\n";
    std::list< rt_struct_member_info* >::const_iterator pro_it 
        = _members.begin();
    std::list< rt_struct_member_info* >::const_iterator pro_it_end 
        = _members.end();
    for( ; pro_it != pro_it_end; ++pro_it )
    {
        ss << "    " << (*pro_it)->name << "\n";
    }
    ss << std::endl;
    return ss.str();
}



//FIXME map< string, info >
rt_struct_member_info&
rt_struct_user::get_attribute_info( const std::string& name )
{
    std::list< rt_struct_member_info* >::iterator it        = _members.begin();
    std::list< rt_struct_member_info* >::iterator it_end    = _members.end();
    for ( ; it != it_end; ++it )
    {
        if ( (*it)->name == name )
            return **it;
    }
    std::string msg( "attribute_info for attribute " );
    msg += name;
    msg += " not found.";
    throw exception( msg.c_str(), SPROCESS_HERE );
}



} // namespace stream_process

