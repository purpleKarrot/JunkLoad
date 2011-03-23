#ifndef __STREAM_PROCESS__VERTEX_PRINT_OP__HPP__
#define __STREAM_PROCESS__VERTEX_PRINT_OP__HPP__

namespace stream_process
{
template< typename operator_base_t >
class vertex_print_op : public operator_base_t
{
public:
	typedef operator_base_t                         super;
    typedef typename operator_base_t::sp_types_t    sp_types_t;
    
    STREAM_PROCESS_TYPES
    
    vertex_print_op();
    
	virtual void push( slice_type* slice );

	virtual void setup_attributes();
	virtual void setup_accessors();

    virtual operator_base_t* clone() { return new vertex_print_op( *this ); }

    virtual bool needs_bounds_checking() const  { return true; }
    virtual bool is_multi_threadable() const    { return true; }

protected:
	attribute_accessor< vec3 >      _get_position;
	attribute_accessor< size_t >	_get_index;

}; // class face_print_op


#define SP_CLASS_NAME      vertex_print_op< operator_base_t >
#define SP_TEMPLATE_STRING template< typename operator_base_t >

SP_TEMPLATE_STRING
SP_CLASS_NAME::
vertex_print_op()
    : super()
{}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
push( slice_type* slice )
{
    stream_container_type& vertices = slice->get_stream( 0 );
    
    typename stream_container_type::const_iterator
        it = vertices.begin(), it_end = vertices.end();
    for( ; it != it_end; ++it )
    {
        std::cout
            << "vertex " << _get_index( *it )
            << " at " << _get_position( *it )
            << "\n";
    }
    std::cout << std::endl;

    super::_out_buffer.push_back( slice );
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
setup_attributes()
{
    stream_config& cfg              = *super::_stream_config;
    stream_structure& vs            = cfg.get_vertex_structure();

    super::read( vs, "point_index", _get_index );
    super::read( vs, "position", _get_position );
}


SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
setup_accessors()
{
    _get_index.setup();
    _get_position.setup();
}



#undef SP_CLASS_NAME
#undef SP_TEMPLATE_STRING


} // namespace stream_process

#endif
