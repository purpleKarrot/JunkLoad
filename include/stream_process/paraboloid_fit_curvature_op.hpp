#ifndef __STREAM_PROCESS__PARABOLOID_FIT_CURVATURE_OP__HPP__
#define __STREAM_PROCESS__PARABOLOID_FIT_CURVATURE_OP__HPP__

#include <stream_process/stream_process_types.hpp>

#include <stream_process/fit_to_paraboloid.hpp>
#include <stream_process/compute_paraboloid.hpp>

namespace stream_process
{

template< typename operator_base_t >
class paraboloid_fit_curvature_op : public operator_base_t
{
public:
	typedef operator_base_t                         super;
    typedef typename operator_base_t::sp_types_t    sp_types_t;
    
    STREAM_PROCESS_TYPES
    
    paraboloid_fit_curvature_op();
    //paraboloid_fit_curvature_op( const paraboloid_fit_curvature_op& orig_ );
    
	virtual void push( slice_type* slice );

	virtual void setup_negotiate();
	virtual void setup_attributes();
	virtual void setup_accessors();
	virtual void setup_finalize();
    
    virtual void add_config_options();

    virtual operator_base_t* clone() { return new paraboloid_fit_curvature_op( *this ); }

    virtual bool needs_bounds_checking() const  { return true; }
    virtual bool is_multi_threadable() const    { return false; }

protected:
    virtual void _push_fit( slice_type* slice_ );
    virtual void _push_calc( slice_type* slice_ );
    
    attribute_accessor< vec3 >              _get_position;
    //attribute_accessor< vec3 >            _get_normal;
    attribute_accessor< nbh_type >          _get_neighbors;
    attribute_accessor< vec3 >              _get_e1;
    attribute_accessor< vec3 >              _get_e2;
    attribute_accessor< sp_float_type >     _get_k1;
    attribute_accessor< sp_float_type >     _get_k2;
    
    attribute_accessor< mat3hp >            _get_curvature; // U
    attribute_accessor< vec3 >              _get_scales; // sigma
    
    fit_to_paraboloid< 10, hp_float_type >  _fit_to_paraboloid;
    compute_paraboloid< hp_float_type >     _compute_pb;
    
}; // class paraboloid_fit_curvature_op


#define SP_CLASS_NAME      paraboloid_fit_curvature_op< operator_base_t >
#define SP_TEMPLATE_STRING template< typename operator_base_t >

SP_TEMPLATE_STRING
SP_CLASS_NAME::
paraboloid_fit_curvature_op()
    : super()
{}


#if 0
SP_TEMPLATE_STRING
SP_CLASS_NAME::
paraboloid_fit_curvature_op( const paraboloid_fit_curvature_op& orig_ )
    : super( orig_ )
    
{}
#endif


SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
push( slice_type* slice )
{
    #if 0
    _push_fit( slice );
    #else
    _push_calc( slice );
    #endif
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
_push_calc( slice_type* slice )
{
    hp_float_type k1_, k2_;
    vec3hp  t_;

    typename slice_type::iterator 
        it      = slice->begin(),
        it_end  = slice->end();
    for( ; it != it_end; ++it )
    {
        stream_data* vertex = *it;
        
        const nbh_type& nbh = _get_neighbors( vertex );
        const vec3& p       = _get_position( vertex );
        
        size_t nbs  = 9;
        typename nbh_type::const_iterator
            nb_it       = nbh.begin(),
            nb_it_end   = nbh.end();
        for( size_t index = 0; index < nbs; ++index, ++nb_it )
        {
            assert( nb_it != nb_it_end );

            _compute_pb.set_position( index,
                p - _get_position( nb_it->get_ptr() ) );
        }
        
        //std::cout << _fit_to_paraboloid.get_A0() << std::endl;
        
        _compute_pb.solve();
    
    }
    super::_out_buffer.push_back( slice );
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
_push_fit( slice_type* slice )
{
    hp_float_type k1_, k2_;
    vec3hp  t_;

    typename slice_type::iterator 
        it      = slice->begin(),
        it_end  = slice->end();
    for( ; it != it_end; ++it )
    {
        stream_data* vertex = *it;
        
        const nbh_type& nbh = _get_neighbors( vertex );
        const vec3& p       = _get_position( vertex );
        
        _fit_to_paraboloid.set_position( 0, p );
        
        size_t nbs  = 10;
        typename nbh_type::const_iterator
            nb_it       = nbh.begin(),
            nb_it_end   = nbh.begin() + nbs;
        for( size_t index = 1; index < nbs; ++index, ++nb_it )
        {
            assert( nb_it != nb_it_end );

            _fit_to_paraboloid.set_position( index,
                //p - 
                _get_position( nb_it->get_ptr() ) );
        }
        
        //std::cout << _fit_to_paraboloid.get_A0() << std::endl;
        
        _fit_to_paraboloid.fit( k1_, k2_, t_ );
        
        sp_float_type&  k1  = _get_k1( vertex );
        sp_float_type&  k2  = _get_k2( vertex );
        
        k1  = static_cast< sp_float_type >( k1_ );
        k2  = static_cast< sp_float_type >( k2_ );
        
        const mat3hp& U = _fit_to_paraboloid.get_U();
        
        vec3& e1  = _get_e1( vertex );
        vec3& e2  = _get_e2( vertex );
        
        e1.x()  = static_cast< sp_float_type >( U( 0, 2 ) );
        e1.y()  = static_cast< sp_float_type >( U( 1, 2 ) );
        e1.z()  = static_cast< sp_float_type >( U( 2, 2 ) );

        e2.x()  = static_cast< sp_float_type >( U( 0, 1 ) );
        e2.y()  = static_cast< sp_float_type >( U( 1, 1 ) );
        e2.z()  = static_cast< sp_float_type >( U( 2, 1 ) );
        
        const vec3hp& sigma = _fit_to_paraboloid.get_sigma();
        
        vec3& scales    = _get_scales( vertex );
        scales( 0 )     = sigma( 0 );
        scales( 1 )     = sigma( 1 );
        scales( 2 )     = sigma( 2 );
        
        if ( scales.x() < scales.y() || scales.y() < scales.z() )
            throw exception( "eigenvalues not ordered.", SPROCESS_HERE );
        
        mat3hp& curvature   = _get_curvature( vertex );
        curvature           = U;
        
        #if 0
        
        std::cout << "vertex position " << p << std::endl;
        std::cout << "fit translation " << t_ << std::endl;
        std::cout << "diff " << t_ - p << std::endl;
        std::cout << "fit type " << _fit_to_paraboloid.determine_type() << std::endl;
        
        #endif
    
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
    //op.read( vs, "normal",      _get_normal );
    op.read( vs, "neighbors",   _get_neighbors );
    //op.read( vs, "point_index", _get_point_index );

    //op.read( vs, "radius",      _get_radius );

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

