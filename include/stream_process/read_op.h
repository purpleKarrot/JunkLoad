#ifndef _Read_Op_H_
#define _Read_Op_H_

#include "stream_op.h"
#include "rt_struct_member.h"

#include "point_info.hpp"
#include "point_header.hpp"
#include "rt_struct_member_info2.hpp"
#include "binary_reader.hpp"
#include "mapped_point_data.hpp"
#include "point_insertion_notification.hpp"

#include <stream_process/VMMLibIncludes.h>

#include <boost/program_options.hpp>

#include <fcntl.h>
#include <sys/mman.h>


namespace stream_process
{

class read_op_barrier;
class chain_operators;

class read_op 
    : public stream_op 
{

public:
    read_op();
    virtual ~read_op();
    
    virtual void pull_push();

    virtual stream_point* front();
    virtual void pop_front();

    virtual size_t size() const;
    virtual size_t position() const;

    virtual void setup_stage_0();
    virtual void setup_stage_1();
    virtual void setup_stage_2();
    
    virtual void set_mapped_point_data( mapped_point_data* mapped_point_data_ );
    virtual mapped_point_data& get_mapped_point_data();
    virtual const mapped_point_data& get_mapped_point_data() const;
    
    virtual void register_insertion_notification_target( 
        point_insertion_notification_target* target );
   
   virtual void prepare_shutdown();
   
   virtual bool has_read_all_points_from_input();

protected:
    virtual stream_point*   _read_next_point();

    virtual void _read_header();    
    virtual void _mmap_file();
    virtual void _register_point_attributes();
    
    virtual void _on_insertion( stream_point* point );

    void _setup_readers();
    
    virtual void _print_filenames();

    stream_point*       _point;
    
    std::string         _header_filename;
    std::string         _source_filename;
 
    mapped_point_data*  _mapped_point_data;

    size_t		_in_bytes;
    char*       _input_ptr;
    char*       _current_position;
    char*       _tmp_input;
    char*       _tmp_input_position;
    char*       _tmp_point_position;
    
    bool        _preprocessing_is_enabled;
    
    uint32_t	_index;
    size_t		_input_point_count;

    size_t      _number_of_points_read_from_input;
    size_t      _number_of_points_written_to_stream;

    rt_struct_member< uint32_t >            _point_index;   
    rt_struct_member< uint32_t >            _min_ref_index;   
    rt_struct_member< uint32_t >            _max_ref_index;   

    rt_struct_member< vec3f >               _position;
    
    std::list< rt_struct_member_info* >     _input_members;
    
    std::vector< rt_struct_member_info >    _inputs;
    std::vector< binary_reader* >           _input_readers;
    
    chain_operators*                        _chain_operators;
    
    size_t      _min_unsorted_index;
    
    read_op_barrier*                        _read_op_barrier;
    
    std::deque< point_insertion_notification_target* > 
        _insertion_notification_targets;
        
    // ident test
    vec3f                                   _last_position;
    const float                             _precision_limit;
    
};


// read op options 
struct read_op_options : public op_options
{
    read_op_options();
};




}; // namespace vmml

#endif
