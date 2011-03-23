#ifndef __STREAM_PROCESS__FIT_TO_PARABOLOID__HPP__
#define __STREAM_PROCESS__FIT_TO_PARABOLOID__HPP__

#include <stream_process/VMMLibIncludes.h>

#include <vmmlib/lapack_linear_least_squares.hpp>
#include <vmmlib/lapack_svd.hpp>

namespace stream_process
{

template< size_t M, typename T >
class fit_to_paraboloid
{
public:
    enum QUADRIC_SURFACE
    {
        SINGLE_SHEET,
        ELLIPSOID,
        CONE,
        
        HYPERBOLIC_PARABOLOID,
        ELLIPTICAL_PARABOLOID,
        CYLINDER_OR_PLANE
    };

    typedef vmml::matrix< M, 9, T > matMx9;
    typedef vmml::matrix< 9, M, T > mat9xM;
    typedef vmml::matrix< 9, 9, T > mat9x9;
    typedef vmml::vector< 3, T >    vec3t;
    typedef vmml::vector< 4, T >    vec4t;
    typedef vmml::vector< 9, T >    vec9;
    typedef vmml::vector< M, T >    vecM;

    typedef vmml::matrix< 3, 3, T > mat3t;
    typedef vmml::matrix< 4, 4, T > mat4t;


    template< typename U >
    void set_position( size_t index, const vmml::vector< 3, U >& position_ );
    
    void fit( T& k1, T& k2, vmml::vector< 3, T >& translation_ );
    
    QUADRIC_SURFACE determine_type();

    const matMx9& get_A0()      { return _A0; }
    const mat3t& get_U()        { return _U; }
    const vec3t& get_sigma()    { return _sigma; }
    
protected:
    void    _compute_translation( vmml::vector< 3, T >& translation_ );
    
    matMx9  _A0;
    vecM    _B;
    vec9    _X0;

    mat4t   _S;
    mat3t   _A2;
    mat4t   _P;
    
    mat3t   _U;
    
    T       _delta;
    T       _D;
    T       _s;
    T       _p;
    T       _v;
    
    T       _a_squared;
    T       _b_squared;
    
    vec3t   _sigma;
    vec4t   _r;
    
    QUADRIC_SURFACE _surface;


}; // class fit_to_paraboloid


template< size_t M, typename T >
template< typename U >
void
fit_to_paraboloid< M, T >::
set_position( size_t index, const vmml::vector< 3, U >& position_ )
{
    //std::cout << "position " << index << ": " << position_ << std::endl; 

    const T x       = static_cast< T >( position_.x() );
    const T y       = static_cast< T >( position_.y() );
    const T z       = static_cast< T >( position_.z() );
    
    _A0( index, 0 ) = x * x;
    _A0( index, 1 ) = y * y;
    _A0( index, 2 ) = z * z;
    _A0( index, 3 ) = x * y;
    _A0( index, 4 ) = y * z;
    _A0( index, 5 ) = z * x;
    _A0( index, 6 ) = x;
    _A0( index, 7 ) = y;
    _A0( index, 8 ) = z;
}


template< size_t M, typename T >
void
fit_to_paraboloid< M, T >::
fit( T& k1, T& k2, vmml::vector< 3, T >& translation_ )
{
    // Dai et al 2007, all (x) refer to formula (X) in that paper

    //std::cout << "A0 " << _A0 << std::endl;

    _B = 1.0;
    
    vmml::lapack::linear_least_squares_xgels< M, 9, T >   lls;
    lls.compute( _A0, _B, _X0 );
    
    // (9) + (10), (50)
    _S( 0, 0 )   = _X0( 0 );
    _S( 0, 1 )   = _X0( 3 ) * 0.5;
    _S( 0, 2 )   = _X0( 5 ) * 0.5;
    _S( 0, 3 )   = _X0( 6 ) * 0.5;
    _S( 1, 0 )   = _X0( 3 ) * 0.5;
    _S( 1, 1 )   = _X0( 1 );
    _S( 1, 2 )   = _X0( 4 ) * 0.5;
    _S( 1, 3 )   = _X0( 7 ) * 0.5;
    _S( 2, 0 )   = _X0( 5 ) * 0.5;
    _S( 2, 1 )   = _X0( 4 ) * 0.5;
    _S( 2, 2 )   = _X0( 2 );
    _S( 2, 3 )   = _X0( 8 ) * 0.5;
    _S( 3, 0 )   = _X0( 6 ) * 0.5;
    _S( 3, 1 )   = _X0( 7 ) * 0.5;
    _S( 3, 2 )   = _X0( 8 ) * 0.5;
    _S( 3, 3 )   = -1.0;
    
    // (36) + ..., (54) + ...
    _S.get_sub_matrix( _A2 );
    
    _delta  = _S.det();
    _D      = _A2.det();
    
    #if 0
    print_type();
    #endif
    
    vmml::lapack_svd< 3, 3, T > svd;
    
    #if 0
    bool ok = svd.compute( _A2, _sigma );
    #else
    _U = _A2;
    bool ok = svd.compute_and_overwrite_input( _U, _sigma );
    #endif
    if ( ! ok )
    {
        throw exception( "SVD failed.", SPROCESS_HERE );
    }
    
    // (52)
    _P.set_sub_matrix( _A2 );

    _r( 0 ) = _sigma.x() * 0.5;
    _r( 1 ) = _sigma.y() * 0.5;
    _r( 2 ) = _sigma.z() * 0.5;
    _r( 3 ) = 0.0;
    
    _P.set_row( 3, _r );
    _P.set_column( 3, _r );
    
    //std::cout << " P " << _P << std::endl;
    
    _p  = _P.det();
    _s  = _delta;
    
    // (54)
    T v_tmp = sqrt( fabs( - _s / _p ) );
    
    _v = _sigma.x() < 0.0 ? -v_tmp : v_tmp;
    
    _a_squared  = _v / _sigma.x();
    _b_squared  = _v / _sigma.y();
    
    k1  = 4.0 / _a_squared;
    k2  = 4.0 / _b_squared;
    
    _compute_translation( translation_ );
    
}


template< size_t M, typename T >
void
fit_to_paraboloid< M, T >::
_compute_translation( vmml::vector< 3, T >& translation_ )
{
    const T k1  = _X0( 0 );
    const T k2  = _X0( 1 );
    const T k3  = _X0( 2 );
    const T k4  = _X0( 3 );
    const T k5  = _X0( 4 );
    const T k6  = _X0( 5 );
    const T k7  = _X0( 6 );
    const T k8  = _X0( 7 );
    const T k9  = _X0( 8 );
    
    const T r1  = _r( 0 );
    const T r2  = _r( 1 );
    const T r3  = _r( 2 );
    
    const T v = _v;
    
    // z = -(1/2)*(-k[9]*k[4]^2 +4*k[9]*k[2]*k[1]-r[3]*v*k[4]^2 +4*r[3]*v*k[2]*k[1]
    //-2*k[6]*k[7]*k[2]-2*k[6]*r[1]*v*k[2]+k[6]*k[4]*k[8]+k[6]*k[4]*r[2]*v-2*k[5]*k[1]*k[8]
    //-2*k[5]*k[1]*r[2]*v+k[5]*k[4]*k[7]+k[5]*k[4]*r[1]*v)
    //  /(k[5]*k[4]*k[6]-k[6]^2*k[2]-k[1]*k[5]^2-k[3]*k[4]^2+4*k[3]*k[2]*k[1])
    
    const T z = -0.5 * ( -k9*k4*k4 + 4.0*k9*k2*k1 - r3*v*k4*k4 + 4.0*r3*v*k2*k1 
        - 2.0*k6*k7*k2 - 2.0*k6*r1*v*k2 + k6*k4*k8 + k6*k4*r2*v - 2.0*k5*k1*k8
        - 2.0*k5*k1*r2*v + k5*k4*k7 + k5*k4*r1*v )
        / ( k5*k4*k6 - k6*k6*k2 - k1*k5*k5 - k3*k4*k4 + 4.0 * k3*k2*k1 );
        
    // y = ((2*(-k[8]-r[2]*v-k[5]*z))*k[1]+k[4]*k[7]+k[4]*r[1]*v+k[4]*k[6]*z)
    //      /(-k[4]^2+4*k[2]*k[1])
    
    const T y = (( 2.0 * ( -k8 - r2*v - k5*z )) * k1 + k4*k7 + k4*r1*v + k4*k6*z )
        / ( -k4*k4 + 4.0*k2*k1);
        
    // x = (1/2)*(-k[7]-r[1]*v-k[4]*y-k[6]*z)/k[1]    
    
    const T x = 0.5 * ( -k7 - r1*v - k4*y - k6*z ) / k1;
    
    translation_.set( x, y, z );
}



template< size_t M, typename T >
typename fit_to_paraboloid< M, T >::QUADRIC_SURFACE
fit_to_paraboloid< M, T >::
determine_type()
{
    T epsilon = 1e-6;

    if ( fabs( _delta ) < epsilon ) // delta == 0
    {
        if ( fabs( _D ) < epsilon ) // D == 0 
            _surface    = CYLINDER_OR_PLANE;
        else
            _surface = CONE;
    }
    else if ( _delta > 0 )
    {
        if ( fabs( _D ) < epsilon ) // D == 0
            _surface    = HYPERBOLIC_PARABOLOID;
        else
            _surface    = SINGLE_SHEET;
    }
    else
    {
        if ( fabs( _D ) < epsilon ) // D == 0 
            _surface    = ELLIPTICAL_PARABOLOID;
        else
            _surface    = ELLIPSOID;
    }
    return _surface;
}


} // namespace stream_process

#endif

