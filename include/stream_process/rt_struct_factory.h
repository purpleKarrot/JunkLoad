#ifndef _rt_struct_factory_H_
#define _rt_struct_factory_H_

#include "rt_struct.h"
#include "rt_struct_info.hpp"

#include "StreamProcessing.h"
#include "exception.hpp"

#include <boost/pool/pool.hpp>

/**
 * @brief dynamic data object factory
 *
 * the factory is required in order to use dynamic data objects.
 * each factory can only build one type of dynamic data objects, so if you
 * need multiple different ones, just create a factory for each. 
 *
 * @author Jonas Boesch
 *
 */

namespace stream_process
{

class rt_struct_user;
class rt_struct_member_base;
class rt_struct_member_info;

class rt_struct_factory
{
public:
    rt_struct_factory( rt_struct_info& info );
    virtual ~rt_struct_factory();
    
    void register_user( rt_struct_user* user );
    void register_member( const std::string& name, 
        rt_struct_member_base* member );
    
    void prepare_stage2();
    
    // 'allocators' / 'deallocators' for dynamic objects
    // ( uses a pool for efficiency reasons )
    inline rt_struct* create();    
    inline void destroy( rt_struct* data );

    // query dynamic class for 'member variables'
    bool has_member( const std::string& name ) const;
    const size_t get_member_offset( const std::string& name ) const;

    size_t get_input_size_in_bytes() const;
    size_t get_stream_size_in_bytes() const;
    size_t get_output_size_in_bytes() const;
    
    // debug: check if data is a valid ptr to an instance of the dynamic class
    bool is_valid_ptr( rt_struct* data );
    
    virtual void setup_stage_0();
    virtual void setup_stage_1();     
    virtual void setup_stage_2();    
    
    const std::map< std::string, rt_struct_member_info* >& 
        get_member_map() const;

    rt_struct_info&         get_rt_struct_info();
    const rt_struct_info&   get_rt_struct_info() const;
        
protected:
    void _reserve_all_of_user( rt_struct_user* user );

    void _reserve_user_members();
    void _construct_rt_struct();
    void _initialize_pool();

    // sets the offsets in all rt_struct_members (accessors) that registered
    // itselves in the rt_struct_users _members list.
    void _set_member_offsets();
    
    rt_struct_info&     _rt_struct_info;
    
    size_t  _input_data_size_in_bytes;
    size_t  _stream_data_size_in_bytes;
    size_t  _output_data_size_in_bytes;

    // members can only be registered in the setup phase.
    // freeze is used to check that.
    bool    _freeze;

    std::list< rt_struct_user* > _users;
    std::map< std::string, std::list< rt_struct_member_base* > > _members_by_name;

    boost::pool<>*    _pool;
};



inline rt_struct* 
rt_struct_factory::create() 
{
    return static_cast< rt_struct*> ( _pool->malloc() );
    //return _pool->create();
}



inline void 
rt_struct_factory::destroy( rt_struct* data ) 
{
    assert( data );
    _pool->free( data );
}


} // namespace stream_process

#endif
