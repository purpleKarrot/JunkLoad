#include "stats_op.h"

#include "options.h"
#include "stream_op.h"

#include "Utils.h"
#include "Log.h"

#include "unix_utils.h"

#include "chain_manager.hpp"
#include "multi_timer.hpp"

#include <fstream>
#include <sstream>

namespace stream_process
{
stats_op::stats_op()
    : _lifetime( "lifetime" )
    , _point_index( "point_index" )
    , _start_time( 0 )
	, _largest_index( 0 )
	, _max_active_set_size( 0 )
    , _total_point_count( 0 )
{
    set_name( "stats" );
}


    
stats_op::~stats_op()
{}



void 
stats_op::insert( stream_point* point )
{
    _lifetime.set( point, multi_timer::get_current_time() );
    
    _largest_index = point->get( _point_index );

    if ( _active_set->get_size() > _max_active_set_size )
        _max_active_set_size = _active_set->get_size();
    ++_total_point_count;

    if ( _largest_index % 10000 == 0 && _largest_index != 0 )
    {
        #if 1
        LOGINFO << std::setiosflags(std::ios::left )
            //<< "write " << std::setw( 9 ) << index 
            << "element " << std::setw( 9 ) << _largest_index 
            << " active set " << std::setw( 9 ) << _active_set->get_size() 
            << " max " << std::setw( 9 ) << _max_active_set_size
        << std::resetiosflags( std::ios::left );// << std::endl;
        
        const stream_operators& ops = _chain_manager->get_stream_operators();
        stream_operators::const_iterator it     = ops.begin();
        stream_operators::const_iterator it_end = ops.end();

        LOGINFO << "  ";
        for( size_t i = 0; it != it_end; ++it, ++i )
        {
            LOGINFO << "op" << i << ": " << (*it)->size() << " ";
        }
	  LOGINFO << "\r" << std::flush;
        #else
        LOGINFO << _largest_index << " " << _active_set_fifo->size() << " "
            << _max_active_set_size << std::endl;
        #endif
        

    }

}



void 
stats_op::remove( stream_point* point )
{
    double& lifetime = point->get( _lifetime );
    lifetime = multi_timer::get_current_time() - lifetime;
    _avg_time += lifetime; 
}


void 
stats_op::setup_stage_0()
{
    _reserve( _lifetime );
}



void 
stats_op::setup_stage_2()
{}



void 
stats_op::prepare_processing()
{
    _start_time = multi_timer::get_current_time();
}



void 
stats_op::prepare_shutdown()
{
	print_end_stats();
}



void
stats_op::print_end_stats()
{
	LOGINFO << "\n\n";
	//LOGINFO << "\nstatistics:\n" << std::endl;
	
    LOGINFO 
        << "stream operators:\n"
		<< _chain_manager->get_stream_operators().get_operator_timings_string()
        << "\n";
    LOGINFO 
        << "chain operators:\n"
        << _chain_manager->get_chain_operators().get_operator_timings_string()
        << "\n";

    LOGINFO << "average point-in-stream lifetime: \n    " << _avg_time / 
        _total_point_count << " seconds.\n";

    double time_end = multi_timer::get_current_time();
    double time = time_end;
    time -= _start_time;
    Log::get().precision( 5 );
    LOGINFO << "total time:\n    " 
        << time << " seconds.\n" << std::endl;

}

    
} // namespace stream_process
