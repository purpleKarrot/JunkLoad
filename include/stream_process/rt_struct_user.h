#ifndef __VMML_RT_STRUCT_USER_H__
#define __VMML_RT_STRUCT_USER_H__


#include "rt_struct_member.h"
#include "rt_struct_member_info2.hpp"
#include "rt_struct_factory.h"
#include "rt_struct_member_io_status.hpp"

#include "data_types.hpp"

namespace stream_process
{

class rt_struct_user
{
public:
    typedef std::list< rt_struct_member_info* >     member_infos;
    typedef std::pair< std::string, data_type_id >  name_type_pair;

    rt_struct_user();
    virtual ~rt_struct_user() {};

    const rt_struct_factory& get_factory() const;
    static void set_factory( rt_struct_factory* factory );
    
    void set_name( const std::string& name );
    const std::string& get_name() const;
    
    virtual void setup_stage_0();
    virtual void setup_stage_1();     
    virtual void setup_stage_2();
    
    const std::vector< name_type_pair >& get_required_inputs() const;
    const std::list< rt_struct_member_info* >& get_reserved_members() const;

    std::string print_member_status() const;
    
    rt_struct_member_info& get_attribute_info( const std::string& name );

protected:
    friend class rt_struct_factory;
 
    void _require( const rt_struct_member_base& member );
    void _require( const rt_struct_member_base& member, data_type_id type_ );
    
    template< typename T >
    void _reserve( const rt_struct_member< T >& member_, 
        const IO_STATUS io_status = IO_WRITE_TO_STREAM );

    template< typename T >
    void _reserve_array( const rt_struct_member< T >& member_, 
        data_type_id dt_id, size_t array_size, 
        const IO_STATUS io_status = IO_WRITE_TO_STREAM );
    
    template< typename T, template< typename T > class U >
    void _reserve_vmmlib_type( const rt_struct_member< U< T > >& member_, 
     const IO_STATUS io_status );

    template< typename T >
    void _reserve_custom_type( const rt_struct_member< T >& member_, 
        data_type_id dt_id, size_t element_size_in_bytes, size_t array_size, 
        const IO_STATUS io_status = IO_WRITE_TO_STREAM );

    std::string _name;

    // FIXME map< string, info >
    std::list< rt_struct_member_info* > _members;  
    std::vector< name_type_pair >       _required_inputs;
       
    static rt_struct_factory* _factory;

};


template< typename T >
void 
rt_struct_user::_reserve( const rt_struct_member< T >& member_, 
    const IO_STATUS io_status )
{
    get_data_type_id_from_type< T > get_type;

    rt_struct_member_info* member_info = 
        new rt_struct_member_info( 
            member_.get_name(), 
            get_type(), 
            1 
            );

    if ( io_status & IO_WRITE_TO_OUTPUT )
        member_info->is_output = true;

    _members.push_back( member_info );
    
    
}



template< typename T >
void 
rt_struct_user::_reserve_array( const rt_struct_member< T >& member_, 
    data_type_id dt_id, size_t array_size, const IO_STATUS io_status )
{
    rt_struct_member_info* member_info = 
        new rt_struct_member_info( 
            member_.get_name(), 
            dt_id, 
            array_size 
            );
    if ( io_status & IO_WRITE_TO_OUTPUT )
        member_info->is_output = true;

    _members.push_back( member_info );
    
}


template< typename T, template< typename T > class U >
void
rt_struct_user::_reserve_vmmlib_type( const rt_struct_member< U< T > >& member_, 
     const IO_STATUS io_status )
{
    get_data_type_id_from_type< T > get_type;

    size_t array_size = sizeof( U< T > ) / sizeof( T );

    rt_struct_member_info* member_info = 
        new rt_struct_member_info( 
            member_.get_name(), 
            get_type(),
            array_size 
            );
    if ( io_status & IO_WRITE_TO_OUTPUT )
        member_info->is_output = true;

    _members.push_back( member_info );

}



template< typename T >
void
rt_struct_user::_reserve_custom_type( const rt_struct_member< T >& member_, 
    data_type_id dt_id, size_t element_size_in_bytes, size_t array_size, 
    const IO_STATUS io_status )
{
    rt_struct_member_info* member_info = 
        new rt_struct_member_info( 
            member_.get_name(), 
            dt_id,
            element_size_in_bytes, 
            array_size 
            );
    if ( io_status & IO_WRITE_TO_OUTPUT )
        member_info->is_output = true;

    _members.push_back( member_info );

}





} // namespace stream_process

#endif
