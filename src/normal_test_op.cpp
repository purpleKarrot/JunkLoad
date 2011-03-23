/*
 *  normal_test_op.cpp
 *  StreamProcessing
 *
 * created by jonas bšsch
 *
 */

#include "normal_test_op.hpp"

#include <cmath>
#include <limits>

namespace stream_process
{
    
normal_test_op::normal_test_op()
    : stream_op()
    , _position(        "position" )
    , _position_copy(   "position_copy" )
    , _normal(          "normal" )
    , _normal_copy(     "normal_copy" )
    , _normal_max( std::numeric_limits< float >::min() )
    , _normal_min( std::numeric_limits< float >::max() )
    , _position_max( std::numeric_limits< float >::min() )
    , _position_min( std::numeric_limits< float >::max() )
{
    set_name( "normal_test" );
}

normal_test_op::~normal_test_op()
{}

void normal_test_op::pull_push()
{  
    stream_point* point;
    // pull elements from previous operator
    while ( ( point = prev->front() ) ) 
    {
        assert( point );
        prev->pop_front();
        test( point );
        _fifo.push_back( point );
    }
}



void 
normal_test_op::test( stream_point* point )
{
    const vec3f& normal         = point->get( _normal );
    const vec3f& normal_copy    = point->get( _normal_copy );
    const vec3f& position       = point->get( _position );
    const vec3f& position_copy  = point->get( _position_copy );
    
    vec3f tmp_normal    = normal - normal_copy;
    vec3f tmp_position  = position - position_copy;

#if 0
    vec3f::iterator it = tmp_normal.begin();
    vec3f::iterator it_end = tmp_normal.end();
    for ( ; it != it_end; ++it )
    {
        std::cout << *it << ", ";
    }
    std::cout << std::endl;
    
    std::cout << tmp_normal << std::endl;
#endif

    for( size_t i = 0; i < 3; ++i )
    {
        if ( fabs( tmp_normal[ i ] < _normal_min[ i ] ) )
            _normal_min[ i ] = tmp_normal[ i ];
        else if ( fabs( tmp_normal[ i ] > _normal_max[ i ] ) )
            _normal_max[ i ] = tmp_normal[ i ];
        if ( fabs( tmp_position[ i ] < _position_min[ i ] ) )
            _position_min[ i ] = tmp_position[ i ];
        else if ( fabs( tmp_position[ i ] > _position_max[ i ] ) )
            _position_max[ i ] = tmp_position[ i ];
    }
    
    int debug_stop = 1;
}


stream_point* 
normal_test_op::front()
{
    return ( _fifo.empty() ) ? 0 : _fifo.front();
}



void 
normal_test_op::pop_front()
{
    if ( ! _fifo.empty() )
        _fifo.pop_front();
}


void 
normal_test_op::setup_stage_0()
{}



void 
normal_test_op::setup_stage_1()
{}



void 
normal_test_op::setup_stage_2() 
{
}


size_t 
normal_test_op::size() const
{ 
    return _fifo.size(); 
}


void
normal_test_op::clear_stage()
{
    std::cout << "normal min "      << _normal_min << std::endl;
    std::cout << "normal max "      << _normal_max << std::endl;
    std::cout << "position min "    << _position_min << std::endl;
    std::cout << "position max "    << _position_max << std::endl;
}


} // namespace stream_process
