#ifndef __STREAM_PROCESS__ESTIMATE_SPLAT_STREAM_OP__HPP__
#define __STREAM_PROCESS__ESTIMATE_SPLAT_STREAM_OP__HPP__

#include <stream_process/perturb_vector.hpp>

namespace stream_process
{


template< typename operator_base_t >
class estimate_splat_stream_op : public operator_base_t
{
public:
	typedef operator_base_t					super;
	typedef typename super::sp_types_t		sp_types_t;

    STREAM_PROCESS_TYPES

	typedef typename vmml::vector< 3, size_t >	vec3s;

	estimate_splat_stream_op();
	
	virtual void push( slice_type* slice );
	
	virtual void setup_attributes();
	virtual void setup_accessors();

    virtual operator_base_t* clone();

    virtual bool needs_bounds_checking() const  { return true; }
    virtual bool is_multi_threadable() const    { return false; }

protected:
	virtual void _compute_splat( stream_data* point );
    virtual bool _test_results( stream_data* point ) const;

	attribute_accessor< vec3 >			_get_position;
	attribute_accessor< nbh_type >      _get_neighbors;
	attribute_accessor< mat3hp >		_get_curvature;
	attribute_accessor< vec3 >			_get_scales;

	attribute_accessor< vec3 >			_get_axis;
	attribute_accessor< sp_float_type >	_get_axis_length;
	attribute_accessor< sp_float_type >	_get_axis_ratio;

	sp_float_type	_min_axis_ratio;
	size_t          _max_neighbors;
	
    vec3            _diff;
    vec3            _curv1;
    vec3            _curv2;

}; // class estimate_splat_stream_op


template< typename operator_base_t >
estimate_splat_stream_op< operator_base_t >::
estimate_splat_stream_op()
	: super()
    , _get_position()
    , _get_neighbors()
    , _get_curvature()
    , _get_scales()
    , _get_axis()
    , _get_axis_length()
    , _get_axis_ratio()
	, _min_axis_ratio( 0.5 )
	, _max_neighbors( 8 )
{}



template< typename operator_base_t >
void
estimate_splat_stream_op< operator_base_t >::
push( slice_type* slice )
{
	typename slice_type::iterator it = slice->begin(), it_end = slice->end();
	for( ; it != it_end; ++it )
	{
        stream_data* point = *it;

        _compute_splat( point );
              
		const vec3& position        = _get_position( point );
		const vec3& scales          = _get_scales( point );
		const mat3hp& curv          = _get_curvature( point );
		
		vec3& axis                  = _get_axis( point );
		sp_float_type& axis_length	= _get_axis_length( point );
		sp_float_type& axis_ratio	= _get_axis_ratio( point );

        assert( axis_length != 0 );
        
        if ( axis.is_unit_vector() )
        {
            axis.perturb();
        }
        
        assert( _test_results( point ) );
        
	}
    
    super::_out_buffer.push_back( slice );
    
}


template< typename operator_base_t >
void
estimate_splat_stream_op< operator_base_t >::
_compute_splat( stream_data* point )
{
		// use principal curvature directions as ellipse axis
		
		const vec3& position        = _get_position( point );
		const vec3& scales          = _get_scales( point );
		const mat3hp& curv          = _get_curvature( point );
		
		vec3& axis                  = _get_axis( point );
		sp_float_type& axis_length	= _get_axis_length( point );
		sp_float_type& axis_ratio	= _get_axis_ratio( point );
		
		nbh_type& nbh				= _get_neighbors( point );
		
		axis_length                 = scales[1];

        if ( axis_length == static_cast< sp_float_type >( 0 ) )
        {
            throw exception( "cannot compute splat with zero principal curvature vector.", 
                SPROCESS_HERE  );
        }
        

		axis_ratio                  = scales[2] / scales[1];

		// clamp ratio 
		if ( axis_ratio < _min_axis_ratio )
			axis_ratio = _min_axis_ratio;
			
		// adjust elliptical splat extent
		const sp_float_type a   = axis_length * axis_length;
		const sp_float_type b   = a * axis_ratio * axis_ratio;
		
		sp_float_type max = 0.0;
	
		size_t max_nb = nbh.size();
		if ( max_nb > _max_neighbors )
			max_nb = _max_neighbors;
		
        _curv1.set( curv( 0,1 ), curv( 1,1 ), curv( 2,1 ) );
        _curv2.set( curv( 0,2 ), curv( 1,2 ), curv( 2,2 ) );

		typename nbh_type::const_iterator it = nbh.begin(), it_end = nbh.end();
		for( size_t index = 0; it != it_end && index < max_nb; ++it, ++index )
		{
			// project neighbor into tangential ellipse plane
			// note that larger curvature curv[1] is shorter ellipse axis
			const vec3& nb_pos = _get_position( (*it).get_ptr() );
		
			_diff = nb_pos - position;
			
			sp_float_type x = dot( _diff, _curv2 );
			sp_float_type y = dot( _diff, _curv1 );

			// test point in ellipse equation
			sp_float_type f = x * x / a + y * y / b;

			// get largest value of f
			if ( f > max )
				max = f;
		}

		// scale major ellipse axis length
		max = sqrt( max );

		axis_length *= max;
		
		// note that larger curvature curv[1] is shorter ellipse axis
		axis.set( curv( 0,2 ), curv( 1,2 ), curv( 2,2 ) );
        
}



template< typename operator_base_t >
bool
estimate_splat_stream_op< operator_base_t >::
_test_results( stream_data* point ) const
{
#if 1
    bool ok = true;
    if ( contains_nan_or_inf( _get_axis( point ), true ) )
    {
        std::cout << "axis invalid." << std::endl;
        ok = false;
    }

    if ( is_nan_or_inf( _get_axis_length( point ), true ) )
    {
        std::cout << "axis length invalid." << std::endl;
        ok = false;
    }

    if ( is_nan_or_inf( _get_axis_ratio( point ), true ) )
    {
        std::cout << "axis ratio invalid." << std::endl;
        ok = false;
    }
    
    return ok;
    
#else

    const vec3& axis                    = _get_axis( point );
    const sp_float_type& axis_length    = _get_axis_length( point );
    const sp_float_type& axis_ratio     = _get_axis_ratio( point );

    for( size_t index = 0; index < 3; ++index )
    {
        const sp_float_type& f = axis[ index ];
        if ( std::isnan( f ) || std::isinf( f ) )
            return false;
    }

    if ( std::isnan( axis_length ) || std::isinf( axis_length ) )
        return false;
    if ( std::isnan( axis_ratio ) || std::isinf( axis_ratio ) )
        return false;

    return true;
#endif
}




template< typename operator_base_t >
void
estimate_splat_stream_op< operator_base_t >::
setup_attributes()
{
	point_structure& ps = super::_stream_config->get_vertex_structure();
	super::read( ps, "position",	_get_position );
	super::read( ps, "neighbors",	_get_neighbors );
	super::read( ps, "curvature",	_get_curvature );
	super::read( ps, "scales",		_get_scales );
	
	super::write( ps, "axis",			_get_axis, true );
	super::write( ps, "axis_ratio",		_get_axis_ratio, true );
	super::write( ps, "axis_length",	_get_axis_length, true );
	
}


template< typename operator_base_t >
void
estimate_splat_stream_op< operator_base_t >::
setup_accessors()
{
	_get_position.setup();
	_get_neighbors.setup();
	_get_curvature.setup();
	_get_scales.setup();
	
	_get_axis.setup();
	_get_axis_length.setup();
	_get_axis_ratio.setup();
}



template< typename operator_base_t >
typename estimate_splat_stream_op< operator_base_t >::super*
estimate_splat_stream_op< operator_base_t >::
clone()
{
    return new estimate_splat_stream_op( *this );
}



} // namespace stream_process

#endif

