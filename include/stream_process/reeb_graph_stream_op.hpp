#ifndef __STREAM_PROCESS__REEB_GRAPH_STREAM_OP__HPP__
#define __STREAM_PROCESS__REEB_GRAPH_STREAM_OP__HPP__

#include <stream_process/stream_process_types.hpp>

#include <stream_process/reeb_graph_sa.hpp>

namespace stream_process
{

template< typename operator_base_t >
class reeb_graph_stream_op : public operator_base_t
{
public:
	typedef operator_base_t					super;
	typedef typename super::sp_types_t		sp_types_t;

    STREAM_PROCESS_TYPES

	reeb_graph_stream_op();
	reeb_graph_stream_op( const reeb_graph_stream_op& original );
	
	virtual void push( slice_type* slice );
	
	virtual void setup_attributes();
	virtual void setup_accessors();

    virtual operator_base_t* clone();

    virtual bool needs_bounds_checking() const  { return true; }
    virtual bool is_multi_threadable() const    { return false; }

protected:

}; // class reeb_graph_op

#define SP_TEMPLATE_TYPES   template< typename operator_base_t >
#define SP_CLASS_NAME       reeb_graph_stream_op< operator_base_t >

SP_TEMPLATE_TYPES
SP_CLASS_NAME::reeb_graph_stream_op()
    : operator_base_t()
{

}



SP_TEMPLATE_TYPES
SP_CLASS_NAME::reeb_graph_stream_op( const SP_CLASS_NAME& orig )
    : operator_base_t( orig )
{

}



SP_TEMPLATE_TYPES
void
SP_CLASS_NAME::push( slice_type* slice )
{}



SP_TEMPLATE_TYPES
void
SP_CLASS_NAME::setup_attributes()
{}



SP_TEMPLATE_TYPES
void
SP_CLASS_NAME::setup_accessors()
{}



SP_TEMPLATE_TYPES
operator_base_t*
SP_CLASS_NAME::clone()
{
    return new SP_CLASS_NAME( *this );
}



#undef  SP_TEMPLATE_TYPES
#undef  SP_CLASS_NAME

} // namespace stream_process

#endif

