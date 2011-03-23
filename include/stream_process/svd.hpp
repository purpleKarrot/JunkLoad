#ifndef __STREAM_PROCESS__SVD__HPP__
#define __STREAM_PROCESS__SVD__HPP__

#include <stream_process/VMMLibIncludes.h>

namespace stream_process
{

template< typename sp_float_t >
class svd
{
public:
    typedef vmml::vector< 3, sp_float_t >		vec3;
    typedef vmml::vector< 3, size_t >			vec3s;

    typedef vmml::matrix< 3, 3, sp_float_t >	mat3;
    typedef vmml::matrix< 4, 4, sp_float_t >	mat4;

    void compute_3x3( const mat4& a );
    
    const mat3&     get_u() const;
    const vec3&     get_sigma() const;
    const mat3&     get_v() const;
    const vec3s&    get_order() const;
    
protected:
    mat3    _u;
    vec3    _sigma;
    mat3    _v;
    vec3s   _order;

}; // class svd

template< typename sp_float_t >
void
svd< sp_float_t >::compute_3x3( const mat4& a )
{
    // copy upper 3x3 submatrix into _u
    a.get_sub_matrix( _u );
    
    vmml::svdecompose( _u, _sigma, _v );

    for( size_t index = 0; index < 3; ++index )
    {
        _sigma[ index ] = fabs( _sigma[ index ] );
    }
    
    // order singular values
    _order.set( 2, 1, 0 );
    
    size_t& l1 = _order.x();
    size_t& l2 = _order.y();
    size_t& l3 = _order.z();
    
    if ( _sigma[ l1 ] > _sigma[ l2 ] )
        std::swap( l1, l2 );
    
    if ( _sigma[ l2 ] > _sigma[ l3 ] )
    {
        if ( _sigma[ l1 ] > _sigma[ l3 ] )
        {
            size_t k = l3;
            l3 = l2;
            l2 = l1;
            l1 = k;
        }
        else
            std::swap( l2, l3 );
    }

    assert( _sigma[ l1 ] <= _sigma[ l2 ] );
    assert( _sigma[ l2 ] <= _sigma[ l3 ] );

}

template< typename sp_float_t >
const typename svd< sp_float_t >::mat3&
svd< sp_float_t >::get_u() const
{
    return _u;
}



template< typename sp_float_t >
const typename svd< sp_float_t >::vec3&
svd< sp_float_t >::get_sigma() const
{
    return _sigma;
}



template< typename sp_float_t >
const typename svd< sp_float_t >::mat3&
svd< sp_float_t >::get_v() const
{
    return _v;
}



template< typename sp_float_t >
const typename svd< sp_float_t >::vec3s&
svd< sp_float_t >::get_order() const
{
    return _order;
}


} // namespace stream_process

#endif

