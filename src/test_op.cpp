/*
 *  test_op.cpp
 *  StreamProcessing
 *
 * created by jonas bšsch
 *
 */

#include "test_op.h"

#include <cmath>


namespace stream_process
{
    
test_op::test_op()
    : stream_op()
    , _position(        "position" )
    , _point_index(     "point_index" )
    , _neighbors(       "neighbors" )
    , _nb_count(        "nb_count" )
    , _test(            "test" )
    , _nb_count_kd(     "cnt" )
    , _kd_neighbors(    "list" )
    , _distances(       "dist" )
    , _min_ref_index(   "min_ref_index" )
    , _max_ref_index(   "max_ref_index" )
    , _diff_nb_count( 0 )
    , _smaller_nbs( 0 )

{
    set_name( "test" );
}

test_op::~test_op()
{}

void test_op::pull_push()
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
test_op::test( stream_point* point )
{
    // neighbor test -> kd_heap vs new neighbor_op
    const uint32_t kd_count = point->get( _nb_count_kd );
    const uint32_t nb_count = point->get( _nb_count );
    float* kd_dists         = point->get_ptr( _distances );
    stream_point** kd_nbs   = point->get_ptr( _kd_neighbors );
    neighbor* nbs           = point->get_ptr( _neighbors );
    
    assert( kd_count == nb_count );
    
    uint32_t index, kd_index;
    float dist, kd_dist;
    for( size_t i = 0; i < nb_count; ++i )
    {
        index       = nbs[ i ].get_point()->get( _point_index );
        kd_index    = kd_nbs[ i ]->get( _point_index );
        dist        = nbs[ i ].get_distance();
        kd_dist     = kd_dists[ i ];
        
        if ( index != kd_index || dist != kd_dist )
        {
            if ( index == kd_index )
            {
                assert( 0 );
            }
            ++_diff_nb_count;
            if ( dist < kd_dist )
            {
                ++_smaller_nbs;
            }
        }
    }
}


stream_point* 
test_op::front()
{
    return ( _fifo.empty() ) ? 0 : _fifo.front();
}



void 
test_op::pop_front()
{
    if ( ! _fifo.empty() )
        _fifo.pop_front();
}


void 
test_op::setup_stage_0()
{
    _reserve( _test );
}



void 
test_op::setup_stage_1()
{}



void 
test_op::setup_stage_2() 
{
}


size_t 
test_op::size() const
{ 
    return _fifo.size(); 
}

void
test_op::clear_stage()
{
    std::cout << "diff nb: " << _diff_nb_count << "\n"
        << "smaller nbs: " << _smaller_nbs << std::endl;
}


} // namespace stream_process
