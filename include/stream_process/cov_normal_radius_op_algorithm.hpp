#ifndef __STREAM_PROCESS__COV_NORMAL_RADIUS_OP_ALGORITHM__HPP__
#define __STREAM_PROCESS__COV_NORMAL_RADIUS_OP_ALGORITHM__HPP__

#include <stream_process/normal_estimation_op_algorithm.hpp>

#include <stream_process/radius.hpp>
#include <stream_process/covariance.hpp>

namespace stream_process
{

// hp_float_t is used for precision-sensitive operations
// to prevent numeric errors when using single-precision floats as float_t

template<
    typename float_t        = float,
    typename hp_float_t     = double
    >
class cov_normal_radius_op_algorithm
    : public normal_estimation_op_algorithm< float_t, hp_float_t >
{
public:
    typedef normal_estimation_op_algorithm< float_t, hp_float_t >   super;

    typedef vmml::vector< 3, float_t >          vec3;
    typedef vmml::vector< 3, hp_float_t >       vec3hp;
    typedef vmml::vector< 3, size_t >           vec3s;

    typedef vmml::matrix< 3, 3, hp_float_t >    mat3hp;
    typedef vmml::matrix< 4, 4, hp_float_t >    mat4hp;

    cov_normal_radius_op_algorithm();

    virtual op_algorithm* clone();
    
    virtual void setup_stage_0();
    
    virtual std::string about() const;
    
    virtual bool does_provide( const std::string& name, data_type_id type_ );

protected:
	virtual void _compute( stream_point* point );

    // created attributes
    rt_struct_member< float_t >         _radius;

    covariance< float_t, hp_float_t >   _covar_compute;
    radius< float_t >                   _radius_compute;

}; // class normal_estimation_op_algorithm


template< typename float_t, typename hp_float_t >
cov_normal_radius_op_algorithm< float_t, hp_float_t >::
cov_normal_radius_op_algorithm()
    : super()
    , _radius(          "radius" )
    , _covar_compute( super::_position, super::_neighbors )
    , _radius_compute( super::_neighbors )
{
    op_algorithm::set_name( "cov-normal-radius" );
}


template< typename float_t, typename hp_float_t >
void
cov_normal_radius_op_algorithm< float_t, hp_float_t >::
_compute( stream_point* point )
{
    mat4hp& covariance  = point->get( super::_mls_covariance );
    covariance          = _covar_compute.compute( point );
    
    super::_compute( point );
    
    float_t& radius     = point->get( _radius );
    radius              = _radius_compute.compute_mls_support_radius( point );
}



template< typename float_t, typename hp_float_t >
void
cov_normal_radius_op_algorithm< float_t, hp_float_t >::
setup_stage_0()
{
    get_data_type_id_from_type< hp_float_t > get_hp_float_type;
    _reserve_array( super::_mls_covariance, get_hp_float_type(), 16 );
    
    _reserve( _radius, IO_WRITE_TO_OUTPUT );
    
    super::setup_stage_0();
}

template< typename float_t, typename hp_float_t >
std::string
cov_normal_radius_op_algorithm< float_t, hp_float_t >::
about() const
{
    return "mls covariance, normal, radius";
}



template< typename float_t, typename hp_float_t >
bool
cov_normal_radius_op_algorithm< float_t, hp_float_t >::
does_provide( const std::string& name, data_type_id type_ )
{
    if ( name == super::_mls_covariance.get_name() )
        return true;
    if ( name == _radius.get_name() )
        return true;
	return false;
}


template< typename float_t, typename hp_float_t >
op_algorithm*
cov_normal_radius_op_algorithm< float_t, hp_float_t >::clone()
{
    return new cov_normal_radius_op_algorithm< float_t, hp_float_t >( *this );
}



} // namespace stream_process

#endif

