#ifndef _Write_Op_H_
#define _Write_Op_H_

#include "StreamProcessing.h"
#include "stream_op.h"
#include "rt_struct_member.h"
#include "point_info.hpp"
#include "point_release_notification.hpp"

#include <deque>
#include <boost/iostreams/device/mapped_file.hpp>

namespace stream_process
{

class chain_operators;
class write_op : public stream_op 
{
public:
    write_op();

    virtual void pull_push();
    virtual stream_point* front();

    virtual void pop_front();
    virtual void clear_stage();

    virtual size_t size() const;
    virtual size_t position() const { return _index; };

    virtual void setup_stage_0();
    virtual void setup_stage_1();
    virtual void setup_stage_2();
	
	virtual void register_release_notification_target( 
		point_release_notification_target* target );
	
    virtual void prepare_shutdown();
    
protected:
    virtual void _update_and_write_output_header();
    virtual void _mmap_file();
	virtual void _on_release( stream_point* point );
    
    virtual void _print_filenames();

	void _write_out_and_remove_point( stream_point* point );
	
    std::deque< stream_point* > _fifo; 

    std::string _out_data_filename;
    std::string _out_header_filename;
       
    boost::iostreams::mapped_file _output_file;
    char* _output_ptr;
    char* _current_output_ptr;
    
    uint32_t    _index;
    size_t      _output_size_in_bytes;
    size_t      _number_of_points_written_to_output;
  
    rt_struct_member< uint32_t >    _point_index; 
    rt_struct_member< vec3f >       _position;
   
    chain_operators* _chain_operators;
	
	std::deque< point_release_notification_target* > _release_notification_targets;
};


// write op options 
struct write_op_options : public op_options
{
    write_op_options();
};


} // namespace stream_process

#endif
