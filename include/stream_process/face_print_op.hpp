#ifndef __STREAM_PROCESS__FACE_PRINT_OP__HPP__
#define __STREAM_PROCESS__FACE_PRINT_OP__HPP__

namespace stream_process
{
template< typename operator_base_t >
class face_print_op : public operator_base_t
{
public:
	typedef operator_base_t                         super;
    typedef typename operator_base_t::sp_types_t    sp_types_t;
    
    STREAM_PROCESS_TYPES
    
    face_print_op();
    
	virtual void push( slice_type* slice );

	virtual void setup_attributes();
	virtual void setup_accessors();

    virtual operator_base_t* clone() { return new face_print_op( *this ); }

    virtual bool needs_bounds_checking() const  { return true; }
    virtual bool is_multi_threadable() const    { return true; }

protected:
	attribute_accessor< vec3ui >	_get_vertex_indices;
	attribute_accessor< size_t >	_get_face_index;

}; // class face_print_op


#define SP_CLASS_NAME      face_print_op< operator_base_t >
#define SP_TEMPLATE_STRING template< typename operator_base_t >

SP_TEMPLATE_STRING
SP_CLASS_NAME::
face_print_op()
    : super()
{}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
push( slice_type* slice )
{
    stream_container_type& faces = slice->get_stream( 1 );
    
    typename stream_container_type::const_iterator
        it = faces.begin(), it_end = faces.end();
    for( ; it != it_end; ++it )
    {
        const vec3ui& vi =  _get_vertex_indices( *it );
        assert( vi( 0 ) != std::numeric_limits< uint32_t >::max() );
        assert( vi( 1 ) != std::numeric_limits< uint32_t >::max() );
        assert( vi( 2 ) != std::numeric_limits< uint32_t >::max() );

        const size_t face_index = _get_face_index( *it );
        if ( face_index == 132385 )
        {
            std::cout << "XXXXXXXXXX face " << vi << "\n";
        }
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
    stream_structure& fs            = cfg.get_face_structure();

    super::read( fs, "vertex_indices", _get_vertex_indices );
    super::read( fs, "face_index", _get_face_index );
}


SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
setup_accessors()
{
    _get_vertex_indices.setup();
    _get_face_index.setup();
}



#undef SP_CLASS_NAME
#undef SP_TEMPLATE_STRING


} // namespace stream_process

#endif

