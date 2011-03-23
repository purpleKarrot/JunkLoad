#ifndef __STREAM_PROCESS__COVARIANCE_OP_ALGORITHM__HPP__
#define __STREAM_PROCESS__COVARIANCE_OP_ALGORITHM__HPP__

#include <stream_process/op_algorithm.hpp>
#include <stream_process/rt_struct_member.h>
#include <stream_process/data_types.hpp>

#include <stream_process/neighbor.h>

#include <stream_process/svd.hpp>
#include <stream_process/covariance.hpp>

namespace stream_process
{

// hp_float_t is used for precision-sensitive operations
// to prevent numeric errors when using single-precision floats as float_t

template<
    typename float_t = float,
    typename hp_float_t = double
    >
class covariance_op_algorithm : public op_algorithm
{
public:
    typedef vmml::vector< 3, float_t >          vec3;
    typedef vmml::vector< 3, hp_float_t >       vec3hp;

    typedef vmml::matrix< 4, 4, hp_float_t >    mat4hp;

    covariance_op_algorithm();

    virtual op_algorithm* clone();
    
    virtual void setup_stage_0();
    virtual void setup_stage_2();
    
    virtual std::string about() const;
    
    virtual bool does_provide( const std::string& name, data_type_id type_ );

protected:
	virtual void _compute( stream_point* point );

    // created attributes
    rt_struct_member< mat4hp >      _covariance;

    // used attributes
    rt_struct_member< vec3 >        _position;
    rt_struct_member< neighbor >    _neighbors;

    size_t      _max_neighbors;

    covariance< float_t, hp_float_t >   _covar_compute;

}; // class covariance_op_algorithm


template< typename float_t, typename hp_float_t >
covariance_op_algorithm< float_t, hp_float_t >::
covariance_op_algorithm()
    : _covariance(      "tmp_covar" )
    , _position(        "position" )
    , _neighbors(       "neighbors" )
    , _covar_compute( _position, _neighbors )
{
    set_name( "mls covariance" );
}


template< typename float_t, typename hp_float_t >
op_algorithm*
covariance_op_algorithm< float_t, hp_float_t >::clone()
{
    return new covariance_op_algorithm< float_t, hp_float_t >( *this );
}


template< typename float_t, typename hp_float_t >
void
covariance_op_algorithm< float_t, hp_float_t >::
_compute( stream_point* point )
{
    // get the covariance matrix point attribute
    mat4hp& covariance = point->get( _covariance );
    
    covariance = _covar_compute.compute( point );
}


template< typename float_t, typename hp_float_t >
void
covariance_op_algorithm< float_t, hp_float_t >::
setup_stage_0()
{
    // -- required inputs --
    _require( _position );
    _require( _neighbors );
    
    // -- auxiliary variables -- 
    get_data_type_id_from_type< hp_float_t > get_hp_type;
    _reserve_array( _covariance, get_hp_type(), 16, IO_WRITE_TO_OUTPUT );
}



template< typename float_t, typename hp_float_t >
void
covariance_op_algorithm< float_t, hp_float_t >::
setup_stage_2()
{
   var_map::iterator it = _config->find( "nb-count" );
    if( it != _config->end() )
        _max_neighbors = (*it).second.as< size_t >();

    // clamp neighbors // FIXME have a look at it again
    _max_neighbors = std::min( _max_neighbors, (size_t) 8 );
}



template< typename float_t, typename hp_float_t >
std::string
covariance_op_algorithm< float_t, hp_float_t >::
about() const
{
    return "computes the moving least squares weighted covariance";
}



template< typename float_t, typename hp_float_t >
bool
covariance_op_algorithm< float_t, hp_float_t >::
does_provide( const std::string& name, data_type_id type_ )
{
    if ( name == _covariance.get_name() )
    {
        return true;
    }
	return false;
}


} // namespace stream_process

#endif

