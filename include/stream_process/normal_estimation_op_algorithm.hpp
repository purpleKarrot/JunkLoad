#ifndef __STREAM_PROCESS__NORMAL_ESTIMATION_OP_ALGORITHM__HPP__
#define __STREAM_PROCESS__NORMAL_ESTIMATION_OP_ALGORITHM__HPP__

#include <stream_process/VMMLibIncludes.h>

#include <stream_process/op_algorithm.hpp>
#include <stream_process/rt_struct.h>
#include <stream_process/neighbor.h>

namespace stream_process
{

// hp_float_t is used for precision-sensitive operations
// to prevent numeric errors when using single-precision floats as float_t

template<
    typename float_t        = float,
    typename hp_float_t     = double
    >
class normal_estimation_op_algorithm : public op_algorithm
{
public:
    typedef vmml::vector< 3, float_t >          vec3;
    typedef vmml::vector< 3, hp_float_t >       vec3hp;
    typedef vmml::vector< 3, size_t >           vec3s;

    typedef vmml::matrix< 3, 3, hp_float_t >    mat3hp;
    typedef vmml::matrix< 4, 4, hp_float_t >    mat4hp;

    normal_estimation_op_algorithm();

    virtual op_algorithm* clone();
    
    virtual void setup_stage_0();
    virtual void setup_stage_2();
    
    virtual std::string about() const;
    
    virtual bool does_provide( const std::string& name, data_type_id type_ );

protected:
	virtual void _compute( stream_point* point );

    virtual bool _singular_values_ok( stream_point* point );
    virtual void _compute_normal_from_triangles( stream_point* point );
    virtual void _compute_normal_from_covariance( stream_point* point );

    // created attributes
    rt_struct_member< vec3 >            _normal;
    rt_struct_member< mat4hp >          _mls_covariance;

    // used attributes
    rt_struct_member< vec3 >            _position;
    rt_struct_member< neighbor >        _neighbors;
    
    size_t                              _max_neighbors;
    
    svd< hp_float_t >                   _svd;
        
    vec3        _major_axis;
    vec3        _minor_axis;
    vec3        _tmp_normal;

}; // class normal_estimation_op_algorithm


template< typename float_t, typename hp_float_t >
normal_estimation_op_algorithm< float_t, hp_float_t >::
normal_estimation_op_algorithm()
    : _normal(          "normal" )
    , _mls_covariance(  "tmp_covar" )
    , _position(        "position" )
    , _neighbors(       "neighbors" )
{
    set_name( "normal estimation" );
}


template< typename float_t, typename hp_float_t >
void
normal_estimation_op_algorithm< float_t, hp_float_t >::
_compute( stream_point* point )
{
    mat4hp& covariance  = point->get( _mls_covariance );

    _svd.compute_3x3( covariance );

    if ( _singular_values_ok( point ) )
        _compute_normal_from_covariance( point );
    else
        _compute_normal_from_triangles( point );
}



template< typename float_t, typename hp_float_t >
bool
normal_estimation_op_algorithm< float_t, hp_float_t >::
_singular_values_ok( stream_point* point )
{
    const vec3hp& sigma     = _svd.get_sigma();
    const hp_float_t limit  = 0.0000000001;

    size_t k = 0;
    for( size_t index = 0; index < 3; ++index )
    {
        if ( fabs( sigma[ index ] ) < limit )
            ++k;
    }
    if ( k > 1 )
    {
        // two singular values zero => points form a line
        LOGINFO
            << "multiple zero singular values found during SVD of covariance."
            << std::endl;

        return false;
    }
    
    return true;

}



template< typename float_t, typename hp_float_t >
void
normal_estimation_op_algorithm< float_t, hp_float_t >::
_compute_normal_from_triangles( stream_point* point )
{
    vec3& position          = point->get( _position ); 
    vec3& normal            = point->get( _normal ); 
    neighbor* neighbors     = point->get_ptr( _neighbors );
    
    // find alternate normal
    normal = 0.0;
    
    size_t max = _max_neighbors - 1;
    for ( size_t k = 0; k < max; ++k ) 
    {
        const vec3f& nb_pos = neighbors[ k ].get_point()->get( _position ); 
        const vec3f& nb_pos2 = neighbors[ k+1 ].get_point()->get( _position );

        // average from triangles
        _tmp_normal.computeNormal( position, nb_pos, nb_pos2 );

        normal += _tmp_normal;
    }
    normal.normalize();
}




template< typename float_t, typename hp_float_t >
void
normal_estimation_op_algorithm< float_t, hp_float_t >::
_compute_normal_from_covariance( stream_point* point )
{
    // if we're here we know that the svd got reasonable results, so:
   
    const mat3hp& u     = _svd.get_u();
    const vec3hp& sigma = _svd.get_sigma();
    
    const vec3s& order  = _svd.get_order();
    
    // get normal to fitting plane (cross-product of largest pair)
    for( size_t index = 0; index < 3; ++index )
    {
        _minor_axis( index ) = u( index, order( 1 ) );
        _major_axis( index ) = u( index, order( 2 ) );
    }

    _major_axis.normalize();
    _minor_axis.normalize();

    vec3& normal = point->get( _normal ); 
    
    normal.cross( _minor_axis, _major_axis );
    normal.normalize();
}




template< typename float_t, typename hp_float_t >
void
normal_estimation_op_algorithm< float_t, hp_float_t >::
setup_stage_0()
{
    // -- required inputs --
    _require( _position );
    _require( _neighbors );
    _require( _mls_covariance );
    
    get_data_type_id_from_type< float_t > get_float_type;
    _reserve_array( _normal, get_float_type(), 3, IO_WRITE_TO_OUTPUT );
}



template< typename float_t, typename hp_float_t >
void
normal_estimation_op_algorithm< float_t, hp_float_t >::
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
normal_estimation_op_algorithm< float_t, hp_float_t >::
about() const
{
    return "estimates the normal based on the points covariance";
}



template< typename float_t, typename hp_float_t >
bool
normal_estimation_op_algorithm< float_t, hp_float_t >::
does_provide( const std::string& name, data_type_id type_ )
{
    if ( name == _normal.get_name() )
    {
        return true;
    }
	return false;
}


template< typename float_t, typename hp_float_t >
op_algorithm*
normal_estimation_op_algorithm< float_t, hp_float_t >::clone()
{
    return new normal_estimation_op_algorithm< float_t, hp_float_t >( *this );
}


} // namespace stream_process

#endif

