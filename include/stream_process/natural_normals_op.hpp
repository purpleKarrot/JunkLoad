#ifndef __STREAM_PROCESS__NATURAL_NORMALS_OP__HPP__
#define __STREAM_PROCESS__NATURAL_NORMALS_OP__HPP__

#include <stream_process/stream_process_types.hpp>
#include <stream_process/get_jacobian_matrix.hpp>

#include <vmmlib/jacobi_solver.hpp>

#include <stream_process/normal_test.hpp>

namespace stream_process
{

// implementation of the normal computation scheme proposed in
// alexa, adamson 2007: Interpolatory Point Set Surfaces – Convexity and Hermite Data

template< typename operator_base_t >
class natural_normals_op : public operator_base_t
{
public:
	typedef operator_base_t                     super;
	typedef typename super::sp_types_t          sp_types_t;

    STREAM_PROCESS_TYPES
    
    typedef get_jacobian_matrix< sp_types_t >   get_jacobian_matrix_type;
    
    natural_normals_op();
    natural_normals_op( const natural_normals_op& original );
	
	virtual void push( slice_type* slice );
	
	virtual void setup_attributes();
	virtual void setup_accessors();
    virtual void setup_finalize();
    
    virtual operator_base_t* clone();
    
    virtual bool needs_bounds_checking() const  { return true; }
    virtual bool is_multi_threadable() const    { return true; }

    virtual void add_config_options();

protected:
	attribute_accessor< vec3 >			_get_position;
	attribute_accessor< nbh_type >      _get_neighbors;
	attribute_accessor< sp_float_type > _get_radius;
	attribute_accessor< vec3 >			_get_normal;
    
    mat3    _a;
    mat3    _v;
    vec3    _d;

    get_jacobian_matrix_type    _get_jacobian_matrix;
    
    normal_test< vec3 >                 _normal_test;
    std::string                         _normal_sanity;
	
}; // class natural_normals_op

#define SP_CLASS_NAME      natural_normals_op< operator_base_t >
#define SP_TEMPLATE_STRING template< typename operator_base_t >


SP_TEMPLATE_STRING
SP_CLASS_NAME::
natural_normals_op()
	: super()
    , _get_jacobian_matrix( _get_position, _get_neighbors, _get_radius )
{
}


SP_TEMPLATE_STRING
SP_CLASS_NAME::
natural_normals_op( const natural_normals_op& original )
	: super( original )
    , _get_position( original._get_position )
    , _get_neighbors( original._get_neighbors )
    , _get_normal( original._get_normal )
    , _get_radius( original._get_radius )
    , _get_jacobian_matrix( _get_position, _get_neighbors, _get_radius,
        original._get_jacobian_matrix )
{}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
push( slice_type* slice )
{
    typename slice_type::iterator 
        it      = slice->begin(),
        it_end  = slice->end();
    for( ; it != it_end; ++it )
    {
        _get_jacobian_matrix( _a, *it );
        
        size_t rotations;
        vmml::solve_jacobi_3x3( _a, _d, _v, rotations );

        vec3& normal = _get_normal( *it );
        
        size_t smallest_index = _d.find_abs_min_index();
        _v.get_row( smallest_index, normal );
        
        if ( _normal_test( normal, _normal_sanity ) == false )
        {
            std::string msg = "natural normal op computed invalid normal: ";
            msg += _normal_sanity;
            throw exception( msg, SPROCESS_HERE );
        }
    }
    
    super::_out_buffer.push_back( slice );

}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
add_config_options()
{
	stream_options& opts	= super::_stream_config->get_options();

    option k;
    k.setup_string( "nn-algo", "central_differences" );
    k.set_help_text( "algorithm used for filling the jacobian matrix: 'central_differences' (default) or 'forward_differences'" );
    opts.add_option( k, "natural normals" );
    
    k.setup_string( "nn-weight-function", "local_support" );
    k.set_help_text( "weight function: 'local_support' (default), 'pow_neg_m' or 'wendlands'" );
    opts.add_option( k, "natural normals" );
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
setup_finalize()
{
	stream_config& cfg			= *super::_stream_config;
    stream_options& opts        = cfg.get_options();
    
    const std::string method = opts.get( "nn-algo" ).get_string();
    const std::string weight = opts.get( "nn-weight-function" ).get_string();

    _get_jacobian_matrix.set_algorithm( method );
    _get_jacobian_matrix.set_weight_function( weight );
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
setup_attributes()
{
	point_structure& ps = super::_stream_config->get_vertex_structure();
	super::read( ps, "position",	_get_position );
	super::read( ps, "neighbors",	_get_neighbors );
	super::read( ps, "radius",      _get_radius );

	super::write( ps, "normal", _get_normal, true );

}


SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
setup_accessors()
{
	_get_position.setup();
	_get_neighbors.setup();
    _get_radius.setup();
	_get_normal.setup();
}


SP_TEMPLATE_STRING
operator_base_t*
SP_CLASS_NAME::
clone()
{
    return new SP_CLASS_NAME( *this );
}


#undef SP_CLASS_NAME
#undef SP_TEMPLATE_STRING

} // namespace stream_process

#endif

