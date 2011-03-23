/*
 *  read_operator.cpp
 *  StreamProcessing
 *
 *  @author Renato Pajarola, 2004
 *  @author Jonas Boesch, 2006
 *
 */

#include "read_op.h"
#include "read_op_barrier.hpp"
#include "active_set.h"


#include "chain_manager.hpp"
#include "chain_operators.hpp"

#include "Utils.h"

// for endian conversion
#include <netinet/in.h>
#include <arpa/inet.h>

#include <cassert>

namespace stream_process
{

extern vec3f vmin, vmax;

read_op::read_op()
    : stream_op()
	, _point( 0 )
	, _header_filename()
	, _source_filename()
	, _in_bytes( 0 )
	, _input_ptr( 0 )
	, _current_position( 0 )
	, _tmp_input( 0 )
	, _tmp_input_position( 0 )
	, _tmp_point_position( 0 )
    , _index( 0 )
	, _input_point_count( 0 )
    , _point_index( "point_index" )
    , _min_ref_index( "min_ref_index" )
    , _max_ref_index( "max_ref_index" )
	, _input_members()
	, _inputs()
	, _input_readers()
    , _chain_operators( & _chain_manager->get_chain_operators() )
    , _preprocessing_is_enabled( false )
    , _min_unsorted_index( 0 )
    , _read_op_barrier( 0 )
    , _number_of_points_read_from_input( 0 )
    , _precision_limit( 10.0 * sqrt( std::numeric_limits< float >::min() ) )
{
    set_name( "read" );
}



read_op::~read_op()
{
    delete[] _tmp_input;
    delete _read_op_barrier;
}


stream_point*
read_op::_read_next_point()
{
    // allocate space for new point
    stream_point* point = _factory->create();
    assert( _factory->is_valid_ptr( point ) );

    // in single-threaded mode, the check function is empty.
    // in multi-threaded mode, it will block until the preprocess has 
    // finished on enough points to proceed.
    _read_op_barrier->check();
    
    // copy point data to memory - we cannot use the point buffer 
    // allocatd from the factory yet, because we might need to
    // convert data and sizes might differ.
    memcpy( _tmp_input, _current_position, _in_bytes );
    _current_position  += _in_bytes;
    
    ++_number_of_points_read_from_input;

    _tmp_input_position = _tmp_input;
    _tmp_point_position = reinterpret_cast< char* >( point );
    
    // read data for all members and convert and endian convert if necessary
    // and copy the converted data to the point memory 
    std::vector< binary_reader* >::iterator it = _input_readers.begin();
    std::vector< binary_reader* >::iterator it_end = _input_readers.end();
    for( ; it != it_end; ++it )
    {
        (*it)->read( _tmp_input_position, _tmp_point_position );

        _tmp_input_position += (*it)->read_size;
        _tmp_point_position += (*it)->read_size; // TODO
    }

    // FIXME hack to remove points with identical position from stream
    if ( _index == 0 )
    {
        _last_position = point->get( _position );
    }
    else
    {
        const vec3f& pos = point->get( _position );

        if (   fabs( pos.x() - _last_position.x() ) < _precision_limit
            && fabs( pos.y() - _last_position.y() ) < _precision_limit
            && fabs( pos.z() - _last_position.z() ) < _precision_limit
            )
        {
            _factory->destroy( point );
            return 0;
        }
        else
            _last_position = pos;
    }

    return point;
}

void 
read_op::pull_push()
{
    if ( _number_of_points_read_from_input < _input_point_count )
    {
        stream_point* point = _read_next_point();
        
        while( ! point ) 
        {
            if ( _number_of_points_read_from_input >= _input_point_count )
                return;

            point = _read_next_point();
        }
        
        assert( point );

        // write index to point
        point->set( _point_index, _index );
        // set min and max ref indices to itself
        point->set( _min_ref_index, _index );
        point->set( _max_ref_index, _index );
        
        ++_number_of_points_written_to_stream;
        
        _multi_timer.stop();
        _on_insertion( _point );
        _chain_manager->get_chain_operators().insert( point );
        _multi_timer.start();

        _chain_manager->set_largest_element_index( _index );
        
        _point = point;
    }
    else
        _point = 0;
}




stream_point* 
read_op::front()
{
    return _point;
}



void 
read_op::pop_front()
{
    // FIXME
    if ( !_point )
        std::cerr << "read_op::pop_front() - called with no buffered element." 
            << std::endl;
    _point = 0;
    ++_index;
}



size_t 
read_op::size() const
{
  return 0; //n-index;
}



size_t 
read_op::position() const 
{ 
    return _index; 
}



void read_op::_register_point_attributes()
{
    // add all point attributes to the _members vectors
    point_info::iterator it     = _point_info->begin();
    point_info::iterator it_end = _point_info->end();

    rt_struct_member_info* info; 
    for( ; it != it_end; ++it )
    {
        info = new rt_struct_member_info( *it );
        if ( info->name == "position" )
        {
            info->set_setting( "transform_back", "true" );
        }
        else if ( info->name == "normal" )
        {
            info->set_setting( "transform_back", "true" );
        }

        _members.push_back( info );
    }
    
    
}



void 
read_op::_read_header()
{
    // read header data into point_info structure
    _mapped_point_data->setup_point_info( _header_filename, _point_info );
}



void 
read_op::_mmap_file()
{
    _mapped_point_data->setup_mmap( _source_filename );

    _input_ptr = _mapped_point_data->get_data_ptr();
    _current_position = _input_ptr;
}



void 
read_op::setup_stage_0()
{
    // we setup requests for the data elements we will need in this operator
    _reserve( _point_index );
    _reserve( _min_ref_index );
    _reserve( _max_ref_index );
  
    // check for preprocessing mode 
    var_map::iterator it = _config->find( "preprocess-data" );
    if ( it != _config->end() )
        _preprocessing_is_enabled = (*it).second.as< bool >();

    if ( _preprocessing_is_enabled )
    {
        // nothing to do
    }
    else
    {
        std::string header;
        std::string data;
        
        // get filenames from config
        var_map::iterator it = _config->find( "in-file" );
        if ( it != _config->end() )
            data = (*it).second.as< std::string >();

        it = _config->find( "in-header" );
        if ( it != _config->end() )
            header = (*it).second.as< std::string >();
        else
        {
            header = data.substr( 0, data.find_last_of( "." ) );
            header += ".ph";
            
        }
            
        _header_filename = header;
        _source_filename = data;

        _print_filenames();
    }
}



void 
read_op::setup_stage_1()
{   
    // set up the file / io handling
    if ( ! _preprocessing_is_enabled )
    {
        assert( _mapped_point_data == 0 );
        _mapped_point_data = new mapped_point_data();
        _read_header();
    }
    else
    {
        _mapped_point_data = _chain_manager->get_input_mapped_point_data();
        assert( _mapped_point_data );
 
        // the preprocess registered all the point-attributes from the 
        // data file in point_info, so we don't have to do it here.
    }

    _register_point_attributes();
    _setup_readers();
    
    _input_point_count = _point_info->get_point_count(); 
    _index = 0;

    // we copy all the active_set op's members into read_op
    // so dependency checking will work. FIXME?
    chain_operators::iterator it        = _chain_operators->begin();
    chain_operators::iterator it_end    = _chain_operators->end();

    std::list< rt_struct_member_info* >::const_iterator mit;
    std::list< rt_struct_member_info* >::const_iterator mit_end;
    
    for( ; it != it_end; ++it )
    {
        const std::list< rt_struct_member_info* >& members 
            = (*it)->get_reserved_members();
        for( mit = members.begin(), mit_end = members.end(); mit != mit_end; ++mit )
        {
            _members.push_back( new rt_struct_member_info( **mit ) );
        }
    }

}



void 
read_op::setup_stage_2()
{
    _in_bytes   = _factory->get_input_size_in_bytes();
    _tmp_input  = new char[ _in_bytes ];


    if ( _preprocessing_is_enabled )
    {
        // check for multi-threaded mode
        bool multi_threaded = false;
        var_map::iterator it = _config->find( "multi-threaded" );
        if ( it != _config->end() )
            multi_threaded = (*it).second.as< bool >();

        if (  multi_threaded )
        {
            _read_op_barrier = new preprocess_aware_read_op_barrier( 
                _chain_manager->get_preprocess_shared_data(), _index 
                );
        }
        else
            _read_op_barrier = new empty_read_op_barrier( _index );

    }
    else
    {
        // setup the source data memory map
        _mapped_point_data->setup_mmap( _source_filename );

        // create barrier instance
        _read_op_barrier = new empty_read_op_barrier( _index );
    }

    _input_ptr = _mapped_point_data->get_data_ptr();
    _current_position = _input_ptr;
}



void 
read_op::_setup_readers()
{
    point_info::iterator it     = _point_info->begin();
    point_info::iterator it_end = _point_info->end();
    for( ; it != it_end; ++it )
    {
        rt_struct_member_info& info = *it;
        if ( ! info.is_input )
            break;
        
        if ( info.name == "position" )
        {
            _position.set_offset( info.stream_offset );
        }

        switch( info.element_size_in_bytes )
        {
            case 1:
                _input_readers.push_back(
                    new bin_reader( info.array_size, 1 )
                    );
                break;
            case 2:
                if ( _point_info->requires_endian_conversion() )
                    _input_readers.push_back(
                        new endian_cv_reader_16( info.array_size )
                        );
                else
                    _input_readers.push_back(
                        new bin_reader( info.array_size, 2 )
                        );
                break;
            case 4:
                if ( _point_info->requires_endian_conversion() )
                    _input_readers.push_back(
                        new endian_cv_reader_32( info.array_size )
                        );
                else
                    _input_readers.push_back(
                        new bin_reader( info.array_size, 4 )
                        );
                break;
            default:
                throw exception( "reader for type not implemented yet.", 
                    SPROCESS_HERE );
                break;
        
        }
    }
}



void
read_op::set_mapped_point_data( mapped_point_data* mapped_point_data_ )
{
    if ( _mapped_point_data != 0 )
        throw exception( "tried overwriting mapped_point_data ", 
            SPROCESS_HERE );

    _mapped_point_data = mapped_point_data_;
}



mapped_point_data&
read_op::get_mapped_point_data()
{
    return *_mapped_point_data;
}



const mapped_point_data&
read_op::get_mapped_point_data() const
{
    return *_mapped_point_data;
}



void
read_op::register_insertion_notification_target( 
    point_insertion_notification_target* target )
{
	if ( target == 0 )
	{
		throw exception( "tried to register nullpointer as notification target.",
			SPROCESS_HERE );
	}
	_insertion_notification_targets.push_back( target );
}



void
read_op::_on_insertion( stream_point* point )
{
	std::deque< point_insertion_notification_target* >::iterator it, it_end;
	it		= _insertion_notification_targets.begin();
	it_end	= _insertion_notification_targets.end();
	for( ; it != it_end; ++it )
	{
		(*it)->on_insertion( point );
	}
}



void
read_op::prepare_shutdown()
{
    #if 0
    LOGINFO
        << "\nread_op:\npoints read from input: "
        << _number_of_points_read_from_input
        << "\nnumber of points written to stream: "
        << _number_of_points_written_to_stream
        << "\nnmber of duplicate points in input: "
        << _number_of_points_read_from_input
            - _number_of_points_written_to_stream
        << "\n"
        << std::endl;
    #else
    const size_t discarded_points = _number_of_points_read_from_input
            - _number_of_points_written_to_stream;

    if ( discarded_points != 0 )
    {
        LOGINFO
            << "\ndiscarded " 
            << _number_of_points_read_from_input
                - _number_of_points_written_to_stream
            << " duplicate points from input.\n\n";
    }
    #endif
    
    _print_filenames();
}


void
read_op::_print_filenames()
{
    if ( _header_filename.empty() && _source_filename.empty() )
        return;

    LOGINFO
        << "input header                    " << _header_filename << "\n"
        << "input data                      " << _source_filename 
        << std::endl;
}


bool
read_op::has_read_all_points_from_input()
{
    return _number_of_points_read_from_input == _input_point_count;
}



read_op_options::read_op_options()
    : op_options( "Read")
{
    _op_options.add_options()
        ( "in-file,i", boost::program_options::value< std::string >(),
            "Input data file.\n" 
            ) 
        ( "in-header,h", boost::program_options::value< std::string >(),
            "Input header file.\n"
            "  If only the data file but no header is specified, the header name is assumed to be of the same name as the data file, but with the suffix '.ph'. "
            "E.g. for an input data file called 'bunny.points', the header must be called 'bunny.ph', or must be specified explicitly."
        )
        ;
        
        _pos_op_options.add( "in-file", 1 );
        _pos_op_options.add( "in-header", 1 );
}


} // namespace stream_process
