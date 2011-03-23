#ifndef __STREAM_PROCESS__COVARIANCE_STREAM_OP__HPP__
#define __STREAM_PROCESS__COVARIANCE_STREAM_OP__HPP__

#include <stream_process/stream_process_types.hpp>
#include <stream_process/attribute_accessor.hpp>
#include <stream_process/neighbors.hpp>
#include <stream_process/compute_covariance.hpp>

namespace stream_process
{


template< typename operator_base_t >
class covariance_stream_op : public operator_base_t
{
public:
	typedef operator_base_t                         super;
    typedef typename operator_base_t::sp_types_t    sp_types_t;
    
    STREAM_PROCESS_TYPES
    
	covariance_stream_op();
	covariance_stream_op( const covariance_stream_op& original );
	
	virtual void push( slice_type* slice );

	virtual void setup_attributes();
	virtual void setup_accessors();

    virtual operator_base_t* clone();

    virtual bool needs_bounds_checking() const  { return true; }
    virtual bool is_multi_threadable() const    { return true; }

protected:	
	attribute_accessor< vec3 >			_get_position;
	attribute_accessor< mat4hp >		_get_covariance;
	attribute_accessor< nbh_type >      _get_neighbors;
	
	compute_covariance< sp_types_t >	_compute_covariance;
	
}; // class covariance_stream_op


template< typename operator_base_t >
covariance_stream_op< operator_base_t >::
covariance_stream_op()
	: super()
    , _get_position()
    , _get_covariance()
    , _get_neighbors()
	, _compute_covariance( _get_position, _get_neighbors, _get_covariance )
{}



template< typename operator_base_t >
covariance_stream_op< operator_base_t >::
covariance_stream_op( const covariance_stream_op& original )
	: super( original )
    , _get_position(    original._get_position )
    , _get_covariance(  original._get_covariance )
    , _get_neighbors(   original._get_neighbors )
	, _compute_covariance( _get_position, _get_neighbors, _get_covariance )
{}


template< typename operator_base_t >
void
covariance_stream_op< operator_base_t >::
push( slice_type* slice )
{
    assert( slice );
	typename slice_type::iterator it = slice->begin(), it_end = slice->end();
	for( ; it != it_end; ++it )
	{
		_compute_covariance( *it );
	}
    super::_out_buffer.push_back( slice );
}



template< typename operator_base_t >
void
covariance_stream_op< operator_base_t >::
setup_attributes()
{
	point_structure& ps = super::_stream_config->get_vertex_structure();
	super::read( ps, "position", _get_position );
	super::read( ps, "neighbors", _get_neighbors );

	super::write( ps, "covariance", _get_covariance, false );
}


template< typename operator_base_t >
void
covariance_stream_op< operator_base_t >::
setup_accessors()
{
	_get_position.setup();
	_get_neighbors.setup();
	_get_covariance.setup();
}



template< typename operator_base_t >
typename covariance_stream_op< operator_base_t >::super*
covariance_stream_op< operator_base_t >::
clone()
{
    return new covariance_stream_op( *this );
}


} // namespace stream_process

#endif

