#ifndef __STREAM_PROCESS__STREAM_OP__H__
#define __STREAM_PROCESS__STREAM_OP__H__

/*
 *  stream_op.h
 *  StreamProcessing
 *
 *  Created by Renato Pajarola on Mon May 24 2004.
 *  Copyright (c) 2004 UC Irvine. All rights reserved.
 *
 */

#include <iostream>

#include "StreamProcessing.h"

#include "stream_point.h"
#include "chain_config.h"
#include "op_common.hpp"
#include "options.h"
#include "op_options.h"

namespace stream_process
{

class stream_op : public op_common
{
public:
    stream_op();
    virtual ~stream_op() {};
  
    virtual void pull_push() = 0;
    virtual stream_point* front() = 0;

    virtual void pop_front() = 0;
    virtual void clear_stage();
    
    // smallest unprocessed element in this or previous stages
    inline virtual size_t smallest_element();	
    // smallest reference of any unprocessed element in this or previous stages
    inline virtual size_t smallest_reference();	

    virtual size_t size() const = 0;

    virtual void attach_to( stream_op* parent );
    virtual bool does_provide( const std::string& name, data_type_id type_ );
    virtual void check_requirements() const;

    virtual void setup_stage_0();
    virtual void setup_stage_1();     
    virtual void setup_stage_2();

    virtual void prepare_processing();
    virtual void prepare_shutdown();

protected:
    stream_op*		prev;

	size_t			_index_in_op_chain;
	static size_t	_stream_op_count;

}; // class stream_op


// typedefs 
typedef std::list< stream_op* > OpList;
typedef std::vector< stream_op* > OpVector;


inline 
size_t 
stream_op::smallest_element()
{
    return ( prev ) ? prev->smallest_element() : UINT_MAX;
}



inline
size_t 
stream_op::smallest_reference()
{
    return ( prev ) ? prev->smallest_reference() : UINT_MAX;
}



} // namespace stream_process

#endif
