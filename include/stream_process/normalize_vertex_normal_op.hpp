#ifndef __STREAM_PROCESS__NORMALIZE_VERTEX_NORMAL_OP__HPP__
#define __STREAM_PROCESS__NORMALIZE_VERTEX_NORMAL_OP__HPP__

#include <stream_process/stream_process_types.hpp>

namespace stream_process
{

template< typename operator_base_t >
class normalize_vertex_normal_op : public operator_base_t
{
public:
	typedef operator_base_t                         super;
    typedef typename operator_base_t::sp_types_t    sp_types_t;
    
    STREAM_PROCESS_TYPES
    
    normalize_vertex_normal_op();
    
	virtual void push( slice_type* slice );

	virtual void setup_attributes();
	virtual void setup_accessors();

    virtual operator_base_t* clone() { return new normalize_vertex_normal_op( *this ); }

    virtual bool needs_bounds_checking() const  { return true; }
    virtual bool is_multi_threadable() const    { return true; }

protected:
    // vertex accessors
	attribute_accessor< vec3 >      _get_vertex_normal;

}; // class zero_vertex_normal_op


#define SP_CLASS_NAME      normalize_vertex_normal_op< operator_base_t >
#define SP_TEMPLATE_STRING template< typename operator_base_t >

SP_TEMPLATE_STRING
SP_CLASS_NAME::
normalize_vertex_normal_op()
    : super()
{}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
push( slice_type* slice )
{
    stream_container_type& vertices = slice->get_stream( 0 );
    typename stream_container_type::iterator 
        it = vertices.begin(), it_end = vertices.end();
    for( ; it != it_end; ++it )
    {
        normalize( _get_vertex_normal( *it ) );
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

    super::write( vs, "normal", _get_vertex_normal, true );
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
setup_accessors()
{
    _get_vertex_normal.setup();
}



#undef SP_CLASS_NAME
#undef SP_TEMPLATE_STRING

} // namespace stream_process

#endif

