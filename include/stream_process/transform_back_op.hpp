#ifndef __STREAM_PROCESS__TRANSFORM_BACK_OP__H__
#define __STREAM_PROCESS__TRANSFORM_BACK_OP__H__

#include "stream_op.h"

#include <deque>
#include <vector>
#include "rt_struct_member.h"

namespace stream_process
{

/**
*
* @brief back transformation operator
*
* during the preprocess, the point set was transformed to minimize memory
* usage. this operator transforms the point set back to the original.
*
* @author jonas boesch
*
*/

class transform_back_op 
    : public stream_op 
{
public:
    transform_back_op();

    virtual void pull_push();
    virtual stream_point* front();
    virtual void pop_front();

    virtual size_t size() const { return _fifo.size(); };

    virtual void setup_stage_0();
    virtual void setup_stage_1();
    virtual void setup_stage_2();

protected:
    void _transform_back( stream_point* point );

    template< typename T >
    void _transform_back_all_of_type( stream_point* point, 
        std::vector< rt_struct_member< T > >& accessors );

    template< typename T >
    void _transform_back_t( T& data );
    
    mat4f   _inverse_transform;
    
    std::deque< stream_point* > _fifo;
    std::deque< stream_point* > _out_queue;

    // transform_back_op uses these members
    rt_struct_member< uint32_t >    _point_index;
    
    std::vector< rt_struct_member< vec3f > > _vec3f_attributes_to_transform;
    // FIXME TODO more types

}; // class transform_back_op



template< typename T >
void 
transform_back_op::_transform_back_all_of_type( stream_point* point, 
    std::vector< rt_struct_member< T > >& accessors )
{
    typename std::vector< rt_struct_member< T > >::iterator it
        = accessors.begin();
    typename std::vector< rt_struct_member< T > >::iterator it_end
        = accessors.end();
    
    for ( ; it != it_end; ++it )
    {
        rt_struct_member< T >& accessor = *it;
        T& data = point->get( accessor );
        _transform_back_t< T >( data );
    }
    
}



template< typename T >
void
transform_back_op::_transform_back_t( T& data )
{
    data = _inverse_transform * data;
}


} // namespace stream_process

#endif
