#ifndef __STREAM_PROCESS__ACCURATE_CURVATURE_OP__HPP__
#define __STREAM_PROCESS__ACCURATE_CURVATURE_OP__HPP__

#include <stream_process/stream_process_types.hpp>

#include <stream_process/compute_accurate_curvature.hpp>
#include <stream_process/compute_curvature.hpp>

namespace stream_process
{
template< typename operator_base_t >
class accurate_curvature_op : public operator_base_t
{
public:
	typedef operator_base_t                         super;
    typedef typename operator_base_t::sp_types_t    sp_types_t;
    
    STREAM_PROCESS_TYPES
    
    //typedef compute_accurate_curvature< sp_types_t > compute_curvature_type;
    typedef compute_curvature< sp_types_t >             compute_curvature_type;
    
    accurate_curvature_op();
    accurate_curvature_op( const accurate_curvature_op& orig_ );
    
	virtual void push( slice_type* slice );

	virtual void setup_attributes();
	virtual void setup_accessors();
	virtual void setup_finalize();
    
    virtual void add_config_options();

    virtual operator_base_t* clone() { return new accurate_curvature_op( *this ); }

    //virtual bool needs_bounds_checking() const  { return true; }
    //virtual bool is_multi_threadable() const    { return true; }

protected:
    compute_curvature_type          _compute_curvature;

}; // class accurate_curvature_op


#define SP_CLASS_NAME      accurate_curvature_op< operator_base_t >
#define SP_TEMPLATE_STRING template< typename operator_base_t >

SP_TEMPLATE_STRING
SP_CLASS_NAME::
accurate_curvature_op()
    : super()
    , _compute_curvature()
{}


SP_TEMPLATE_STRING
SP_CLASS_NAME::
accurate_curvature_op( const accurate_curvature_op& orig_ )
    : super( orig_ )
    , _compute_curvature( orig_._compute_curvature )
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
        _compute_curvature( *it );
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
    
    _compute_curvature.setup_attributes( *this, vs );
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
setup_accessors()
{
	stream_config& cfg			= *super::_stream_config;
    stream_options& opts        = cfg.get_options();
    _compute_curvature.setup_accessors();
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
