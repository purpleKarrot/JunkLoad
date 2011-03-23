#ifndef __STREAM_PROCESS__PARABOLOID_TEST__HPP__
#define __STREAM_PROCESS__PARABOLOID_TEST__HPP__

#include <stream_process/stream_process_types.hpp>
#include <stream_process/paraboloid.hpp>

namespace stream_process
{
template< typename sp_types_t >
class paraboloid_test
{
public:
    STREAM_PROCESS_TYPES
    
    struct init_data
    {
        // pos + normal of the 'current' point
        vec3            position;
        vec3            normal;
        // principal curvature estimates 
        vec3            e1;
        vec3            e2;
        sp_float_type   k1;
        sp_float_type   k2;
    };

    void initialize( const init_data& init_data_ );

    sp_float_type get_z_diff( const vec3& point )
    {
        vec3 p = point - _init_data.position;
        p = _transform * p;

        sp_float_type z = _same_sign 
            ? _paraboloid.compute_z_elliptic( p ) 
            : _paraboloid.compute_z_hyperbolic( p );
        
        return fabs( z - p.z() );
    }

protected:
    mat3    _transform;

    init_data   _init_data;
    
    bool _same_sign;

    paraboloid< sp_float_type > _paraboloid;


}; // class paraboloid_test


template< typename sp_types_t >
void
paraboloid_test< sp_types_t >::initialize( const init_data& init_data_ )
{
    const vec3& n   = init_data_.normal;
    vec3 e1  = normalize( init_data_.e1 );
    vec3 e2  = normalize( init_data_.e2 );
    
    assert( fabs( e1.length() - 1.0 ) < 0.0001 );
    assert( fabs( e2.length() - 1.0 ) < 0.0001 );
    
    // if k1 and k2 have the same sign, they describe an elliptic paraboloid, 
    // otherwise a hyberbolic paraboloid
    
    const sp_float_type& k1 = init_data_.k1;
    const sp_float_type& k2 = init_data_.k2;

    sp_float_type radius1   = 1.0 / init_data_.k1;
    sp_float_type radius2   = 1.0 / init_data_.k2;
    
    vec3 e1_axis    = e1 * radius1; // 'x'
    vec3 e2_axis    = e2 * radius2; // 'y'

    _transform.set_row( 0, e1 );
    _transform.set_row( 1, e2 );
    _transform.set_row( 2, n );
    
    _init_data = init_data_;
    
    _same_sign = k1 < 0 == k2 < 0;
    _paraboloid.initialize( 4.0 / k1, 4.0 / k2 );

}


} // namespace stream_process

#endif

