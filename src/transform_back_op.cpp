/*
 *  transform_back_operator.cpp
 *  StreamProcessing
 *
 *  Created by Renato Pajarola on Wed Jul 14 2004.
 *  Copyright (c) 2004 UC Irvine. All rights reserved.
 *
 */

#include "transform_back_op.hpp"

#include "VMMLibIncludes.h"

#include "point_info.hpp"
#include "chain_manager.hpp"

#include <algorithm>

namespace stream_process
{

transform_back_op::transform_back_op() 
    : stream_op()
    , _point_index( "point_index" )
{
    set_name( "back_transform" );
}



void 
transform_back_op::pull_push()
{
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
        const uint32_t& index = point->get( _point_index );

        // only write to stream if not used by previous ones anymore
        if ( index < stream_op::smallest_reference() ) 
        {
            _fifo.pop_front();
            _transform_back( point );
            _out_queue.push_back( point );
        } 
        else
            break;
    }
}



void
transform_back_op::_transform_back( stream_point* point )
{
    _transform_back_all_of_type< vec3f >( point, _vec3f_attributes_to_transform );
}



stream_point* 
transform_back_op::front()
{
    return ( _out_queue.empty() ) ? 0 : _out_queue.front();
}



void 
transform_back_op::pop_front()
{
    if ( ! _out_queue.empty() )
    {
        _out_queue.pop_front();
    }
}



void
transform_back_op::setup_stage_0()
{}



void 
transform_back_op::setup_stage_1()
{}



void 
transform_back_op::setup_stage_2()
{
    // TODO add more types?
    
    point_info::iterator it     = _point_info->begin();
    point_info::iterator it_end = _point_info->end();
    
    for( ; it != it_end; ++it )
    {
        rt_struct_member_info& info = *it;
        if ( info.has_setting( "transform_back" ) )
        {
            if ( info.get_setting( "transform_back" ) == "true" )
            {
                if ( info.type == SP_FLOAT_32 && info.array_size == 3 )
                {
                    rt_struct_member< vec3f > accessor = 
                        _point_info->get_accessor_for_attribute< vec3f >( info.name );
                    _vec3f_attributes_to_transform.push_back( accessor );
                }
            }
        }
    }
    
    
    const mat4f& tf = _chain_manager->get_preprocess_shared_data().get_transformation();
    if ( ! tf.getInverse( _inverse_transform ) )
    {
        throw exception( "transformation matrix is not invertible.", 
            SPROCESS_HERE );
    }
}

} // namespace stream_process
