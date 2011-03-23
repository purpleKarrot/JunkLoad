#ifndef __STREAM_PROCESS__ESTIMATE_CURVATURE_STREAM_OP__HPP__
#define __STREAM_PROCESS__ESTIMATE_CURVATURE_STREAM_OP__HPP__

#include <stream_process/svd.hpp>
#include <stream_process/compute_normal_space_covariance.hpp>

#include <stream_process/nan_inf_test.hpp>

namespace stream_process
{

template< typename operator_base_t >
class estimate_curvature_stream_op : public operator_base_t
{
public:
	typedef operator_base_t					super;
	typedef typename super::sp_types_t		sp_types_t;

    STREAM_PROCESS_TYPES

	typedef typename vmml::vector< 3, size_t >	vec3s;

	estimate_curvature_stream_op();
	estimate_curvature_stream_op( const estimate_curvature_stream_op& original );
    virtual operator_base_t* clone();
	
	virtual void push( slice_type* slice );
	
	virtual void setup_attributes();
	virtual void setup_accessors();


    virtual bool needs_bounds_checking() const  { return true; }
    virtual bool is_multi_threadable() const    { return false; }

protected:
    virtual bool _test_results( stream_data* data_ );
    
	attribute_accessor< vec3 >			_get_position;
	attribute_accessor< nbh_type >      _get_neighbors;
	attribute_accessor< vec3 >			_get_normal;
	attribute_accessor< mat3hp >		_get_curvature;
	attribute_accessor< vec3 >			_get_scales;
    
    attribute_accessor< size_t >        _get_index;
	
	vec3hp                  _row;
	svd< hp_float_type >    _svd;

	compute_normal_space_covariance< sp_types_t >	_compute_ns_covar;
	
}; // class estimate_curvature_stream_op


template< typename operator_base_t >
estimate_curvature_stream_op< operator_base_t >::
estimate_curvature_stream_op()
	: super()
    , _get_position()
    , _get_neighbors()
    , _get_normal()
    , _get_curvature()
    , _get_scales()
	, _compute_ns_covar( _get_position, _get_neighbors, _get_normal )
{}



template< typename operator_base_t >
estimate_curvature_stream_op< operator_base_t >::
estimate_curvature_stream_op( const estimate_curvature_stream_op& original )
	: super( original )
    , _get_position(    original._get_position )
    , _get_neighbors(   original._get_neighbors )
    , _get_normal(      original._get_normal )
    , _get_curvature(   original._get_curvature )
    , _get_scales(      original._get_scales )
    , _get_index(       original._get_index )
	, _compute_ns_covar( _get_position, _get_neighbors, _get_normal )
{}


template< typename operator_base_t >
void
estimate_curvature_stream_op< operator_base_t >::
push( slice_type* slice )
{
    assert( slice );
	typename slice_type::iterator it = slice->begin(), it_end = slice->end();
	for( ; it != it_end; ++it )
	{
		stream_data* point = *it;

		const mat4hp& ns_covar = _compute_ns_covar( point );
        
        contains_nan_or_inf( ns_covar, true );

		_svd.compute_3x3( ns_covar );
        
		vec3& scales        = _get_scales( point );

        contains_nan_or_inf( scales, true );

		const mat3hp& u     = _svd.get_u();
		const vec3hp& sigma = _svd.get_sigma();
		const vec3s& order  = _svd.get_order();
		
        // DEBUG
        contains_nan_or_inf( u, true );
        contains_nan_or_inf( sigma, true );
        contains_nan_or_inf( order, true );
        //

		const size_t& l1    = order.x();
		const size_t& l2    = order.y();
		const size_t& l3    = order.z();

		// copy ordered singular values into scales vector
		scales.set( sigma[ l3 ], sigma[ l2 ], sigma[ l1 ] );

        
        if ( scales.y() == 0.0 || scales.z() == 0.0 )
        {
            std::cout << ns_covar << std::endl;
            std::cout << scales << std::endl;
            throw exception( "0.0 in scales.", SPROCESS_HERE );
        }
        
		mat3hp& curvature = _get_curvature( point );

        //curvature.zero();
    
        #if 1
        curvature( 0, 0 )   = u( 0, l3 );
        curvature( 0, 1 )   = u( 0, l2 );
        curvature( 0, 2 )   = u( 0, l1 );

        curvature( 1, 0 )   = u( 1, l3 );
        curvature( 1, 1 )   = u( 1, l2 );
        curvature( 1, 2 )   = u( 1, l1 );

        curvature( 2, 0 )   = u( 2, l3 );
        curvature( 2, 1 )   = u( 2, l2 );
        curvature( 2, 2 )   = u( 2, l1 );
        
        #else
		for( size_t index = 0; index < 3; ++index )
		{
			_row.set( u( index, l3 ), u( index, l2 ), u( index, l1 ) );
			_row.normalize();
			
			for( size_t j = 0; j < 3; ++j )
			{
				curvature( index, j ) = _row( j ); 
			}
		}
        #endif

        contains_nan_or_inf( u, true );

        size_t point_index = _get_index( point );
        assert( _test_results( point ) );
	}
    super::_out_buffer.push_back( slice );
    
}



template< typename operator_base_t >
bool
estimate_curvature_stream_op< operator_base_t >::
_test_results( stream_data* point )
{
#if 1
    bool ok = true;
    if ( contains_nan_or_inf( _get_scales( point ), true ) )
    {
        std::cout << "scales invalid." << std::endl;
        ok = false;
    }

    if ( contains_nan_or_inf( _get_curvature( point ), true ) )
    {
        std::cout << "curv invalid." << std::endl;
        ok = false;
    }
    
    return ok;
    
#else
    vec3& scales        = _get_scales( point );
    
    for( size_t index = 0; index < 3; ++index )
    {
        const sp_float_type& f = scales[ index ];
        if ( std::isnan( f ) || std::isinf( f ) )
            return false;
        if ( f == 0.0 )
            return false;
    }
    
    mat3hp& curvature   = _get_curvature( point );
    
    for( size_t index = 0; index < 9; ++index )
    {
        const hp_float_type& f = curvature.array[ index ];
        if ( std::isnan( f ) || std::isinf( f ) )
            return false;
    }
    return true;
#endif
}



template< typename operator_base_t >
void
estimate_curvature_stream_op< operator_base_t >::
setup_attributes()
{
	point_structure& ps = super::_stream_config->get_vertex_structure();
	super::read( ps, "position",	_get_position );
	super::read( ps, "neighbors",	_get_neighbors );
	super::read( ps, "normal",		_get_normal );

	super::write( ps, "curvature",	_get_curvature, true );
	super::write( ps, "scales",		_get_scales, true );

    super::read( ps, "point_index", _get_index );

}


template< typename operator_base_t >
void
estimate_curvature_stream_op< operator_base_t >::
setup_accessors()
{
	_get_position.setup();
	_get_neighbors.setup();
	_get_normal.setup();

	_get_curvature.setup();
	_get_scales.setup();
    
    _get_index.setup();
}



template< typename operator_base_t >
typename estimate_curvature_stream_op< operator_base_t >::super*
estimate_curvature_stream_op< operator_base_t >::
clone()
{
    return new estimate_curvature_stream_op( *this );
}


} // namespace stream_process

#endif

