#ifndef __STREAM_PROCESS__FACE_NORMAL_OP__HPP__
#define __STREAM_PROCESS__FACE_NORMAL_OP__HPP__

#include <stream_process/stream_process_types.hpp>

namespace stream_process
{

template< typename operator_base_t >
class face_normal_op : public operator_base_t
{
public:
	typedef operator_base_t                         super;
    typedef typename operator_base_t::sp_types_t    sp_types_t;
    
    STREAM_PROCESS_TYPES
    
    face_normal_op();
    
	virtual void push( slice_type* slice );

	virtual void setup_attributes();
	virtual void setup_accessors();

    virtual operator_base_t* clone() { return new face_normal_op( *this ); }

    virtual bool needs_bounds_checking() const  { return true; }
    virtual bool is_multi_threadable() const    { return true; }

protected:
    // vertex accessors
	attribute_accessor< vec3 >      _get_vertex_position;
	attribute_accessor< vec3 >      _get_vertex_normal;
    
    // face accessors
	attribute_accessor< vec3 >      _get_face_normal;
	attribute_accessor< ptr_vec3 >  _get_vertex_pointers;

}; // class face_normal_op


#define SP_CLASS_NAME      face_normal_op< operator_base_t >
#define SP_TEMPLATE_STRING template< typename operator_base_t >

SP_TEMPLATE_STRING
SP_CLASS_NAME::
face_normal_op()
    : super()
{}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
push( slice_type* slice )
{
    stream_container_type& faces = slice->get_stream( 1 );
    typename stream_container_type::iterator 
        it = faces.begin(), it_end = faces.end();
    for( ; it != it_end; ++it )
    {
        vec3& face_normal           = _get_face_normal( *it );
        const ptr_vec3& vertices    = _get_vertex_pointers( *it );
        
        face_normal.compute_normal( 
            _get_vertex_position( vertices[0] ),
            _get_vertex_position( vertices[1] ),
            _get_vertex_position( vertices[2] )
            );

        for( size_t index = 0; index < 3; ++index )
        {
            vec3& vertex_normal = _get_vertex_normal( vertices[ index ] );
            vertex_normal += face_normal;
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

    super::read( vs,    "position",         _get_vertex_position );
    super::write( vs,   "normal",           _get_vertex_normal, true );

    super::write( fs,   "normal",           _get_face_normal, false );
    super::read(  fs,   "vertex_pointers",  _get_vertex_pointers );
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
setup_accessors()
{
    _get_vertex_position.setup();
    _get_vertex_normal.setup();

    _get_face_normal.setup();
    _get_vertex_pointers.setup();
}



#undef SP_CLASS_NAME
#undef SP_TEMPLATE_STRING

} // namespace stream_process

#endif

