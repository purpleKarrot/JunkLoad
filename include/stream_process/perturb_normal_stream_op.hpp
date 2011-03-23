#ifndef __STREAM_PROCESS__PERTURB_NORMAL_STREAM_OP__HPP__
#define __STREAM_PROCESS__PERTURB_NORMAL_STREAM_OP__HPP__

#include <stream_process/stream_process_types.hpp>
#include <stream_process/perturb_vector.hpp>

namespace stream_process
{

template< typename operator_base_t >
class perturb_normal_stream_op : public operator_base_t
{
public:
	typedef operator_base_t					super;
	typedef typename super::sp_types_t		sp_types_t;

    STREAM_PROCESS_TYPES
	
    perturb_normal_stream_op();
    perturb_normal_stream_op( const perturb_normal_stream_op& original );
	
	virtual void push( slice_type* slice );
	
	virtual void setup_attributes();
	virtual void setup_accessors();

    virtual operator_base_t* clone();
    
    virtual bool needs_bounds_checking() const  { return false; }
    virtual bool is_multi_threadable() const    { return true; }

protected:
	attribute_accessor< vec3 >			_get_normal;
    
    perturb_vector< vec3 >              _perturb;
    
}; // class estimate_normal_stream_op

#define SP_TEMPLATE_TYPES   template< typename operator_base_t >
#define SP_CLASS_NAME       perturb_normal_stream_op< operator_base_t >


SP_TEMPLATE_TYPES
SP_CLASS_NAME::
perturb_normal_stream_op()
	: super()
    , _get_normal()
{}



SP_TEMPLATE_TYPES
SP_CLASS_NAME::
perturb_normal_stream_op( const perturb_normal_stream_op& original )
	: super( original )
    , _get_normal(      original._get_normal )
{}



SP_TEMPLATE_TYPES
void
SP_CLASS_NAME::
push( slice_type* slice )
{
    assert( slice );
	typename slice_type::iterator it = slice->begin(), it_end = slice->end();
	size_t counter = 0;
    for( ; it != it_end; ++it )
	{
        vec3& normal = _get_normal( *it );
        
        vec3 n_in( normal );
        
        _perturb.perturb_if_unit( normal );
        
        vec3 n_out( normal );
        
        vec3 n_diff = n_in - n_out;
        if ( 
            n_diff.x() != 0 ||
            n_diff.y() != 0 ||
            n_diff.z() != 0
        )
        {
            ++counter;
//            std::cout << "normal diff " << n_out - n_in << std::endl;
        }

	}
  
    std::cout << "perturbed " << counter << " normals in slice " << slice->get_slice_number() << std::endl;
    
    super::_out_buffer.push_back( slice );
}



SP_TEMPLATE_TYPES
void
SP_CLASS_NAME::
setup_attributes()
{
	point_structure& ps = super::_stream_config->get_vertex_structure();
	super::write( ps, "normal", _get_normal, true );
}


SP_TEMPLATE_TYPES
void
SP_CLASS_NAME::
setup_accessors()
{
	_get_normal.setup();
}


SP_TEMPLATE_TYPES
typename SP_CLASS_NAME::super*
SP_CLASS_NAME::
clone()
{
    return new SP_CLASS_NAME( *this );
}

#undef SP_TEMPLATE_TYPES
#undef SP_CLASS_NAME


} // namespace stream_process

#endif

