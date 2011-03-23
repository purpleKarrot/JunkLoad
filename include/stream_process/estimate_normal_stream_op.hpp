#ifndef __STREAM_PROCESS__ESTIMATE_NORMAL_STREAM_OP__HPP__
#define __STREAM_PROCESS__ESTIMATE_NORMAL_STREAM_OP__HPP__

#include <stream_process/stream_data.hpp>
#include <stream_process/neighbors.hpp>
#include <stream_process/estimate_normal.hpp>
#include <stream_process/normal_test.hpp>

namespace stream_process
{

template< typename operator_base_t >
class estimate_normal_stream_op : public operator_base_t
{
public:
	typedef operator_base_t					super;
	typedef typename super::sp_types_t		sp_types_t;

    STREAM_PROCESS_TYPES
	
    estimate_normal_stream_op();
    estimate_normal_stream_op( const estimate_normal_stream_op& original );
	
	virtual void push( slice_type* slice );
	
	virtual void setup_attributes();
	virtual void setup_accessors();

    virtual operator_base_t* clone();
    
    virtual bool needs_bounds_checking() const  { return true; }
    virtual bool is_multi_threadable() const    { return true; }

protected:
	attribute_accessor< vec3 >			_get_position;
	attribute_accessor< nbh_type >      _get_neighbors;
	attribute_accessor< mat4hp >		_get_covariance;
	attribute_accessor< vec3 >			_get_normal;
	
	estimate_normal< sp_types_t >		_estimate_normal;
    
    normal_test< vec3 >                 _normal_test;
    std::string                         _normal_sanity;

}; // class estimate_normal_stream_op


template< typename operator_base_t >
estimate_normal_stream_op< operator_base_t >::
estimate_normal_stream_op()
	: super()
    , _get_position()
    , _get_neighbors()
    , _get_covariance()
    , _get_normal()
	, _estimate_normal( _get_position, _get_neighbors, _get_covariance, _get_normal )
{}



template< typename operator_base_t >
estimate_normal_stream_op< operator_base_t >::
estimate_normal_stream_op( const estimate_normal_stream_op& original )
	: super( original )
    , _get_position(    original._get_position )
    , _get_neighbors(   original._get_neighbors )
    , _get_covariance(  original._get_covariance )
    , _get_normal(      original._get_normal )
	, _estimate_normal( _get_position, _get_neighbors, _get_covariance, _get_normal )
{}



template< typename operator_base_t >
void
estimate_normal_stream_op< operator_base_t >::
push( slice_type* slice )
{
    assert( slice );
	typename slice_type::iterator it = slice->begin(), it_end = slice->end();
	for( ; it != it_end; ++it )
	{
		_estimate_normal( *it );
        
        
        vec3& normal = _get_normal( *it );

        if ( _normal_test( normal, _normal_sanity ) == false )
        {
            std::cout << "normal: " << normal << std::endl;
        
            std::string msg = "normal op computed invalid normal: ";
            msg += _normal_sanity;
            throw exception( msg, SPROCESS_HERE );
        }

	}
    super::_out_buffer.push_back( slice );
}



template< typename operator_base_t >
void
estimate_normal_stream_op< operator_base_t >::
setup_attributes()
{
	point_structure& ps = super::_stream_config->get_vertex_structure();
	super::read( ps, "position",	_get_position );
	super::read( ps, "neighbors",	_get_neighbors );
	super::read( ps, "covariance",	_get_covariance );

	super::write( ps, "normal",		_get_normal, true );
}


template< typename operator_base_t >
void
estimate_normal_stream_op< operator_base_t >::
setup_accessors()
{
	_get_position.setup();
	_get_neighbors.setup();
	_get_covariance.setup();
	_get_normal.setup();
}


template< typename operator_base_t >
typename estimate_normal_stream_op< operator_base_t >::super*
estimate_normal_stream_op< operator_base_t >::
clone()
{
    return new estimate_normal_stream_op( *this );
}


} // namespace stream_process

#endif

