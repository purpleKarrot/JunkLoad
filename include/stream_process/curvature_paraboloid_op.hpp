#ifndef __STREAM_PROCESS__CURVATURE_PARABOLOID_OP__HPP__
#define __STREAM_PROCESS__CURVATURE_PARABOLOID_OP__HPP__

#include <stream_process/stream_process_types.hpp>

#include <vmmlib/lapack_linear_least_squares.hpp>

namespace stream_process
{

template< typename operator_base_t >
class curvature_paraboloid_op : public operator_base_t
{
public:
	typedef operator_base_t                         super;
    typedef typename operator_base_t::sp_types_t    sp_types_t;
    
    STREAM_PROCESS_TYPES
    
    static const size_t kNumberOfNeighbors = 8;
    
    typedef vmml::matrix< kNumberOfNeighbors, 2, sp_float_type >    mat_k;
    typedef vmml::vector< kNumberOfNeighbors, sp_float_type >       vec_k;
    
    curvature_paraboloid_op();
    
	virtual void push( slice_type* slice );

	virtual void setup_negotiate();
	virtual void setup_attributes();
	virtual void setup_accessors();
	virtual void setup_finalize();
    
    virtual void add_config_options();

    virtual operator_base_t* clone() { return new curvature_paraboloid_op( *this ); }

    virtual bool needs_bounds_checking() const  { return true; }
    virtual bool is_multi_threadable() const    { return true; }

protected:
    attribute_accessor< vec3 >              _get_position;
    attribute_accessor< vec3 >              _get_normal;
    attribute_accessor< nbh_type >          _get_neighbors;
    attribute_accessor< vec3 >              _get_axis;
    
    attribute_accessor< vec2 >              _get_k1k2;
    
    mat_k   _A;
    vec_k   _B;
    vec2    _X;
    
    mat3    _transform;
    
}; // class paraboloid_fit_curvature_op


#define SP_CLASS_NAME      curvature_paraboloid_op< operator_base_t >
#define SP_TEMPLATE_STRING template< typename operator_base_t >

SP_TEMPLATE_STRING
SP_CLASS_NAME::
curvature_paraboloid_op()
    : super()
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
        stream_data* vertex = *it;
        
        const nbh_type& nbh = _get_neighbors( vertex );
        const vec3& p       = _get_position( vertex );
        const vec3& n       = _get_normal( vertex );
        const vec3& e2      = _get_axis( vertex );
        vec3 e1;
        e1.cross( e2, n );
        
        _transform.set_row( 0, e1 );
        _transform.set_row( 1, e2 );
        _transform.set_row( 2, n );
        
        vec3 nb_pos;
        typename nbh_type::const_iterator
            nb_it       = nbh.begin(),
            nb_it_end   = nbh.end();
        for( size_t index = 0; nb_it != nb_it_end; ++nb_it, ++index )
        {
            nb_pos = _transform * ( p - _get_position( nb_it->get_ptr() ) );
            
            _A( index, 0 )  = nb_pos.x() * nb_pos.x(); // x^2
            _A( index, 1 )  = nb_pos.y() * nb_pos.y(); // y^2
            
            _B( index )     = nb_pos.z();
        }
        
        // compute linear least squares
        vmml::lapack::linear_least_squares_xgels< 8, 2, sp_float_type > llsq;

        llsq.compute( _A, _B, _X );

        vec2& k1k2  = _get_k1k2( vertex );

        sp_float_type a = _X( 0 );
        sp_float_type b = _X( 1 );
        
        if ( std::isnan( a ) || std::isnan( b ) )
        {
            throw exception( "nan curvature parameters.", SPROCESS_HERE );
        }
        
        k1k2( 0 ) = 4.0 / ( a * a );
        k1k2( 1 ) = 4.0 / ( b * b );

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

    op.read( vs, "axis",            _get_axis );

    op.write( vs, "k1k2",           _get_k1k2 );
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
setup_accessors()
{
	stream_config& cfg			= *super::_stream_config;
    stream_options& opts        = cfg.get_options();

    _get_position.setup();
    _get_normal.setup();
    _get_neighbors.setup();
    _get_axis.setup();
    _get_k1k2.setup();
    
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::setup_negotiate()
{
	stream_config& cfg			= *super::_stream_config;
    stream_options& opts        = cfg.get_options();

    // we need at least 8 neighbors
    if ( opts.get( "number-of-neighbors").get_int() < kNumberOfNeighbors )
        opts.get( "number-of-neighbors").set_int( kNumberOfNeighbors );
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

