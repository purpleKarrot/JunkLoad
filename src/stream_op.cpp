/*
 *  stream_op.cpp
 *  StreamProcessing
 *
 *  Created by Renato Pajarola on Wed Jul 14 2004.
 *  Copyright (c) 2004 UC Irvine. All rights reserved.
 *
 */

#include "stream_op.h"
#include "chain_manager.hpp"

namespace stream_process
{

size_t stream_op::_stream_op_count = 0;

stream_op::stream_op()
    : op_common()
    , prev( 0 )
{
	_index_in_op_chain = _stream_op_count;
	++_stream_op_count;
}



void 
stream_op::pop_front()
{}



void 
stream_op::setup_stage_0()
{}



void 
stream_op::setup_stage_1()
{}



void 
stream_op::setup_stage_2()
{}



void
stream_op::attach_to( stream_op* parent )
{
    prev = parent;
}



bool
stream_op::does_provide( const std::string& name, data_type_id type_ )
{
    bool provides = false;
    std::list< rt_struct_member_info* >::const_iterator it
        = _members.begin();
    std::list< rt_struct_member_info* >::const_iterator it_end
        = _members.end();

    for( ; !provides && it != it_end; ++it )
    {
        if ( (*it)->name == name )
        {
            provides = true;
            if ( (*it)->type != type_ )
            {
                if ( type_ != SP_UNKNOWN_DATA_TYPE &&
                    (*it)->type != SP_UNKNOWN_DATA_TYPE )
                {
                    data_type_helper& helper = data_type_helper::get_singleton();
                    // FIXME 
                    std::cerr << get_name() << " requests member of type ";
                    std::cerr << helper.get_default_name( type_ ) << " but got "
                        << "type " << helper.get_default_name( (*it)->type )
                        << "." << std::endl;
                }
            }
        }
    }
    if ( provides )
        return provides;
    else if ( prev )
        return prev->does_provide( name, type_ );
    else
        return false;
}



void
stream_op::check_requirements() const
{
    std::vector< rt_struct_user::name_type_pair >::const_iterator it 
        = _required_inputs.begin();
    std::vector< rt_struct_user::name_type_pair >::const_iterator it_end
        = _required_inputs.end();
    for( ; it != it_end; ++it )
    {
        if ( prev && prev->does_provide( it->first, it->second ) )
        {
            // everything is ok;
        }
        else
        {
			std::cerr << "operator '" << get_name() << "' requires input member '"
				<< it->first << "' which is not provided by previous operators."
				<< std::endl;
            throw exception( 
				std::string( get_name() + " requires input " + it->first 
					+ " which is not provided by previous operators." ), 
				SPROCESS_HERE );
        }
    }
}



void 
stream_op::clear_stage()
{}



void
stream_op::prepare_processing()
{}



void
stream_op::prepare_shutdown()
{}



} // namespace stream_process

