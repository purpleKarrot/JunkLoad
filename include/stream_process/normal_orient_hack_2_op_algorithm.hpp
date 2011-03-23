#ifndef __STREAM_PROCESS__NORMAL_ORIENT_HACK_2_OP_ALGORITHM__HPP__
#define __STREAM_PROCESS__NORMAL_ORIENT_HACK_2_OP_ALGORITHM__HPP__

#include <stream_process/op_algorithm.hpp>
#include <stream_process/rt_struct_member.h>
#include <stream_process/data_types.hpp>

#include <stream_process/neighbor.h>

namespace stream_process
{

template< typename float_t, typename integer_t >
class normal_orient_hack_2_op_algorithm : public op_algorithm
{
public:
    typedef vmml::vector< 3, float_t >      vec3;
    
    normal_orient_hack_2_op_algorithm();
    virtual op_algorithm* clone();
    
    virtual std::string about() const;
    
    virtual void setup_stage_2();
    
protected:
	virtual void _compute( stream_point* point );

    rt_struct_member< vec3 >        _normal;
    rt_struct_member< integer_t >   _point_index;
    rt_struct_member< neighbor >    _neighbors;
    
    size_t                          _nb_count;
    
}; // class normal_orient_hack_op_algorithm


template< typename float_t, typename integer_t >
normal_orient_hack_2_op_algorithm< float_t, integer_t >::
normal_orient_hack_2_op_algorithm()
    : _normal(      "normal" )
    , _neighbors(   "neighbors" )
    , _point_index( "point_index" )
    , _nb_count( 0 )
{}



template< typename float_t, typename integer_t >
op_algorithm*
normal_orient_hack_2_op_algorithm< float_t, integer_t >::
clone()
{
    return new normal_orient_hack_2_op_algorithm< float_t, integer_t >( *this );
}



template< typename float_t, typename integer_t >
void
normal_orient_hack_2_op_algorithm< float_t, integer_t >::
setup_stage_2()
{
   var_map::iterator it = _config->find( "nb-count" );
    if( it != _config->end() )
        _nb_count = (*it).second.as< size_t >();
}


template< typename float_t, typename integer_t >
std::string
normal_orient_hack_2_op_algorithm< float_t, integer_t >::
about() const
{
    return "normal orientation hack 2";
}


template< typename float_t, typename integer_t >
void
normal_orient_hack_2_op_algorithm< float_t, integer_t >::
_compute( stream_point* point )
{
    neighbor* neighbors = point->get_ptr( _neighbors );
    neighbor* current_nb    = neighbors;
    neighbor* neighbors_end = neighbors + _nb_count;
    
    neighbor* smallest_nb       = 0;
    size_t smallest_nb_index    = 0;
    size_t point_index          = point->get( _point_index );

    vec3  xn = 0;
    bool inv_test = false;
    for( ; current_nb != neighbors_end; ++current_nb )
    {
        if ( ! smallest_nb )
        {
            smallest_nb = current_nb;
            smallest_nb_index = current_nb->get_point()->get( _point_index );
        }
        else if ( current_nb->get_point()->get( _point_index ) < smallest_nb_index )
        {
            smallest_nb = current_nb;
            smallest_nb_index = current_nb->get_point()->get( _point_index );
        }
    }
    vec3& normal = point->get( _normal );
    
    if ( dot( normal, smallest_nb->get_point()->get( _normal )  ) < -0.0 ) 
    {
        normal *= -1.0;
    }
}



} // namespace stream_process

#endif

