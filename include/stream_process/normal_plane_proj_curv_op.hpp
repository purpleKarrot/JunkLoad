#ifndef __STREAM_PROCESS__NORMAL_PLANE_PROJ_CURV_OP__HPP__
#define __STREAM_PROCESS__NORMAL_PLANE_PROJ_CURV_OP__HPP__

#include <stream_process/stream_process_types.hpp>

#include <stream_process/fit_to_paraboloid.hpp>
#include <stream_process/compute_paraboloid.hpp>

namespace stream_process
{

template< typename operator_base_t >
class normal_plane_proj_curv_op : public operator_base_t
{
public:
	typedef operator_base_t                         super;
    typedef typename operator_base_t::sp_types_t    sp_types_t;
    
    STREAM_PROCESS_TYPES
    
    normal_plane_proj_curv_op();
    //paraboloid_fit_curvature_op( const paraboloid_fit_curvature_op& orig_ );
    
	virtual void push( slice_type* slice );

	virtual void setup_negotiate();
	virtual void setup_attributes();
	virtual void setup_accessors();
	virtual void setup_finalize();
    
    virtual void add_config_options();

    virtual operator_base_t* clone() { return new normal_plane_proj_curv_op( *this ); }

    virtual bool needs_bounds_checking() const  { return true; }
    virtual bool is_multi_threadable() const    { return false; }

protected:
    attribute_accessor< vec3 >              _get_position;
    attribute_accessor< vec3 >              _get_normal;
    attribute_accessor< nbh_type >          _get_neighbors;
    attribute_accessor< vec3 >              _get_e1;
    attribute_accessor< vec3 >              _get_e2;
    attribute_accessor< sp_float_type >     _get_k1;
    attribute_accessor< sp_float_type >     _get_k2;

    attribute_accessor< vec3 >              _get_axis;
    
    attribute_accessor< mat3hp >            _get_curvature; // U
    attribute_accessor< vec3 >              _get_scales; // sigma
    
    fit_to_paraboloid< 10, hp_float_type >  _fit_to_paraboloid;
    compute_paraboloid< hp_float_type >     _compute_pb;
    
}; // class paraboloid_fit_curvature_op


#define SP_CLASS_NAME      normal_plane_proj_curv_op< operator_base_t >
#define SP_TEMPLATE_STRING template< typename operator_base_t >

SP_TEMPLATE_STRING
SP_CLASS_NAME::
normal_plane_proj_curv_op()
    : super()
{}


#if 0
SP_TEMPLATE_STRING
SP_CLASS_NAME::
normal_plane_proj_curv_op( const paraboloid_fit_curvature_op& orig_ )
    : super( orig_ )
    
{}
#endif



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
        stream_data* vertex = *it;
        
        const nbh_type& nbh = _get_neighbors( vertex );
        const vec3& p       = _get_position( vertex );
        const vec3& n       = _get_normal( vertex );
        const vec3& e2      = _get_axis( vertex );
        vec3 e1;
        e1.cross( e2, n );
        
        typename nbh_type::const_iterator
            nb_it       = nbh.begin(),
            nb_it_end   = nbh.end();
        for( ; nb_it != nb_it_end; ++nb_it )
        {
            
        }


    }
    super::_out_buffer.push_back( slice );
}




SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
setup_attributes()
{
    stream_config& cfg              = *super::_stream_config;
    stream_structure& vs            = cfg.get_vertex_structure();
    stream_structure& fs            = cfg.get_face_structure();

    operator_base_t& op = *this;

    op.read( vs, "position",    _get_position );
    op.read( vs, "normal",      _get_normal );
    op.read( vs, "neighbors",   _get_neighbors );
    //op.read( vs, "point_index", _get_point_index );

    op.read( vs, "axis",    _get_axis );

    op.write( vs, "k1",         _get_k1 );
    op.write( vs, "e1",         _get_e1 );
    op.write( vs, "k2",         _get_k2 );
    op.write( vs, "e2",         _get_e2 );

    op.write( vs, "curvature",      _get_curvature );
    op.write( vs, "scales",         _get_scales );

#if 0
    op.write( vs, "axis",           _get_axis, true );
    op.write( vs, "axis_ratio",		_get_axis_ratio, true );
    op.write( vs, "axis_length",	_get_axis_length, true );
#endif
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
setup_accessors()
{
	stream_config& cfg			= *super::_stream_config;
    stream_options& opts        = cfg.get_options();

    _get_position.setup();
    _get_neighbors.setup();
    _get_e1.setup();
    _get_e2.setup();
    _get_k1.setup();
    _get_k2.setup();
    
    _get_axis.setup();
    
    _get_curvature.setup();
    _get_scales.setup();

}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::setup_negotiate()
{
	stream_config& cfg			= *super::_stream_config;
    stream_options& opts        = cfg.get_options();

    if ( opts.get( "number-of-neighbors").get_int() < 10 )
        opts.get( "number-of-neighbors").set_int( 10 );
        
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
setup_finalize()
{
	stream_config& cfg			= *super::_stream_config;
    stream_options& opts        = cfg.get_options();
    
    
    #if 0
    _compute_curvature.set_use_stream_normal(
        opts.get( "curv-use-stream-normal" ).get_bool()
        );
    #endif
    //size_t k = opts.get( "number-of-neighbors" ).get_int();
    //_compute_curvature.set_k( k );
}


SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
add_config_options()
{
	//stream_options& opts	= super::_stream_config->get_options();

#if 0
    option k;
    k.setup_bool( "curv-use-stream-normal", true );
    k.set_help_text( "use stream normal or compute own normal in accurate curvature op" );
    opts.add_option( k, "accurate curvature" );
#endif
}



#undef SP_CLASS_NAME
#undef SP_TEMPLATE_STRING

} // namespace stream_process

#endif

