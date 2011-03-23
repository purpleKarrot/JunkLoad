#include "rt_struct_factory.h"

#include "stream_point.h"

#include "rt_struct_user.h"
#include "rt_struct_member_base.h"

#include "Log.h"
#include "exception.hpp"

namespace stream_process
{


rt_struct_factory::rt_struct_factory( rt_struct_info& info ) 
    : _rt_struct_info( info )
    , _input_data_size_in_bytes( 0 )
    , _stream_data_size_in_bytes( 0 )
    , _output_data_size_in_bytes( 0 )
    , _freeze( false )
    , _pool( 0 )
    //, _factoryPtr()
{
    LOGDEBUG << "instance of dynamicdata factory initialized. " << std::endl;
}



rt_struct_factory::~rt_struct_factory()
{
    if ( _pool )
    {
        _pool->purge_memory();
        delete _pool;
    }
}



void
rt_struct_factory::register_user( rt_struct_user* user )
{
    if ( ! user )
    {
        STREAM_EXCEPT( "factory failed to register rt_struct_user -> nullpointer" );
    } 
    _users.push_back( user );
}


void 
rt_struct_factory::register_member( const std::string& name, 
    rt_struct_member_base* member )
{
    std::map< std::string, std::list< rt_struct_member_base* > >::iterator it
        = _members_by_name.find( name );
    if ( it != _members_by_name.end() )
    {
        (*it).second.push_back( member );
    }
    else
    {
        _members_by_name[ name ].push_back( member );
    }
}




void 
rt_struct_factory::setup_stage_0()
{}



void 
rt_struct_factory::setup_stage_1()
{}



void 
rt_struct_factory::setup_stage_2()
{
    prepare_stage2();
}
   


void 
rt_struct_factory::prepare_stage2()
{
    _reserve_user_members();
    _construct_rt_struct();
    _initialize_pool();
}



void 
rt_struct_factory::_reserve_user_members()
{
    std::list< rt_struct_user* >::iterator it = _users.begin();
    std::list< rt_struct_user* >::iterator itend = _users.end();
    for( ; it != itend; ++it )
    {
        _reserve_all_of_user( *it );
    }
}



bool 
rt_struct_factory::is_valid_ptr( rt_struct* data )
{
    return _pool->is_from( data );
}



void 
rt_struct_factory::_reserve_all_of_user( rt_struct_user* user )
{
    if ( _freeze )
    {
        STREAM_EXCEPT( "element reservation is frozen."  );
    }
    
    std::list< rt_struct_member_info* >& member_infos  = user->_members;
    std::list< rt_struct_member_info* >::iterator it   = member_infos.begin();
    std::list< rt_struct_member_info* >::iterator it_end = member_infos.end();
    
    for( ; it != it_end; ++it )
    {
        _rt_struct_info.add_attribute( **it );
    }
}



const size_t 
rt_struct_factory::get_member_offset( const std::string& name ) const
{
    rt_struct_info::const_iterator it = _rt_struct_info.find( name );
    if ( it != _rt_struct_info.end() )
        return (*it).stream_offset;

    std::string error_msg( "member " );
    error_msg += name;
    error_msg += " could not be found.";
    throw exception( error_msg, SPROCESS_HERE );
}



bool
rt_struct_factory::has_member( const std::string& name ) const
{
    return _rt_struct_info.has_attribute( name );
}



void 
rt_struct_factory::_construct_rt_struct()
{
    // we need to sort the info structure first, so that all the 
    // inputs and outputs are at the beginning.
    _rt_struct_info.sort();
    
    _stream_data_size_in_bytes = 0;

    size_t offset = 0;
    size_t size_in_bytes = 0;

    // set the stream offsets and compute input and ouput point sizes
    rt_struct_info::iterator it     = _rt_struct_info.begin();
    rt_struct_info::iterator it_end = _rt_struct_info.end();
    for( ; it != it_end; ++it )
    {
        rt_struct_member_info& info = *it;

        info.stream_offset  = offset;
         
        size_in_bytes       = info.element_size_in_bytes;
        size_in_bytes      *= info.array_size;

        _stream_data_size_in_bytes += size_in_bytes;
                
        if ( info.is_input )
        {
            _input_data_size_in_bytes += size_in_bytes;
        }
        // TODO configable output format
        if ( info.is_output )
        {
            _output_data_size_in_bytes += size_in_bytes;
            info.output_offset = offset;
        }

        #if 0
        std::cout << "reserving " << size_in_bytes << " bytes for " 
            << info.name << ": " //<< info.input_offset << " " 
            << info.stream_offset //<< " " << info.output_offset 
            << std::endl;
        #endif
        
        offset += size_in_bytes;
    }

    #if 0
    std::cout << "stream size in bytes: " << _stream_data_size_in_bytes 
        << std::endl;
    #endif
    
    _freeze = true; // we freeze element reservation
    
    std::cout << "\npoint structure:\n" << _rt_struct_info << "\n";
    std::cout 
        << "point size in input file:  " 
        << _input_data_size_in_bytes << " bytes.\n"
        << "point size in memory:      " 
            << _stream_data_size_in_bytes << " bytes.\n"
        << "point size in output file: " 
            << _output_data_size_in_bytes << " bytes.\n"
        << std::endl;

    _set_member_offsets();
}



void 
rt_struct_factory::_initialize_pool()
{
    _pool = new boost::pool<>( _stream_data_size_in_bytes );
}



void 
rt_struct_factory::_set_member_offsets()
{
    std::map< std::string, std::list< rt_struct_member_base* > >
        ::iterator m_it = _members_by_name.begin();
    std::map< std::string, std::list< rt_struct_member_base* > >
        ::iterator m_it_end = _members_by_name.end();

    std::list< rt_struct_member_base* >::iterator list_it; 
    std::list< rt_struct_member_base* >::iterator list_it_end; 

    rt_struct_info::const_iterator it;
    rt_struct_info::const_iterator it_end = _rt_struct_info.end();
    
    size_t offset;
    for( ; m_it != m_it_end; ++m_it )
    {
        it = _rt_struct_info.find( (*m_it).first );
        if ( it != it_end )
        {
            offset      = (*it).stream_offset;
            list_it     = (*m_it).second.begin();
            list_it_end = (*m_it).second.end();
            for( ; list_it != list_it_end; ++list_it )
            {
                (*list_it)->set_offset( offset );
            }
        } 
        else 
        {
            throw exception( std::string( "Requested offset of unknown data " 
				+ (*m_it).first ), SPROCESS_HERE );
        }

    }
}



rt_struct_info&
rt_struct_factory::get_rt_struct_info()
{
    return _rt_struct_info;
}



const rt_struct_info&
rt_struct_factory::get_rt_struct_info() const
{
    return _rt_struct_info;
}



size_t
rt_struct_factory::get_input_size_in_bytes() const
{
    return _input_data_size_in_bytes;
}



size_t
rt_struct_factory::get_stream_size_in_bytes() const
{
    return _stream_data_size_in_bytes;
}



size_t
rt_struct_factory::get_output_size_in_bytes() const
{
    return _output_data_size_in_bytes;
}

    


} // namespace stream_process

