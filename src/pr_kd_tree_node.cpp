#include "pr_kd_tree_node.h"

#include "stream_point_functors.h"

namespace stream_process
{
//#define TREE_STATS

// static members
boost::object_pool< pr_kd_tree_node >           pr_kd_tree_node::_pool;
boost::object_pool< pr_kd_tree_node::bucket >   pr_kd_tree_node::_bucket_pool;

size_t pr_kd_tree_node::_bucket_capacity = 2;

const size_t pr_kd_tree_node::_max_children = 2;

rt_struct_member< vec3f > pr_kd_tree_node::_position;

#ifdef TREE_STATS
size_t pr_kd_tree_node::_node_count = 0;
size_t pr_kd_tree_node::_load_count = 0;
#endif


pr_kd_tree_node::pr_kd_tree_node()
    : _parent( 0 )
    , _path( 1 )
    , _has_children( false )
    , _dead( true )
    , _bucket( _bucket_pool.construct() )
{
    memset( _child_nodes, 0, sizeof( void* ) * 2 );
}



pr_kd_tree_node::~pr_kd_tree_node()
{
    if ( _has_children )
        _destroy_child_nodes();
    if ( _bucket )
        _bucket_pool.destroy( _bucket );
}



void 
pr_kd_tree_node::insert( stream_point* point )
{
    if ( _has_children )
    {
        _child_nodes[ find_child( point ) ]->insert( point );
    }
    else
    {
        if ( _bucket->size() == _bucket_capacity )
        {
            split();
            _child_nodes[ find_child( point ) ]->insert( point );
        }
        else
        {
            _bucket->push_back( point );
            #ifdef TREE_STATS
            ++_load_count;
            #endif
            if ( _dead )
                _dead = false;

            //_bucket.sort( stream_point_less( _position.get_offset(), _axis ) );
        }
    }
}




void 
pr_kd_tree_node::insert_bottom_up( stream_point* point )
{
    const vec3f& position = point->get( _position );
    
    #ifdef SPROCESS_OLD_VMMLIB
    if ( ! _aabb.is_inside( vec2f( position.x, position.y ) ) )
    #else
    if ( ! _aabb.is_inside( vec2f( position.x(), position.y() ) ) )
    #endif
    {
        if ( _parent )
            _parent->insert_bottom_up( point );
        else
        {
            // FIXME this inserts the point, even if it does not fit into the
            // bounding box of the root node. We have to do this atm because
            // resizing the root aabb is not implemented yet.
            insert( point );
        }
    }
    else if ( _has_children )
    {
        _child_nodes[ find_child( point ) ]->insert( point );
    }
    else 
        insert( point );
#if 0
    else
    {
        if ( _bucket->size() == _bucket_capacity )
        {
            split();
            _child_nodes[ find_child( point ) ]->insert( point );
        }
        else
        {
            _bucket->push_back( point );
            #ifdef TREE_STATS
            ++_load_count;
            #endif
            if ( _dead )
                _dead = false;

            //_bucket.sort( stream_point_less( _position.get_offset(), _axis ) );
        }
    }
#endif
}




void 
pr_kd_tree_node::remove( stream_point* point )
{
    if ( _has_children )
    {
        _child_nodes[ find_child( point ) ]->remove( point );        
    }
    else
    {
        bucket::iterator it 
            = std::find( _bucket->begin(), _bucket->end(), point );
        if ( it != _bucket->end() )
        {
            _bucket->erase( it );
            #ifdef TREE_STATS
            --_load_count;
            #endif
            if ( _bucket->empty() )
            {   
                if ( _has_children && _zombie_check() )
                {
                    _destroy_child_nodes();
                }
                pr_kd_tree_node* p = _parent;
                bool continue_removing = ( p != 0 );
                while ( continue_removing )
                {
                    continue_removing = p->_zombie_check();
                    if ( continue_removing )
                    {
                        p->_destroy_child_nodes();
                        p = p->_parent;
                        if ( p == 0 )
                            continue_removing = false;
                    }
                }
            }
        }
        else
		{
			// DEBUG 
			vec3f position = point->get( _position );
			std::cerr << "pos " << position << std::endl;
            throw exception( "Could not remove payload from tree!",
				SPROCESS_HERE );
		}
	}
}




void 
pr_kd_tree_node::spill_bucket()
{
    bucket::iterator it     = _bucket->begin();
    bucket::iterator itend  = _bucket->end();
    for( ; it != itend; ++it )
    {
        _child_nodes[ find_child( *it ) ]->insert( *it );
        #ifdef TREE_STATS
        --_load_count;
        #endif
    }
    _bucket->clear();
}




void 
pr_kd_tree_node::_create_child_nodes()
{
    assert( ! _has_children );
    for( size_t i = 0; i < _max_children; ++i )
    {
        _child_nodes[ i ] = _pool.construct();
        #ifdef TREE_STATS
        ++_node_count;
        #endif
    }
    _has_children = true;
}




void 
pr_kd_tree_node::_destroy_child_nodes()
{
    assert( _dead && _has_children );
    for( size_t i = 0; i < _max_children; ++i )
    {
        if ( _child_nodes[ i ]->_has_children )
        {
            _child_nodes[ i ]->_destroy_child_nodes();
        }
        _pool.destroy( _child_nodes[ i ] );
        #ifdef TREE_STATS
        --_node_count;
        #endif
    }
    _has_children = false;
    _bucket = _bucket_pool.construct();
}



bool    
pr_kd_tree_node::_zombie_check()
{
    if ( _has_children )
    {
        _dead = true;
        for( size_t i = 0; _dead && i < _max_children; ++i )
        {
            _dead = _child_nodes[ i ]->_zombie_check();
        }
        return _dead;
    }
    else
    {
        return _bucket->empty();
    }

}



void
pr_kd_tree_node::split()
{
    _create_child_nodes();
    
    pr_kd_tree_node* child0( _child_nodes[0] );
    pr_kd_tree_node* child1( _child_nodes[1] );
    
    _aabb.split_kd( _axis, _split[ _axis ], child0->_aabb, child1->_aabb );

    child0->_setup( this, 0 );
    child1->_setup( this, 1 );

    spill_bucket();
    // this node has child nodes now, so we don't need the bucket anymore
    _bucket_pool.destroy( _bucket );
    _bucket = 0;
}



void 
pr_kd_tree_node::set_position( const rt_struct_member< vec3f >& position )
{
    _position = position;
}



} // namespace stream_process

