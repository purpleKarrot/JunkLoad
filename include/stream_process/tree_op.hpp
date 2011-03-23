#ifndef __STREAM_PROCESS__TREE_OP__HPP__
#define __STREAM_PROCESS__TREE_OP__HPP__

#include "active_set_op.h"
#include "point_info.hpp"
#include "options.h"

#include "neighbor.h"
#include "knn_search_visitor.h"

#include "index_queue.h"
#include "tree_op_shared_data.hpp"

#include <deque>

//#define MAX_COUNT 8

namespace stream_process
{

class active_set;

template< class node_t >
class tree_op : public active_set_op
{
public:
    typedef std::deque< stream_point* > stream_point_queue;

    tree_op();
    virtual ~tree_op();
    
    // -- active set op interface --
    virtual void insert( stream_point* point );
    virtual void remove( stream_point* point );

    virtual void prepare_processing();
    virtual void prepare_shutdown();

    virtual void setup_stage_0();
    virtual void setup_stage_1();
    virtual void setup_stage_2();
    
    tree_op_shared_data&  get_shared_data();
    const tree_op_shared_data&  get_shared_data() const;
    
    node_t& get_root_node() { return _root; };
    
protected:
    void            _update_index_queue( stream_point* point );
    void            _activate_next_point();
    inline uint32_t _get_min_ref() const;
	
	bool			_is_in_aabb( stream_point* point );

    void            _DEBUG_check_integrity();
       
    rt_struct_member< vec3f >           _position;
    rt_struct_member< uint32_t >        _point_index;
    rt_struct_member< neighbor >        _neighbors;
    rt_struct_member< uint32_t >        _nb_count;
    rt_struct_member< uint32_t >        _min_ref_index;
    rt_struct_member< uint32_t >        _max_ref_index;
    
    node_t  _root;
    
    // we store the points in op ourselves to prevent possibly slow 
    // size() queries
    size_t  _points_in_op;
    size_t  _max_index;

    knn_search_visitor< node_t, std::less< float > >* _knn_search_visitor;

    stream_point_queue  _tmp_queue;
    
    // configurable options
    size_t          _bucket_capacity;
    size_t          _max_neighbors;

    tree_op_shared_data _tree_op_shared_data;
    index_queue&        _index_queue;

    // functors for neighbor comparison
    pair_compare_first_adapter< float, stream_point*, std::greater< float > >
        _pair_greater;
    pair_compare_first_adapter< float, stream_point*, std::less< float > >
        _pair_less;


    size_t _DEBUG_max_points_in_op;
};



template< class node_t >
tree_op< node_t >::tree_op()
    : active_set_op()
    , _position(        "position" )
    , _point_index(     "point_index" )
    , _neighbors(       "neighbors" )
    , _nb_count(        "nb_count" )
    , _min_ref_index(   "min_ref_index" )
    , _max_ref_index(   "max_ref_index" )
	, _root()
	, _points_in_op( 0 )
	, _max_index( 0 )
    , _knn_search_visitor( 0 )
    , _tmp_queue( 0 )
	, _bucket_capacity( 128 )
	, _max_neighbors( 0 )
    , _tree_op_shared_data()
    , _index_queue( _tree_op_shared_data.index_queue_ )
    , _DEBUG_max_points_in_op( 0 )
{
    set_name( "tree" );
}


    
template< class node_t >
tree_op< node_t >::~tree_op()
{
    std::cout << "chain op: max nb of points " << _DEBUG_max_points_in_op << std::endl;
}



template< class node_t >
void 
tree_op< node_t >::setup_stage_0()
{
    //_reserve( _min_ref_index );
    //_reserve( _max_ref_index );
    _reserve( _nb_count );

    var_map::iterator it = _config->find( "nb-count" );
    if( it != _config->end() )
        _max_neighbors = (*it).second.as< size_t >();
    else
        _max_neighbors = 8;
}



template< class node_t >
void 
tree_op< node_t >::setup_stage_1()
{
    // check for custom neighbor count ( configurable option )
    var_map::iterator it = _config->find( "nb-count" );
    if( it != _config->end() )
        _max_neighbors = (*it).second.as< size_t >();
    
    LOGINFO
        << "k (number of neighbors)         "
        << _max_neighbors
        << std::endl;
    
    // check for custom bucket size ( configurable option )
    it = _config->find( "bucket-size" );
    if( it != _config->end() )
        _bucket_capacity = (*it).second.as< size_t >();
    // and set it in the tree
    node_t::set_bucket_capacity( _bucket_capacity );
    
    LOGINFO
        << "b (kd-tree bucket size)         " << 
        _bucket_capacity
        << std::endl;

    // reserve the memory for the neighbor distance-pointer pairs.
    // we cannot do this in the constructor since ^^ (e.g. other ops)
    // might change _max_neighbors
    _reserve_custom_type( _neighbors, SP_UNKNOWN_DATA_TYPE, 
        sizeof( std::pair< float, stream_point* > ), _max_neighbors );

}



template< class node_t >
void 
tree_op< node_t >::setup_stage_2()
{
    // set the position rt_struct_member in the tree node
    // node_t::_position ? TODO
    _root.set_position( _position );

    // set kd aabb
    const vec3f& min3 = _point_info->get_min();
    const vec3f& max3 = _point_info->get_max();

    // we set the extents of the root node to the bounding box we got
    // from the point header
    
    #ifdef SPROCESS_OLD_VMMLIB
    _root.set_aabb( aabb2f( vec2f( min3.x, min3.y ), vec2f( max3.x, max3.y ) ) );
    #else
    _root.set_aabb( aabb2f( vec2f( min3.x(), min3.y() ), vec2f( max3.x(), max3.y() ) ) );
    #endif
    
    // FIXME make configurable
    _knn_search_visitor = new knn_search_visitor< node_t, 
        std::less< float > >( _max_neighbors, _position, _neighbors, 
            _nb_count );
    
    #if 1
    _knn_search_visitor->_point_index = _point_index;
    #endif

}



template< class node_t >
void 
tree_op< node_t >::insert( stream_point* point )
{      

    LOGDEBUG << "inserting point into tree " 
        << "- index " << point->get( _point_index ) 
        << " pos " << point->get( _position ) 
        << std::endl;
		
    // safety check for bounds
    if ( ! _is_in_aabb( point ) )
	{
		throw exception( "invalid point position (outside global aabb).",
			SPROCESS_HERE );
	}

    // store latest z for safe range comparison
    _tree_op_shared_data.z_max = point->get( _position )[ 2 ];
    // we have to update the safe range
    _tree_op_shared_data.compute_safe_range();

    // initialize neighbor count to 0
    point->get( _nb_count ) = 0; 
    //memset( point->get_ptr( _neighbors ), 0, _max_neighbors * sizeof( neighbor ) );
   
    ++_max_index;
    
    ++_points_in_op;
    if ( _points_in_op > _DEBUG_max_points_in_op )
        _DEBUG_max_points_in_op = _points_in_op;
    
    // we only start searching once we have at least k neighbors in the tree
    if ( _max_index > _max_neighbors )
    {
        // initialize search visitor and send it down the tree
        _knn_search_visitor->initialize_query( point );
        _root.accept_visitor( _knn_search_visitor ); 
        
        // insert point into the tree
        _root.insert( point );
        // we need to update the index queue so the write op can determine
        // how long it needs to keep this point in the active set
        _update_index_queue( point );
    }
    else if ( _max_index == _max_neighbors )
    {
        _tmp_queue.push_back( point );
        // we have enough points in the tree to start nb search.
        // -> empty tmp queue and start searching...
        stream_point_queue::iterator it        = _tmp_queue.begin();
        stream_point_queue::iterator it_end    = _tmp_queue.end();
        
        stream_point* current_point;
        for( ; it != it_end; ++it )
        {
			current_point = *it;
            
            _knn_search_visitor->initialize_query(  current_point  );
            _root.accept_visitor( _knn_search_visitor );
            // insert point into the tree
            _root.insert( current_point );
            // we need to update the index queue so the write op can determine
            // how long it needs to keep this point in the active set
            _update_index_queue( current_point );
        }
        _tmp_queue.clear();
        _tree_op_shared_data.do_search = true;
    }
    else // max_index < _max_neighbors 
    {
        // we have to store the points in the tmp queue until we have 
        // enough points to fill the nb list in the tree
        _tmp_queue.push_back( point );
    }
       
}



template< class node_t >
void 
tree_op< node_t >::remove( stream_point* point )
{
    --_points_in_op;
    uint32_t index = point->get( _point_index );
    while ( ! _index_queue.empty() && _index_queue.top().second < index )
    {
        _index_queue.pop();
    }
    
    _root.remove( point );
}



template< class node_t >
void 
tree_op< node_t >::prepare_processing()
{
}



template< class node_t >
void 
tree_op< node_t >::prepare_shutdown()
{
}



template< class node_t >
const tree_op_shared_data&
tree_op< node_t >::get_shared_data() const
{
    return _tree_op_shared_data;
}



template< class node_t >
tree_op_shared_data&
tree_op< node_t >::get_shared_data()
{
    return _tree_op_shared_data;
}



template< class node_t >
bool
tree_op< node_t >::_is_in_aabb( stream_point* point )
{
	const aabb2f& aabb = _root.get_aabb();
	const vec2f& min   = aabb.get_min();
	const vec2f& max   = aabb.get_max();

	const vec3f& pos = point->get( _position );
	
    #ifdef SPROCESS_OLD_VMMLIB
	if ( 
		pos.x < min.x ||
		pos.x > max.x ||
		pos.y < min.y ||
		pos.y > max.y		
		)
	{
    #else
	if ( 
		pos.x() < min.x() ||
		pos.x() > max.x() ||
		pos.y() < min.y() ||
		pos.y() > max.y()		
		)
	{
    #endif
        #if 0
		std::cerr 
			<< "pos " << pos << "\n"
			<< "aabb " << aabb << "\n"
			<< std::endl;
        #endif
        return false;
	}
		
	return true;
}



template< class node_t >
void
tree_op< node_t >::_update_index_queue( stream_point* point )
{
    const uint32_t nb_count     = point->get( _nb_count );
    const neighbor* nbs         = point->get_ptr( _neighbors );
    const uint32_t point_index  = point->get( _point_index );

    uint32_t min_ref = std::numeric_limits< uint32_t >::max();
    uint32_t ref;
    for( size_t index = 0; index < nb_count; ++index )
    {
        ref = nbs[ index ].get_point()->get( _point_index );
        if ( ref < min_ref )
            min_ref = ref;
    }
    
    _index_queue.push(
        index_reference( min_ref, point->get( _point_index ) )
        );
}




template< class node_t >
void
tree_op< node_t >::_DEBUG_check_integrity()
{
    std::deque< node_t* > nodes;
    nodes.push_back( &_root );
    size_t cnt = 0;
    while ( ! nodes.empty() )
    {
        ++cnt;
        node_t* node = nodes.front();
        nodes.pop_front();
        if ( ! node->is_leaf() )
        {
            nodes.push_back( node->get_child( 0 ) );
            nodes.push_back( node->get_child( 1 ) );
        }
        else // node is_leaf
        {
            const typename node_t::bucket& bucket_ = node->get_bucket();
            if ( ! bucket_.empty() )
            {
                typename node_t::bucket::const_iterator it      = bucket_.begin();
                typename node_t::bucket::const_iterator it_end  = bucket_.end();
                for( ; it != it_end; ++it )
                {
                    _knn_search_visitor->_DEBUG_check_neighbors ( *it );
                }
            }
        }
    
    }
}



} // namespace stream_process

#endif

