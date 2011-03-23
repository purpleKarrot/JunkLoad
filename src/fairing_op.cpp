/*
 *  fairing_operator.cpp
 *  StreamProcessing
 *
 *  Created by Renato Pajarola on Wed Jul 14 2004.
 *  Copyright (c) 2004 UC Irvine. All rights reserved.
 *
 */

#include "fairing_op.h"

#include "point_info.hpp"
#include "moved_point_reinsertion_visitor.hpp"
#include "tree_op.hpp"
#include "active_set.h"
#include "chain_manager.hpp"
// FIXME 
#include "tree_op.hpp"
#include "pr_kd_tree_node.h"

#include "write_op.h"

#define MAX_COUNT 64

namespace stream_process
{

fairing_op::fairing_op() 
    : stream_op()
    , F( 0 )
    , G( 0 )
    , _splat( false )
    , _point_index(     "point_index" )
    , _position(        "position" )
    //, _faired_position( "faired_position" )
    , _normal(          "normal" )
    , _neighbors(       "neighbors" )
    , _nb_count(        "nb_count" )
    , _min_ref_index(   "min_ref_index" )
    , _max_ref_index(   "max_ref_index" )
    , _length() // not initialized, depends on operator chain
    , _ratio()  // not initialized, depends on operator chain
    , _radius() // not initialized, depends on operator chain
    , _area(            "area" )
    , _position_copy(   "position_copy" )
    , _normal_copy(     "normal_copy" )
{
    set_name( "fairing" );
}



fairing_op::~fairing_op()
{
    delete F;
    delete G;
}



void 
fairing_op::pull_push()
{
    stream_point* point;
    IndexReference zref;
    // pull elements from previous operator
    while ( ( point = prev->front() ) ) 
    {
        prev->pop_front();
        
        const uint32_t min_ref_index = point->get( _min_ref_index );
        const uint32_t index       = point->get( _point_index );

        // update priority queue that maintains smallest referenced index in all buffers
        zref.first  = min_ref_index;    // smallest referenced index
        zref.second = index;            // from element
        ZQ.push( zref );

        // duplicate some values used for fairing
        const vec3f& position   = point->get( _position );
        
        vec3f& position_copy    = point->get( _position_copy );
        position_copy           = position;
        
        const vec3f& normal     = point->get( _normal );
        
        vec3f& normal_copy      = point->get( _normal_copy );
        normal_copy             = normal;
        
        //get area var
        float& area             = point->get( _area );
                
        if ( _splat ) // splat op is in op queue before this op
        {
            const float ratio     = point->get( _ratio );
            const float length    = point->get( _length );

            area = M_PI * ratio * length * length;

            if ( ! area > 0.0f )
            {
                LOGERROR << "Area is " << area << ", must be larger than 0." 
                    << std::endl;
                LOGERROR << "Ratio " << ratio << ", length " << length 
                    << std::endl;
                STREAM_EXCEPT( "Area is out of range." );
            }
        }
        else
        {
            float& radius     = point->get( _radius );

            area = M_PI * radius * radius;
            
            if ( ! area > 0.0f )
            {
                LOGERROR << "Area is " << area << ", must be larger than 0." 
                << std::endl;
                LOGERROR << "radius " << radius << std::endl;
                STREAM_EXCEPT( "Area is out of range." );
            }
        }
        //std::cout << " AREA " << area << std::endl;
        //if ( index > 20 )
        //    exit( 2 );
        // enter waiting queue
        _fifo.push_back( point );
    }
    
    // check queue elements
    while ( ! _fifo.empty() ) 
    {        
        point = _fifo.front();

        const uint32_t& max_ref_index   = point->get( _max_ref_index );
        const uint32_t& index           = point->get( _point_index );

        // only update coordinates of elements that have fully been processed
        if ( max_ref_index < stream_op::smallest_element() &&
             index < stream_op::smallest_reference() )
        {
            _fifo.pop_front();
        
            // perform fairing
            fair_splat( point );
      
            // recompute updated self-covariance
            //tensorProd(tmp->covar, tmp->v, tmp->v);

            // transfer to queue
            _fifo2.push_back( point );
        } 
        else
            break;
    }
}



void 
fairing_op::clear_stage()
{
    stream_point* point;
    ExtraRef eref;
    
    while ( ! _fifo.empty() ) 
    {
        point = _fifo.front();
        _fifo.pop_front();

        // perform fairing
        fair_splat( point );
        _fifo2.push_back( point );
    }
}



stream_point* 
fairing_op::front()
{
    stream_point* point( 0 );
    if ( ! _fifo2.empty() )
    {
        point = _fifo2.front();

        const uint32_t max_ref_index    = point->get( _max_ref_index );
        const uint32_t index            = point->get( _point_index );

        if ( _fifo.empty() || max_ref_index < index )
        {
            const uint32_t& nb_count    = point->get( _nb_count );
            const vec3f& pos            = point->get( _position );
            neighbor* current_nb        = point->get_ptr( _neighbors );
            neighbor* neighbors_end     = current_nb + nb_count;

            vec3f diff;
            for( ; current_nb != neighbors_end; ++current_nb )
            {
                const vec3f& nb_pos = current_nb->get_point()->get( _position );
                diff = nb_pos - pos;
                current_nb->set_distance( dot( diff, diff ) );
            }
            // FIXME ? re-sort nb list?
        }
    }
    return point;
}



void 
fairing_op::pop_front()
{
    if ( ! _fifo2.empty() ) 
    {
        const uint32_t index = _fifo2.front()->get( _point_index );
        // remove unused references
        while ( ! ZQ.empty() && (uint32_t) ZQ.top().second < index)
            ZQ.pop();
        _fifo2.pop_front();
    }
}



size_t 
fairing_op::smallest_element()
{
    if ( ! _fifo2.empty() )
    {
        return _fifo2.front()->get( _point_index );
    }
    else if ( ! _fifo.empty() )
    {
        return _fifo.front()->get( _point_index );
    }
    else
        return stream_op::smallest_element();
}



size_t 
fairing_op::smallest_reference()
{
  if (!ZQ.empty())
    return std::min( (size_t) ZQ.top().first, (size_t) stream_op::smallest_reference());
  else
    return stream_op::smallest_reference();
}



size_t 
fairing_op::size() const
{
    return _fifo.size() + _fifo2.size();
}



void 
fairing_op::fair_splat( stream_point* point )
{
    // initialize fairing
    const vec3f& pos_copy   = point->get( _position_copy );
    float factor            = point->get( _area );
    //std::cout << "sanity check " << factor << tmpPos << std::endl;

    assert( factor > 0.0 ); 
    vec3f average( pos_copy );
    average *= factor;

    float weight = factor;

    const uint32_t& nb_count    = point->get( _nb_count );
    neighbor* neighbors         = point->get_ptr( _neighbors );
    neighbor* current_nb        = neighbors; 
    neighbor* neighbors_end     = current_nb + nb_count;
    
    float dist, f, g;
    uint32_t nb_index;
    stream_point* nb;
    vec3f tmp_vec;
    for( ; current_nb != neighbors_end; ++current_nb )
    {
        nb                          = current_nb->get_point();
        nb_index                    = nb->get( _point_index );
        const vec3f& nb_pos_copy    = nb->get( _position_copy );
        const vec3f& nb_normal      = nb->get( _normal );    

        factor                      = nb->get( _area ); // get area weight
        assert( factor > 0.0 );

        // get squared distance to neighbor
        tmp_vec     = nb_pos_copy;
        tmp_vec    -= pos_copy; 
        dist        = dot( tmp_vec, tmp_vec );

        // get distance weight
        f = F->evaluate( dist );

        // get projection distance and projection onto neighbor tangential ellipse plane
        dist    = dot( tmp_vec, nb_normal ); //dotProd(vec, point->list[k]->n);
        tmp_vec     = nb_normal * dist; //scale(vec, dist, point->list[k]->n);
        tmp_vec    += pos_copy; //add(vec, point->position);

        // get projection weight
        // g = G->evaluate( dist * dist );
        dist *= dist;
        g = G->evaluate( dist );

        //factor = factor * f * g;
        factor *= f;
        factor *= g;
 
        // this assert is commented out because the original stream_proc
        // will not assert since NDEBUG is defined! 
        //assert( factor > 0.0 );
        
        if ( factor > 0.0 )
        {
            tmp_vec *= factor; //scale(vec, factor);
            average += tmp_vec; //add(average, vec);
            weight  += factor;
        }
        else 
        {
            // we break the for loop, since if the nth neighbor is too far away
            // to be considered, all further neighbors are by definition even 
            // farther away.
            break;
        }
    }
    
    // update faired position
    assert( weight > 0.0 );
    average *= 1.0 / weight; //scale(average, 1.0 / weight);
    
    //vec3f& position = point->get( _position );
    vec3f& position = point->get( _position );
    position = average;
    
    // FIXME
	moved_point_reinsertion_visitor< pr_kd_tree_node, std::less< float > > 
		visitor( _position_copy, _position );
	visitor.setup( point );
	_active_set->get_root_node().accept_visitor( &visitor );
}



void
fairing_op::setup_stage_0()
{
    _require( _position );
    _require( _normal );
    _require( _neighbors );
    _require( _min_ref_index );
    _require( _max_ref_index );

    _reserve( _area );
    //_reserve_array( _faired_position, SP_FLOAT_32, 3 );
    _reserve_array( _position_copy, SP_FLOAT_32, 3 );
    _reserve_array( _normal_copy, SP_FLOAT_32, 3 );
    
    // we need at least 64 neighbors for the fairing to work nicely.
    var_map::iterator it = _config->find( "nb-count" );
    size_t max_neighbors = 0;
    if( it != _config->end() )
        max_neighbors = (*it).second.as< size_t >();
    else
        throw exception( "nb-count config value not found.", SPROCESS_HERE );
    if ( max_neighbors < MAX_COUNT ) 
        (*it).second.as< size_t >() = MAX_COUNT;    
}




void 
fairing_op::setup_stage_1()
{
    const vec3f& vmax = _point_info->get_max();
    const vec3f& vmin = _point_info->get_min();
    vec3f d = vmax - vmin;
    _diagonal = d.length();
    /*sqrt( ( vmax[0] - vmin[0] ) * ( vmax[0] - vmin[0] ) 
                    + ( vmax[1] - vmin[1] ) * ( vmax[1] - vmin[1] ) 
                    + ( vmax[2] - vmin[2] ) * ( vmax[2] - vmin[2] ) 
                    );
    */
   //std::cout << "    diagonal: " << _diagonal << "," <<std::endl;

    F = new Gaussian< float >( 0.5f * 0.01f * _diagonal, 1000 );
    G = new Gaussian< float >( 0.5f * 0.01f * _diagonal, 1000 );
}



void 
fairing_op::setup_stage_2()
{
    _splat = _factory->has_member( "ratio" );
    if ( _splat )
    {
        _ratio.set_offset( _factory->get_member_offset( "ratio" ) );
        _length.set_offset( _factory->get_member_offset( "length" ) );
    }
    else
    {
        _radius.set_offset( _factory->get_member_offset( "radius" ) );
    }
}

} // namespace stream_process
