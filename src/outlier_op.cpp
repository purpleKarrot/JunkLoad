/*
 *  outlier_op.cpp
 *  StreamProcessing
 *
 * created by jonas bösch
 *
 */

#include "outlier_op.hpp"

#include "point_info.hpp"
#include <cmath>


namespace stream_process
{

outlier_op::outlier_op()
    : stream_op()
    , _position(        "position" )
    , _normal(          "normal" )
    , _neighbors(       "neighbors" )
    , _nb_count(        "nb_count" )
    , _max_ref_index(   "max_ref_index" )
    , _scales(          "scales" )
    , _points_in_op( 0 )
    , _min_out_dist( std::numeric_limits< float >::max() )
    , _max_out_dist( std::numeric_limits< float >::min() )
    , _avg_out_dist( 0 )
{
    set_name( "outlier" );
}



outlier_op::~outlier_op()
{}



void 
outlier_op::pull_push()
{
    stream_point* point;
    // pull elements from previous operator
    while ( point = prev->front() ) 
    {
        prev->pop_front();
        ++_points_in_op;
        _in_buffer.push_back( point );
    }

    while ( ! _in_buffer.empty() )
    {
        point = _in_buffer.front();

        // only process element if all neighbor normals are present
        const uint32_t max_ref_index = point->get( _max_ref_index );
        
        if ( max_ref_index < prev->smallest_element() )
        {
            _compute_outlier_distances( point );
            _in_buffer.pop_front();
            _out_buffer.push_back( point );
        }
        else
            break;
    }
}



void 
outlier_op::_compute_outlier_distances( stream_point* point )
{
    // get point members
    const vec3f& position   = point->get( _position );
    const vec3f& normal     = point->get( _normal );
    uint32_t& nb_count      = point->get( _nb_count );
    neighbor* neighbors     = point->get_ptr( _neighbors );

    // nb iterators
    neighbor* current_nb    = neighbors;
    neighbor* nb_end        = neighbors + nb_count;
    
    // compute the tangent plane for this point
    // we already have the normal
    float distance = - normal.dot( position );
    
    _out_dist_it = _outlier_distance_to_neighbors.begin();
    float distance_squared;
    float nb_point_plane_distance;

    const vec3f& scales = point->get( _scales );
    for( ; current_nb != nb_end; ++current_nb, ++_out_dist_it )
    {
        // get squared distance to nb
        const vec3f& nb_position = current_nb->get_point()->get( _position );
        //distance_squared = position.distanceSquared( nb_position );
        
        nb_point_plane_distance = normal.dot( nb_position ) + distance;
        //nb_point_plane_distance *= nb_point_plane_distance;
        
        *_out_dist_it = /*distance_squared*/ nb_point_plane_distance;
        if ( *_out_dist_it < _min_out_dist )
            _min_out_dist = *_out_dist_it;
        else if ( *_out_dist_it > _max_out_dist )
            _max_out_dist = *_out_dist_it;
        _avg_out_dist += nb_point_plane_distance;
        
        #if 0
        float test0 = nb_point_plane_distance / scales.x;
        float test1 = nb_point_plane_distance / scales.y;
        float test2 = nb_point_plane_distance / scales.z;
        #endif
        
        int break_ = 1;
    }
    

}



void
outlier_op::clear_stage()
{
/*    std::cout << "min out dist " << _min_out_dist << "\n"
        << "max out dist " << _max_out_dist << "\n"
        << "avg dist " << _avg_out_dist / _point_info->get_point_count()
        << std::endl;
*/
}



stream_point* outlier_op::front()
{
    return ( _out_buffer.empty() ) ? 0 : _out_buffer.front();
}



void 
outlier_op::pop_front()
{
    assert( ! _out_buffer.empty() );
    _out_buffer.pop_front();
    --_points_in_op;
}



size_t 
outlier_op::size() const 
{ 
    return _points_in_op;
}



void 
outlier_op::setup_stage_0()
{
    // -- required inputs --
    _require( _position );
    _require( _neighbors );
    _require( _nb_count );
    _require( _normal );
    _require( _scales );
}



void 
outlier_op::setup_stage_1()
{
}



void 
outlier_op::setup_stage_2() 
{
    size_t max_neighbors = 8;
    var_map::iterator it = _config->find( "nb-count" );
    if( it != _config->end() )
        max_neighbors = (*it).second.as< size_t >();

    _outlier_distance_to_neighbors.resize( max_neighbors );
}


} // namespace stream_process
