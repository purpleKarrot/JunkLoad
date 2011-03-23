#ifndef __STREAM_PROCESS__ALGEBRAIC_SPHERE__HPP__
#define __STREAM_PROCESS__ALGEBRAIC_SPHERE__HPP__


#include <vmmlib/vector3.h>
#include <vmmlib/vector4.h>

#include "exception.hpp"

// heavily based on Expe, (c) gael gunnebaud

namespace stream_process
{

enum as_state
{
	AS_STATE_UNKNOWN	= 0,
	AS_STATE_SPHERE		= 1,
	AS_STATE_PLANE		= 2
};



template< typename T >
class algebraic_sphere
{
public:
	algebraic_sphere();

	typedef vmml::Vector3< T > vec3;
	typedef vmml::Vector4< T > vec4;

	inline vec3& get_u13();
	inline const vec3& get_u13() const;

	inline T get_algebraic_distance( const vec3& point ) const;
	inline T get_euclidean_distance( const vec3& point ) const;
	inline vec3 get_gradient( const vec3& point ) const;

	void pratt_normalize();
	
	inline void translate( const vec3& translation );
	inline vec3 project( const vec3& point ) const;

	void determine_as_state();
	
	const vec4&	get_plane_or_sphere() const;
	as_state get_as_state() const;
	
    inline T get_euclidean_distance_iteratively( const vec3& point ) const;
    vec3 project_iteratively( const vec3& point ) const;
    
    const vec3& get_sphere_center() const;
    
protected:
	union
	{
		struct
		{
			T _coefficients[ 5 ];
		};
		struct
		{
			T _u0, _u1, _u2, _u3, _u4;
		};
	};

	vec4 _plane_or_sphere;
	as_state	_as_state;

}; // class algebraic_sphere


template< typename T >
algebraic_sphere< T >::algebraic_sphere()
	: _as_state( AS_STATE_UNKNOWN )
{
}



template< typename T >
inline vmml::Vector3< T >&
algebraic_sphere< T >::get_u13()
{
	return reinterpret_cast< vmml::Vector3< T >& >( _coefficients[1] );
}



template< typename T >
inline const vmml::Vector3< T >&
algebraic_sphere< T >::get_u13() const
{
	return reinterpret_cast< const vmml::Vector3< T >& >( _coefficients[1] );
}



template< typename T >
inline T
algebraic_sphere< T >::get_algebraic_distance( const vmml::Vector3< T >& point ) const
{
	return _u0 + get_u13().dot( point ) + _u4 * point.lengthSquared();
}



template< typename T >
inline const vmml::Vector3< T >&
algebraic_sphere< T >::get_sphere_center() const
{
	return reinterpret_cast< const vmml::Vector3< T >& >( _plane_or_sphere.x );
}


template< typename T >
inline T
algebraic_sphere< T >::get_euclidean_distance( const vmml::Vector3< T >& point ) const
{
	if ( _as_state == AS_STATE_SPHERE )
	{
		T distance = _plane_or_sphere.getDistanceToSphere( point );
		return _u4 < 0.0 ? -distance : distance;
	}
	
	if ( _as_state == AS_STATE_PLANE )
	{
		return _plane_or_sphere.getDistanceToPlane( point );
	}
	
    return get_euclidean_distance_iteratively( point );
}



template< typename T >
void
algebraic_sphere< T >::pratt_normalize()
{
    T scale_factor = 1.0 / sqrt( get_u13().lengthSquared() - 4.*_u0 * _u4 );
    for ( size_t k = 0; k < 5; ++k )
    {
	    _coefficients[k] *= scale_factor;
	}
}



template< typename T >
inline vmml::Vector3< T >
algebraic_sphere< T >::get_gradient( const vmml::Vector3< T >& point ) const
{
        return get_u13() + point * 2. * _u4;
}



template< typename T >
void
algebraic_sphere< T >::translate( const vec3& translation )
{
    _u0 = _u0 - get_u13().dot( translation ) + _u4 * translation.lengthSquared();
    get_u13() = get_u13() - 2.0 * _u4 * translation;
}



template< typename T >
vmml::Vector3< T >
algebraic_sphere< T >::project( const vec3& point ) const
{
    if ( _as_state == AS_STATE_SPHERE )
        return _plane_or_sphere.projectPointOntoSphere( point );
    
    if ( _as_state == AS_STATE_PLANE )
        return _plane_or_sphere.projectPointOntoPlane( point );
	else
	{
        return project_iteratively( point );
	}
}




template< typename T >
void
algebraic_sphere< T >::determine_as_state()
{
	if ( fabs( _u4 ) > 1e-9 )
	{
		_as_state = AS_STATE_SPHERE;
		T b = 1.0 / _u4;
		vmml::Vector3< T >& center_ 
			= *reinterpret_cast< vmml::Vector3< T >* >( &_plane_or_sphere.x );

		_plane_or_sphere.set( get_u13() * -0.5 * b, 0 );
        _plane_or_sphere.w = sqrt( center_.lengthSquared() - b * _u0 );
        
	}
	else if ( _u4 == 0.0 )
	{
		_as_state = AS_STATE_PLANE;
		T s = 1.0 / get_u13().length(); 
		_plane_or_sphere.set( 
			get_u13() * s, 
			_u0 * s
			);
	}
	else
	{
		_as_state = AS_STATE_UNKNOWN;
	}
}



template< typename T >
const vmml::Vector4< T >&
algebraic_sphere< T >::get_plane_or_sphere() const
{
	return _plane_or_sphere;
}



template< typename T >
as_state
algebraic_sphere< T >::get_as_state() const
{
	return _as_state;
}



template< typename T >
inline T 
algebraic_sphere< T >
    ::get_euclidean_distance_iteratively( const vec3& x ) const
{
    T d = 0.;
    vmml::Vector3< T > grad;
    vmml::Vector3< T > dir = get_u13() + x * 2. * _u4;
    T ilg = 1./dir.length();
    dir *= ilg;
    T ad = _u0 + get_u13().dot(x) + _u4 * x.lengthSquared();
    T delta = -ad * std::min(ilg,1.);
    vmml::Vector3< T > p = x + dir*delta;
    d += delta;
    for (int i=0 ; i<5 ; ++i)
    {
        grad = get_u13() + p * 2.* _u4;
        ilg = 1./grad.length();
        delta = -( _u0 + get_u13().dot(p) + _u4 * p.lengthSquared())*std::min(ilg,1.);
        p += dir*delta;
        d += delta;
    }
    return -d;

}



template< typename T >
inline vmml::Vector3< T > 
algebraic_sphere< T >::project_iteratively( const vec3& x ) const
{
    vmml::Vector3< T > grad;
    vmml::Vector3< T > dir = get_u13() + x * 2.0 * _u4;
    T ilg = 1./dir.length();
    dir *= ilg;
    T ad = _u0 + get_u13().dot(x) + _u4 * x.lengthSquared();
    T delta = -ad * std::min( ilg, 1.0 );
    vmml::Vector3< T > p = x + dir * delta;
    for (int i=0 ; i<5 ; ++i)
    {
        grad = get_u13() + p * 2.0 * _u4;
        ilg = 1./grad.length();
        delta = -( _u0 + get_u13().dot(p) 
            + _u4 * p.lengthSquared()) * std::min( ilg, 1.0 );
        p += dir * delta;
    }
    return p;
}


} // namespace stream_process

#endif

