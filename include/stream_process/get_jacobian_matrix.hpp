#ifndef __STREAM_PROCESS__GET_JACOBIAN_MATRIX__HPP__
#define __STREAM_PROCESS__GET_JACOBIAN_MATRIX__HPP__

#include <stream_process/stream_process_types.hpp>

#include <stream_process/compute_centroid.hpp>

#include <stream_process/wendlands_weight.hpp>
#include <stream_process/pow_neg_m_weight.hpp>
#include <stream_process/local_support_weight.hpp>

#include <boost/function.hpp>
#include <boost/mem_fn.hpp>
#include <functional>

namespace stream_process
{

template< typename sp_types_t >
class get_jacobian_matrix
{
public:
    STREAM_PROCESS_TYPES
    
    get_jacobian_matrix( 
        const attribute_accessor< vec3 >& get_position, 
        const attribute_accessor< nbh_type >& get_neighbors,
        const attribute_accessor< sp_float_type >& get_radius
    )   : _get_position( get_position )
        , _get_neighbors( get_neighbors )
        , _get_radius( get_radius )
        , _compute_centroid(
            _get_position,
            _get_neighbors,
            _get_radius, 
            _weight_func,
            _weight_func_feature_size )
        , _algorithm( "central_differences" )
        , _weight_function( "local_support" )
    {
        set_algorithm( _algorithm );
        set_weight_function( _weight_function );
    }
    
    get_jacobian_matrix(
        const attribute_accessor< vec3 >& get_position, 
        const attribute_accessor< nbh_type >& get_neighbors,
        const attribute_accessor< sp_float_type >& get_radius,
        const get_jacobian_matrix& original )
        : _get_position( get_position )
        , _get_neighbors( get_neighbors )
        , _get_radius( get_radius )
        , _compute_centroid(
            _get_position,
            _get_neighbors,
            _get_radius, 
            _weight_func,
            _weight_func_feature_size )
        , _algorithm( original._algorithm )
        , _weight_function( original._weight_function )
    {
        set_algorithm( _algorithm );
        set_weight_function( _weight_function );
    }

    void set_algorithm( const std::string& algo )
    {
        if ( algo == "central_differences" )
        {
            _fill_jacobian = boost::bind( 
                &get_jacobian_matrix::using_central_differences, 
                this, _1, _2 
                );
            _algorithm = algo;
        }
        else if ( algo == "forward_differences" )
        {
            _fill_jacobian = boost::bind( 
                &get_jacobian_matrix::using_forward_differences,
                this, _1, _2 
                );
            _algorithm = algo;
        }
    }
    
    void set_weight_function( const std::string& weight_function )
    {
        if ( weight_function == "pow_neg_m" || weight_function == "pow_neg" )
        {
            _weight_func                = _pow_neg_m_weight;
            _weight_func_feature_size   = _pow_neg_m_weight;
            _weight_function            = weight_function;

        }
        else if ( weight_function == "local_support" )
        {
            _weight_func                = _local_support_weight;
            _weight_func_feature_size   = _local_support_weight;
            _weight_function            = weight_function;

        }
        else if ( weight_function == "wendland" || weight_function == "wendlands" )
        {
            _weight_func                = _wendlands_weight;
            _weight_func_feature_size   = _wendlands_weight;
            _weight_function            = weight_function;
        }
    }
    
    void using_forward_differences( mat3& a, const stream_data* point )
    {
        const vec3& pos     = _get_position( point);
        const nbh_type& nbh = _get_neighbors( point );
        
        sp_float_type delta = _get_radius( point ) * 4.0;
        sp_float_type inv_delta = 1.0 / delta;

        _compute_centroid( point, _c, pos );

        _compute_centroid( point, _cxf, pos + vec3( delta, 0, 0 ) );
        _compute_centroid( point, _cyf, pos + vec3( 0, delta, 0 ) );
        _compute_centroid( point, _czf, pos + vec3( 0, 0, delta ) );

        a.set_row( 0, ( _c - _cxf ) * inv_delta );
        a.set_row( 1, ( _c - _cyf ) * inv_delta );
        a.set_row( 2, ( _c - _czf ) * inv_delta );
    }

    void using_central_differences( mat3& a, const stream_data* point )
    {
        const vec3& pos     = _get_position( point );
        const nbh_type& nbh = _get_neighbors( point );
        
        sp_float_type delta = _get_radius( point ) * 4.0;
        
        _compute_centroid( point, _cxf, pos + vec3( delta, 0, 0 ) );
        _compute_centroid( point, _cyf, pos + vec3( 0, delta, 0 ) );
        _compute_centroid( point, _czf, pos + vec3( 0, 0, delta ) );

        _compute_centroid( point, _cxb, pos - vec3( delta, 0, 0 ) );
        _compute_centroid( point, _cyb, pos - vec3( 0, delta, 0 ) );
        _compute_centroid( point, _czb, pos - vec3( 0, 0, delta ) );
        
        sp_float_type inv_delta2    = 0.5 / delta;

        a.set_row( 0, ( _cxf - _cxb ) * inv_delta2 );
        a.set_row( 1, ( _cyf - _cyb ) * inv_delta2 );
        a.set_row( 2, ( _czf - _czb ) * inv_delta2 );
    }
    
    void operator()( mat3& a, const stream_data* point )
    {
        _fill_jacobian( a, point );
    }

protected:
    const attribute_accessor< vec3 >&           _get_position;
    const attribute_accessor< nbh_type >&       _get_neighbors;
    const attribute_accessor< sp_float_type >&  _get_radius;
    
    compute_centroid< sp_types_t, hp_float_type >   _compute_centroid;
        
    // methods
    std::string         _algorithm;
    boost::function< void ( mat3&a, const stream_data* point )> _fill_jacobian;
    
    // weight functions
    std::string         _weight_function;
    boost::function< hp_float_type ( hp_float_type )>   _weight_func;
    boost::function< void ( hp_float_type, size_t )>    _weight_func_feature_size;

    pow_neg_m_weight< hp_float_type >      _pow_neg_m_weight;
    wendlands_weight< hp_float_type >      _wendlands_weight;
    local_support_weight< hp_float_type >  _local_support_weight;
    
    // method helpers
    vec3    _c, _cxf, _cyf, _czf, _cxb, _cyb, _czb;

}; // class get_jacobian_matrix

} // namespace stream_process

#endif

