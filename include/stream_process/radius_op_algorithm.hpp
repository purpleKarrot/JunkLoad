#ifndef __STREAM_PROCESS__RADIUS_OP_ALGORITHM__HPP__
#define __STREAM_PROCESS__RADIUS_OP_ALGORITHM__HPP__

#include <stream_process/op_algorithm.hpp>
#include <stream_process/neighbor.h>
#include <stream_process/radius.hpp>

//#define SPROCESS_OLD_RADIUS_ALGO

namespace stream_process
{

template< typename integer_t = uint32_t, typename float_t = float >
class radius_op_algorithm : public op_algorithm
{
public:
    radius_op_algorithm();
    virtual ~radius_op_algorithm();
    
    virtual op_algorithm* clone();

    virtual bool is_multi_threadable();

    virtual void setup_stage_0();
    virtual void setup_stage_2();

protected:
	virtual void _compute( stream_point* point );
    
    rt_struct_member< float_t >     _radius;
    rt_struct_member< neighbor >    _neighbors;
    
    radius< float_t >               _radius_compute;
    
    size_t                          _max_neighbors;

}; // class radius_op_algorithm


template< typename integer_t, typename float_t >
radius_op_algorithm< integer_t, float_t >::radius_op_algorithm()
    : _radius(      "radius" )
    , _neighbors(   "neighbors" )
    , _radius_compute( _neighbors )
{
    set_name( "radius op" );
}



template< typename integer_t, typename float_t >
radius_op_algorithm< integer_t, float_t >::~radius_op_algorithm()
{}


template< typename integer_t, typename float_t >
op_algorithm*
radius_op_algorithm< integer_t, float_t >::clone()
{
    return new radius_op_algorithm< integer_t, float_t >( *this );
}


template< typename integer_t, typename float_t >
bool
radius_op_algorithm< integer_t, float_t >::is_multi_threadable()
{
    return true;
}



template< typename integer_t, typename float_t >
inline void
radius_op_algorithm< integer_t, float_t >::_compute( stream_point* point )
{
    float_t& radius             = point->get( _radius );
    
    radius = _radius_compute.compute_mls_support_radius( point );
}



template< typename integer_t, typename float_t >
void
radius_op_algorithm< integer_t, float_t >::setup_stage_0()
{
    // -- required inputs --
    _require( _neighbors );
    
    _reserve( _radius, IO_WRITE_TO_OUTPUT );
}



template< typename integer_t, typename float_t >
void
radius_op_algorithm< integer_t, float_t >::setup_stage_2()
{
    size_t nb_count;

    var_map::iterator it = _config->find( "nb-count" );
    if( it != _config->end() )
        nb_count = (*it).second.as< size_t >();
    else
    {
        throw exception( "nb-count is required.", SPROCESS_HERE );
    }

    _radius_compute.set_max_neighbors( nb_count );
}


} // namespace stream_process

#endif

