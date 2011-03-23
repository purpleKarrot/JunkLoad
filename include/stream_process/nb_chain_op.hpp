#ifndef __STREAM_PROCESS__NB_CHAIN_OP__HPP__
#define __STREAM_PROCESS__NB_CHAIN_OP__HPP__

namespace stream_process
{

template< typename operator_base_t, typename node_t >
class nb_chain_op : public operator_base_t
{
public:
	typedef operator_base_t                             super;
	typedef typename super::sp_types_t                  sp_types_t;
    typedef nb_shared_data< sp_types_t >                nb_shared_data_t;
	typedef find_neighbors< sp_types_t, node_t, false > find_neighbors_t;

    STREAM_PROCESS_TYPES

    nb_chain_op( find_neighbors_t& find_neighbors_ );

    virtual void insert( slice_type* slice );
    virtual void remove( slice_type* slice );

protected:
    find_neighbors_t&       _find_neighbors;

}; // class nb_chain_op

#define SP_TEMPLATE_STRING \
    template< typename operator_base_t, typename node_t >
    
#define SP_CLASS_NAME \
    nb_chain_op< operator_base_t, node_t >
    

SP_TEMPLATE_STRING
SP_CLASS_NAME::
nb_chain_op( find_neighbors_t& find_neighbors_ )
    : _find_neighbors( find_neighbors_ )
{}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
insert( slice_type* slice )
{
    _find_neighbors.push( slice );
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
remove( slice_type* slice )
{
    assert( slice );
    _find_neighbors.remove_from_tree( slice );
}

    

#undef SP_TEMPLATE_STRING
#undef SP_CLASS_NAME

} // namespace stream_process

#endif

