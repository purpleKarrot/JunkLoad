#ifndef __STREAM_PROCESS__CHAIN_MANAGER__HPP__
#define __STREAM_PROCESS__CHAIN_MANAGER__HPP__

#include "point_info.hpp"
#include "rt_struct_info.hpp"
#include "rt_struct_factory.h"

#include "chain_config.h"
#include "stream_operators.hpp"
#include "chain_operators.hpp"

#include "active_set.h"
#include "stream_processing_options.hpp"
#include "multi_timer.hpp"

#include "mapped_point_data.hpp"
#include "preprocess_shared_data.hpp"

#include "point_insertion_notification.hpp"
#include "point_release_notification.hpp"

#include <string>
#include <map>

#include <boost/program_options.hpp>

namespace stream_process
{

class stream_op;
class read_op;
class stream_op_factory;
class chain_op_factory;

class chain_manager
{
public: 
    //chain_manager( int argc, const char** argv );
    chain_manager( stream_processing_options& options_, 
        rt_struct_factory* factory = 0 );
    ~chain_manager();

    void setup();

    void process(); 
    bool process( size_t points );
    void process_sorted_points();
    
    active_set_op*  create_chain_op( const std::string& op_name );    

    void register_stream_op_factory(  const std::string& name, 
        stream_op_factory* stream_op_factory );
    void register_chain_op_factory(  const std::string& name, 
        chain_op_factory* stream_op_factory );
           
    inline void set_largest_element_index( size_t index );

    stream_operators& get_stream_operators();
    const stream_operators& get_stream_operators() const;

    chain_operators& get_chain_operators();
    const chain_operators& get_chain_operators() const;

    preprocess_shared_data& get_preprocess_shared_data();
    const preprocess_shared_data& get_preprocess_shared_data() const;
    void set_preprocess_shared_data( preprocess_shared_data* pp_data );
    
    // WARNING: might return a nullpointer.
    mapped_point_data* get_input_mapped_point_data();
    void set_input_mapped_point_data( mapped_point_data* mapped_point_data_ );

    void setup_options();
    
    void notify_on_insertion( point_insertion_notification_target* target );
    void notify_on_release( point_release_notification_target* target );

    // for boost::thread
    inline void operator()();

protected:
    void _setup_stage_0();
    void _setup_stage_1();
    void _setup_stage_2();
              
    //void _prepare_processing();
    void _prepare_shutdown();
    
    void _setup(); 
    void _setup_logging();
    void _setup_static_members();

    stream_op*      _create_stream_op( const std::string& op_name );    

    stream_processing_options&  _options;
    rt_struct_factory*          _point_factory;

    point_info*                 _point_info;
    active_set                  _active_set;
    stream_operators            _stream_operators;
    chain_operators             _chain_operators;

    chain_config                _config;

    std::vector< std::string >  _stream_op_list;

    std::map< std::string, stream_op_factory* > _stream_op_factories;
    std::map< std::string, chain_op_factory* >  _chain_op_factories;

    size_t _largest_index_in_active_set;
    size_t _largest_index_in_data_set;
    
    // the mmapped point data and pp_data are only set when doing the 
    // preprocessing and stream processing in parallel - otherwise, 
    // read_op will create it's own mapped_point_data instance. 
    mapped_point_data*          _input_mapped_point_data;
    preprocess_shared_data*     _preprocess_shared_data;
    
private:
    // to prevent using the copy-construction ctor
    chain_manager( const chain_manager& chain_manager_ )
        : _options( *( new stream_processing_options() ) ) {};

}; // class chain_manager



inline void 
//FIXME
chain_manager::set_largest_element_index( size_t index )
{
    _largest_index_in_active_set = index;
}



void
chain_manager::operator()()
{
    process_sorted_points();
}


} // namespace stream_process

#endif
