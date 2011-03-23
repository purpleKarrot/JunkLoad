#ifndef __STREAM_PROCESS__RADIUS_STREAM_OP__HPP__
#define __STREAM_PROCESS__RADIUS_STREAM_OP__HPP__

#include <stream_process/compute_radius.hpp>

#include <boost/bind.hpp>

namespace stream_process
{

template< typename operator_base_t >
class radius_stream_op : public operator_base_t
{
public:
	typedef operator_base_t					super;
	typedef typename super::sp_types_t		sp_types_t;

    STREAM_PROCESS_TYPES
    
    typedef compute_radius< sp_types_t >    compute_radius_type;
	
    radius_stream_op();
    radius_stream_op( const radius_stream_op& original );
	
	virtual void push( slice_type* slice );
	
	virtual void setup_attributes();
	virtual void setup_accessors();

	virtual void setup_finalize();
	virtual void add_config_options();

    virtual operator_base_t* clone();

    virtual bool needs_bounds_checking() const  { return true; }
    virtual bool is_multi_threadable() const    { return true; }
    
    void set_radius_algorithm( const std::string& algo );

protected:
	attribute_accessor< vec3 >			_get_position;
	attribute_accessor< nbh_type >      _get_neighbors;
	attribute_accessor< sp_float_type >	_get_radius;
	
	compute_radius_type                 _compute_radius;
    
    boost::function< void ( stream_data* point )>   _compute_radius_func;
    std::string                                     _radius_algo;
    #if 0
            _fill_jacobian = boost::bind( 
                &get_jacobian_matrix::using_central_differences, 
                this, _1, _2 
                );
    #endif

}; // class radius_stream_op


#define SP_CLASS_NAME      radius_stream_op< operator_base_t >
#define SP_TEMPLATE_STRING template< typename operator_base_t >

SP_TEMPLATE_STRING
SP_CLASS_NAME::
radius_stream_op()
	: super()
    , _get_position()
    , _get_neighbors()
    , _get_radius()
	, _compute_radius( _get_position, _get_neighbors, _get_radius )
{
    set_radius_algorithm( "mls-support" );
}



SP_TEMPLATE_STRING
SP_CLASS_NAME::
radius_stream_op( const radius_stream_op& original )
	: super( original )
    , _get_position(    original._get_position )
    , _get_neighbors(   original._get_neighbors )
    , _get_radius(      original._get_radius )
	, _compute_radius( _get_position, _get_neighbors, _get_radius )
{
    set_radius_algorithm( original._radius_algo );
}


SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
set_radius_algorithm( const std::string& algo )
{
    if ( algo == "mls-support" )
    {
        _compute_radius_func = boost::bind( 
                &compute_radius_type::compute_mls_support_radius, 
                &_compute_radius, _1
                );
        _radius_algo = algo;
    }
    else if ( algo == "kth-nb-dist" )
    {
        _compute_radius_func = boost::bind( 
                &compute_radius_type::compute_radius_kth_nb, 
                &_compute_radius, _1
                );
        _radius_algo = algo;
    }
    else if ( algo == "debug-func" )
    {
        _compute_radius_func = boost::bind( 
                &compute_radius_type::debug_radius_func, 
                &_compute_radius, _1
                );
        _radius_algo = algo;
    }


}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
push( slice_type* slice )
{
    assert( slice );
	typename slice_type::iterator it = slice->begin(), it_end = slice->end();
	for( ; it != it_end; ++it )
	{
		//_compute_radius.compute_mls_support_radius( *it );
        _compute_radius_func( *it );
    }
    super::_out_buffer.push_back( slice );
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
setup_attributes()
{
	point_structure& ps = super::_stream_config->get_vertex_structure();
	super::read( ps, "position", _get_position );
	super::read( ps, "neighbors", _get_neighbors );

	super::write( ps, "radius", _get_radius, true );
}


SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
setup_accessors()
{
	_get_position.setup();
	_get_neighbors.setup();
	_get_radius.setup();
}


SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
add_config_options()
{
	stream_options& opts	= super::_stream_config->get_options();

    option k;
    k.setup_string( "radius-algo", "mls-support" );
    k.set_help_text( "algorithm to use for computing the radius: 'mls-support' (default) or 'kth-nb-dist'" );
    opts.add_option( k, "radius" );
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
setup_finalize()
{
	stream_config& cfg			= *super::_stream_config;
    stream_options& opts        = cfg.get_options();
    
    const std::string algo = opts.get( "radius-algo" ).get_string();
    
    set_radius_algorithm( algo );
    
    std::cout << "radius op: using '" 
        << _radius_algo << " algorithm." << std::endl;
    
}


SP_TEMPLATE_STRING
typename SP_CLASS_NAME::super*
SP_CLASS_NAME::
clone()
{
    return new SP_CLASS_NAME( *this );
}


#undef SP_CLASS_NAME
#undef SP_TEMPLATE_STRING


} // namespace stream_process

#endif

