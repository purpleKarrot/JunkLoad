#ifndef __STREAM_PROCESS__COMPUTE_PARABOLOID__HPP__
#define __STREAM_PROCESS__COMPUTE_PARABOLOID__HPP__

#include <stream_process/VMMLibIncludes.h>

#include <vmmlib/lapack_gaussian_elimination.hpp>

namespace stream_process
{

template< typename T >
class compute_paraboloid
{
public:
    typedef     vmml::vector< 3, T >        vec3t;
    typedef     vmml::vector< 4, T >        vec4t;
    typedef     vmml::matrix< 3, 3, T >     mat3t;
    typedef     vmml::matrix< 4, 4, T >     mat4t;

    typedef     vmml::vector< 9, T >        vec9t;
    typedef     vmml::matrix< 9, 9, T >     mat9t;

    enum QUADRIC_SURFACE
    {
        SINGLE_SHEET,
        ELLIPSOID,
        CONE,
        
        HYPERBOLIC_PARABOLOID,
        ELLIPTICAL_PARABOLOID,
        CYLINDER_OR_PLANE
    };

    template< typename U >
    void set_position( size_t index, const vmml::vector< 3, U >& position_ );
    
    void solve()
    {
        vmml::lapack::gaussian_elimination< 1, 9, T >  tgesv;
        
        _X0 = -1.0; // b
               
        std::cout << _A0 << std::endl;

        tgesv.compute( _A0, _X0 );
                    
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
        _S( 3, 3 )   = 0.0;
        
        // (36) + ..., (54) + ...
        _S.get_sub_matrix( _A2 );
        
        _delta  = _S.det();
        _D      = _A2.det();
    
        T epsilon = 0.0001;
        
        QUADRIC_SURFACE type_ = determine_type();
        
        std::cout << "delta " << _delta << ", D " << _D << std::endl;
    
        std::cout << _X0 << std::endl;
        std::cout << _A0 << std::endl;
    
    }
    
    QUADRIC_SURFACE determine_type();
    
    void set_constraint( size_t src_index = 0, size_t index = 8 )
    {
        // 
            const T a = _A0( src_index, 0 );
            const T b = _A0( src_index, 1 );
            const T c = _A0( src_index, 2 );
            const T d = _A0( src_index, 3 );
            const T e = _A0( src_index, 4 );
            const T f = _A0( src_index, 5 );
            const T g = _A0( src_index, 6 );
            const T h = _A0( src_index, 7 );
            const T i = _A0( src_index, 8 );
            
            // aei + bfg + cdh − afh − bdi − ceg  = 0

            //a = (-b*f*g-c*d*h+b*d*i+c*e*g)/(e*i-f*h)
            _A0( index, 0 ) = (-b*f*g-c*d*h+b*d*i+c*e*g)/(e*i-f*h);
            _A0( index, 1 ) = (-a*e*i-c*d*h+a*f*h+c*e*g)/(f*g-d*i);
            _A0( index, 2 ) = (-a*e*i-b*f*g+a*f*h+b*d*i)/(d*h-e*g);
            _A0( index, 3 ) = (-a*e*i-b*f*g+a*f*h+c*e*g)/(c*h-b*i);
            _A0( index, 4 ) = (-b*f*g-c*d*h+a*f*h+b*d*i)/(a*i-c*g);
            _A0( index, 5 ) = (-a*e*i-c*d*h+b*d*i+c*e*g)/(b*g-a*h);
            _A0( index, 6 ) = (-a*e*i-c*d*h+a*f*h+b*d*i)/(b*f-c*e);
            _A0( index, 7 ) = (-a*e*i-b*f*g+b*d*i+c*e*g)/(c*d-a*f);
            _A0( index, 8 ) = (-b*f*g-c*d*h+a*f*h+c*e*g)/(a*e-b*d);
            
            #if 0
            
            _A0( index, 0 ) = x * x;
            _A0( index, 1 ) = y * y;
            _A0( index, 2 ) = z * z;
            _A0( index, 3 ) = x * y;
            _A0( index, 4 ) = y * z;
            _A0( index, 5 ) = z * x;
            _A0( index, 6 ) = x;
            _A0( index, 7 ) = y;
            _A0( index, 8 ) = z;
    
            #endif
    }
    
protected:
    mat9t       _A0;
    vec9t       _X0;
    mat4t       _S;
    mat3t       _A2;
    
    T           _D;
    T           _delta;
    
    QUADRIC_SURFACE _surface;
    
    
}; // class compute_paraboloid



template< typename T >
template< typename U >
void
compute_paraboloid< T >::set_position( size_t index,
    const vmml::vector< 3, U >& position_ )
{
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

template< typename T >
typename compute_paraboloid< T >::QUADRIC_SURFACE
compute_paraboloid< T >::
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

