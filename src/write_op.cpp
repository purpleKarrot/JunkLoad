/*
 *  write_operator.cpp
 *  StreamProcessing
 *
 *  Created by Renato Pajarola on Wed Jul 14 2004.
 *  Copyright (c) 2004 UC Irvine. All rights reserved.
 *
 */

#include "write_op.h"
#include "active_set.h"
#include "exception.hpp"

#include "point_header.hpp"
#include "chain_manager.hpp"
#include "chain_operators.hpp"

#include "Utils.h"

namespace stream_process
{

float avg_lifetime;

write_op::write_op()
    : stream_op()
	, _fifo()
	, _out_data_filename()
	, _out_header_filename()
	, _output_file()
	, _output_ptr( 0 )
	, _current_output_ptr( 0 )
	, _index( 0 )
	, _output_size_in_bytes( 0 )
    , _point_index( "point_index" )
    , _position( "position" )
    , _chain_operators( & _chain_manager->get_chain_operators() )
    , _number_of_points_written_to_output( 0 )
{
    set_name( "write" );
}



void 
write_op::pull_push()
{
    //size_t beginsize = _fifo.size();
    //size_t written = 0;
    // pull elements from previous operator
    stream_point* point;
    
    while ( ( point = prev->front() ) ) 
    {
        prev->pop_front();

        // enter waiting queue
        _fifo.push_back( point );
    }
    
    while ( ! _fifo.empty() ) 
    {
        point = _fifo.front();
        _index = point->get( _point_index );

        // only write to stream if not used by previous ones anymore
        if ( _index < stream_op::smallest_reference() ) 
        {
            /*
            std::cout 
                << "smallest " << stream_op::smallest_reference()
                << " this " << point->get( _point_index ) <<
                std::endl;
            */
			_write_out_and_remove_point( point );
			_fifo.pop_front();
		} 
        else
            break;
    }
}




void 
write_op::clear_stage()
{
    stream_point* point;
    while ( ! _fifo.empty() ) 
    {
        point = _fifo.front();
        _fifo.pop_front();
		_write_out_and_remove_point( point );
	} 

    // prepare shut down 
    _output_file.close();
    _update_and_write_output_header();
}



void
write_op::_write_out_and_remove_point( stream_point* point )
{

	_multi_timer.stop();
	_chain_operators->remove( point );
	_multi_timer.start();

	memcpy( _current_output_ptr, point, _output_size_in_bytes );
	_current_output_ptr     += _output_size_in_bytes;

	// sending out notifications to allow operators to do op-specific clean-up
	_on_release( point );
	
    LOGDEBUG << "removing point " << point->get( _point_index ) << "." << std::endl;

	_factory->destroy( point );

    ++_number_of_points_written_to_output;
    
}



stream_point* 
write_op::front()
{
   std::cout << "write_op::front - function was called, " 
         << " but 'write' is the logical end in the op chain." <<std::endl;
    return 0;
}



void 
write_op::pop_front()
{
   std::cout << "write_op::pop_front - function was called, " 
         << " but 'write' is the logical end in the op chain." <<std::endl;
}



size_t 
write_op::size() const
{
  return _fifo.size(); // + index;
}



void 
write_op::_update_and_write_output_header()
{ 
    _point_info->set_point_count( _number_of_points_written_to_output );

    point_header header( *_point_info );
    header.write( _out_header_filename );
}



void 
write_op::setup_stage_0()
{
    std::string header;
    std::string data;
    
    // get filenames from config
    var_map::iterator it = _config->find( "out-file" );
    if ( it != _config->end() )
        data = (*it).second.as< std::string >();
    else
    {
        // if no out-file is specified, use infile base name + '.out.points'
        it = _config->find( "in-file" );
        data =  (*it).second.as< std::string >();
        data = data.substr( 0, data.find_last_of( "." ) );
        data += ".out.points";
    }
    
    it = _config->find( "out-header" );
    if ( it != _config->end() )
        header = (*it).second.as< std::string >();
    else
    {
        header = data.substr( 0, data.find_last_of( "." ) );
        header += ".ph";
    }
    
    _out_header_filename = header;
    _out_data_filename = data;
    
    _print_filenames();
}



void 
write_op::setup_stage_1()
{}



void 
write_op::setup_stage_2()
{
    size_t initSize = _point_info->get_point_count() / 100;
    if ( initSize > 1 << 17 )
        initSize = 1 << 17;
    else if ( initSize < 1 << 14 ) 
        initSize = 1 << 14;
        
    _output_size_in_bytes = _factory->get_output_size_in_bytes();
    
    LOGVERBOSE << "writing points of " << _output_size_in_bytes << " bytes." 
        << std::endl;
    
    _mmap_file();
}



void
write_op::_mmap_file()
{
    // setting up the file / io handling
    // mmap the outfile
    boost::iostreams::mapped_file_params params( _out_data_filename );
    params.mode = std::ios_base::out;
    params.new_file_size 
        = _point_info->get_point_count() * _output_size_in_bytes;
    
    _output_file.open( params );
    if ( ! _output_file.is_open() )
    {
        STREAM_EXCEPT( "Memory-mapping output file failed." );
    }
    _output_ptr = _current_output_ptr = _output_file.data();
    
    LOGDEBUG << "outfile mmapped to range( " << (void*)_output_ptr << ", " 
        << (void*)(_output_ptr + _output_size_in_bytes * 
                   _point_info->get_point_count() ) << ")." 
        << std::endl;    
}



void
write_op::register_release_notification_target( 
	point_release_notification_target* target )
{
	if ( target == 0 )
	{
		throw exception( "tried to register nullpointer as notification target.",
			SPROCESS_HERE );
	}
	_release_notification_targets.push_back( target );
}



void
write_op::_on_release( stream_point* point )
{
	std::deque< point_release_notification_target* >::iterator it, it_end;
	it		= _release_notification_targets.begin();
	it_end	= _release_notification_targets.end();
	for( ; it != it_end; ++it )
	{
		(*it)->on_release( point );
	}
}


void
write_op::prepare_shutdown()
{
    #if 0
    LOGINFO
        << "wrote " << _number_of_points_written_to_output << " points to output."
        << std::endl;
    #endif
    
    _print_filenames();
}


void
write_op::_print_filenames()
{
    LOGINFO
        << "output header                   " << _out_header_filename << "\n"
        << "output data                     " << _out_data_filename
        << std::endl;
}



write_op_options::write_op_options()
    : op_options( "Deferred write")
{
    _op_options.add_options()
        ( "out-file,o", boost::program_options::value< std::string >(),
            "Output data file.\n"
            "  If no output file is specified, the input filename will be used as basename for the output files. "
            "E.g. for an input data file 'bunny.points', the output data file will be called 'bunny.out.points' and the output header will be called 'bunny.out.ph'.\n"
            )
        ( "out-header", boost::program_options::value< std::string >(),
            "Output header file.\n" 
            "  If only the data file but no header is specified, the header will have the same name as the data file but use the suffix '.ph'. "
            "E.g. for an ouput data file 'processed_bunny.points', the output header will be called 'processed_bunny.ph'."
        )
        ;
    _pos_op_options.add( "out-file", 1 );
    _pos_op_options.add( "out-header", 1 );

}


} // namespace stream_process
