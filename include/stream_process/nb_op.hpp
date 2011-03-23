#ifndef __STREAM_PROCESS__NB_OP__HPP__
#define __STREAM_PROCESS__NB_OP__HPP__

#include <stream_process/stream_data.hpp>
#include <stream_process/stream_process_types.hpp>
#include <stream_process/attribute_accessor.hpp>

#include <stream_process/find_neighbors.hpp>

#include <stream_process/operator_base.hpp>
#include <stream_process/chain_operator.hpp>
#include <stream_process/nb_chain_op.hpp>

#include <vector>
#include <queue>

namespace stream_process
{

template< typename operator_base_t, typename node_t >
class nb_op : public operator_base_t
{
public:
	typedef operator_base_t                 super;
	typedef typename super::sp_types_t      sp_types_t;
    typedef node_t                          node_type;

    STREAM_PROCESS_TYPES

    typedef nb_shared_data< sp_types >      nb_shared_data_type;
	typedef find_neighbors< sp_types, node_type, false >    find_neighbors_type;
    typedef nb_chain_op< chain_op_type, node_type >         nb_chain_op_type;

	nb_op();
    nb_op( const nb_op& original );
	
	virtual void push( slice_type* slice );
    virtual slice_type* top();
    virtual void pop();

    virtual void prepare_setup();
    virtual void setup_finalize();
	virtual void setup_attributes();
	virtual void setup_accessors();
    
    virtual void clear_stage();
    
    virtual void add_config_options();

    virtual operator_base_t* clone();
    
    virtual bool is_multithreadable() const		{ return false; }
    virtual bool needs_bounds_checking() const	{ return false; }
    
protected:	
	attribute_accessor< vec3 >			_get_position;
	attribute_accessor< nbh_type >      _get_neighbors;

    nb_shared_data_type                 _nb_shared_data;
	find_neighbors_type                 _find_neighbors;
    
    slice_type*                         _slice;
    nb_chain_op_type*                   _nb_chain_op;
	

}; // class nb_op

#define SP_TEMPLATE_STRING	template< typename operator_base_t, typename node_t >
#define SP_CLASS_NAME		nb_op< operator_base_t, node_t >

SP_TEMPLATE_STRING
SP_CLASS_NAME::
nb_op()
	: super()
    , _nb_shared_data( _get_position, _get_neighbors )
	, _find_neighbors( _nb_shared_data )
    , _slice( 0 )
    , _nb_chain_op( 0 )
{}



SP_TEMPLATE_STRING
SP_CLASS_NAME::
nb_op( const nb_op& original )
    : super( original )
    , _get_position( original._get_position )
    , _get_neighbors( original._get_neighbors )
    , _nb_shared_data( _get_position, _get_neighbors )
    , _find_neighbors( _nb_shared_data )
    , _slice( 0 )
    , _nb_chain_op( 0 )
{
    if ( original._find_neighbors.has_root() )
    {
        // if the other find_neighbors already has a root node, it was
        // previously initialized -> we need to do so too.
        _find_neighbors.setup(
            original._nb_shared_data._k,  
            original._nb_shared_data._max_bucket_size 
        );
    }
    
    assert( original._nb_chain_op == 0 );
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
push( slice_type* slice )
{
    // the slice is already pushed in the chain op.
}



SP_TEMPLATE_STRING
typename SP_CLASS_NAME::slice_type*
SP_CLASS_NAME::
top()
{
    return _find_neighbors.top();
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
pop()
{
    _find_neighbors.pop();
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
prepare_setup()
{
    _nb_chain_op = new nb_chain_op_type( _find_neighbors );
    _nb_chain_op->set_op_name( "nb-chain-op" );
    super::_stream_manager->add_chain_op( _nb_chain_op );
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
setup_finalize()
{
	stream_config& cfg			= *super::_stream_config;
    stream_options& opts        = cfg.get_options();
    
    size_t max_k            = opts.get( "number-of-neighbors" ).get_int();
    size_t max_bucket_size  = opts.get( "bucket-size" ).get_int();
   
    point_set_header& h = cfg.get_header();
    
    vec3 min_ = h.get_aabb_min< sp_float_type >();
    vec3 max_ = h.get_aabb_max< sp_float_type >();
    _nb_shared_data.set_world_aabb( min_, max_ );

    _find_neighbors.setup( max_k, max_bucket_size );
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
setup_attributes()
{
	point_structure& ps = super::_stream_config->get_vertex_structure();
	super::read( ps, "position", _get_position );

	super::write( ps, "neighbors", _get_neighbors, false );
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
setup_accessors()
{
	_get_position.setup();
	_get_neighbors.setup();
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
add_config_options()
{
	stream_options& opts	= super::_stream_config->get_options();

    option k;
    k.setup_int( "number-of-neighbors", 8 );
    k.set_short_name( 'k' );
    k.set_help_text( "number of nearest neighbors." );
    opts.add_option( k, "neighbor" );
    
    option b;
    b.setup_int( "bucket-size", 64 );
    b.set_short_name( 'b' );
    b.set_help_text( "maximum bucket size for tree-nodes." );
    opts.add_option( b, "neighbor" );
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
clear_stage()
{
    _find_neighbors.clear_stage();
}



SP_TEMPLATE_STRING
operator_base_t*
SP_CLASS_NAME::
clone()
{
    return 0;
}


#undef SP_TEMPLATE_STRING
#undef SP_CLASS_NAME

} // namespace stream_process

#endif

