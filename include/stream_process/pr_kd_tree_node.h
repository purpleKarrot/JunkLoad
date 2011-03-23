#ifndef __VMML__PR_KD_TREE_NODE__H__
#define __VMML__PR_KD_TREE_NODE__H__

#include <boost/pool/object_pool.hpp>

#include "VMMLibIncludes.h"

#include "tree_common.h"
#include "exception.hpp"

#include "stream_point.h"
#include "rt_struct_member.h"
#include "knn_search_visitor.h"

#include <list>

namespace stream_process
{
//#define TREE_STATS

class pr_kd_tree_node 
{
public:
    typedef     std::deque< stream_point* >  bucket;

    pr_kd_tree_node();
    /*virtual*/ ~pr_kd_tree_node();
    
    /*virtual*/ void    insert( stream_point* point );
                void    insert_bottom_up( stream_point* point );

    /*virtual*/ void    remove( stream_point* point );

    /*virtual*/ void    spill_bucket(); // sort bucket into child nodes
    /*virtual*/ void    split();

    // get index of child in which the point belongs 
    /*virtual*/ size_t  find_child( stream_point* point ) const;
    #ifdef SPROCESS_OLD_VMMLIB
    template< typename T, template< typename T > class vector_t >
    size_t  find_child( const vector_t< T >& point ) const;
    #else
    template< typename vector_t >
    size_t  find_child( const vector_t& point ) const;
    #endif

    /*virtual*/ inline pr_kd_tree_node* get_parent() const;
    /*virtual*/ inline pr_kd_tree_node* get_child( size_t num ) const;
    
    // child nodes queries
    inline bool     has_children() const;
    inline bool     is_leaf() const;

    // payload queries
    inline size_t   get_size() const;
    inline bool     is_empty() const;
       
    // bucket access
    inline bucket& get_bucket();
    inline const bucket& get_bucket() const;
    
    static void set_bucket_capacity( const size_t capacity );
    static inline const size_t get_bucket_capacity();

    friend std::ostream& operator<<( std::ostream& o, 
        const pr_kd_tree_node& node )
    {
        o << "pr_kd_tree_node\n"
            << "  bucket size: " << ( ( node._bucket ) ? node._bucket->size() : 0 )
            << "\n"
        << "  " << node._aabb;
        if ( node.has_children() )
        {
            o << "child0 :\n  " << *node._child_nodes[ 0 ] 
                << "child1 :\n  " << *node._child_nodes[ 1 ];
        }
        o << std::endl;
        return o;
    }
    
    #ifdef TREE_STATS
    const size_t    get_load_count();
    const size_t    get_node_count();
    #endif
    
    inline const vec2f     get_split() const;
    inline const size_t    get_axis() const;
    inline const aabb2f&   get_aabb() const;
    inline aabb2f&         get_aabb();

    inline void set_split( const vec2f& split_ );
    inline void set_axis( const size_t axis_ );
    inline void set_aabb( const aabb2f& aabb_ );
    
    template< typename visitor_t >
    inline void accept_visitor( visitor_t* visitor_ );

    static void set_position( const rt_struct_member< vec3f >& position );

protected:
    void _create_child_nodes();
    void _destroy_child_nodes();
    void _setup( pr_kd_tree_node* parent, const size_t child_num );

    static rt_struct_member< vec3f >       _position;

    // check if this node is empty and all child nodes are empty
    bool    _zombie_check(); 

    pr_kd_tree_node*  _parent;
    pr_kd_tree_node*  _child_nodes[ 2 ];

    uint64_t    _path;
    bool        _has_children;
    bool        _dead;

    bucket*     _bucket;
    
    vec2f       _split;
    size_t      _axis;
    aabb2f      _aabb;

    static size_t _bucket_capacity;
    static boost::object_pool< pr_kd_tree_node > _pool;
    static boost::object_pool< bucket >         _bucket_pool;
    
    static const size_t _max_children;

#ifdef TREE_STATS
    static size_t _node_count;
    static size_t _load_count;
#endif

};//class pr_kd_tree_node




inline pr_kd_tree_node* 
pr_kd_tree_node::get_child( size_t num ) const
{
    assert( num < 2 );
    return _child_nodes[ num ];
}



inline pr_kd_tree_node* 
pr_kd_tree_node::get_parent() const
{
    return _parent;
}




inline size_t 
pr_kd_tree_node::get_size() const
{
    return _bucket ? _bucket->size() : 0;
}




inline bool 
pr_kd_tree_node::is_empty() const
{
    return _bucket ? _bucket->size() : 0;
}




inline bool 
pr_kd_tree_node::is_leaf() const
{
    return !_has_children;
}




inline bool 
pr_kd_tree_node::has_children() const
{
    return _has_children;
}




inline pr_kd_tree_node::bucket& 
pr_kd_tree_node::get_bucket()
{
    assert( _bucket );
    return *_bucket;
}



inline const pr_kd_tree_node::bucket& 
pr_kd_tree_node::get_bucket() const
{
    assert( _bucket );
    return *_bucket;
}




inline void 
pr_kd_tree_node::set_bucket_capacity( size_t capacity )
{
    _bucket_capacity = capacity;
}



inline const size_t 
pr_kd_tree_node::get_bucket_capacity()
{
    return _bucket_capacity;
}




inline size_t        
pr_kd_tree_node::find_child( stream_point* point ) const
{
    assert( point );
    return ( point->get( _position )[ _axis ] < _split[ _axis ] ) ? 0 : 1;
}

    
#ifdef SPROCESS_OLD_VMMLIB
template< typename T, template< typename T > class vector_t >
inline size_t
pr_kd_tree_node::find_child( const vector_t< T >& point ) const
{
    return ( point[ _axis ] < _split[ _axis ] ) ? 0 : 1;
}
#else
template< typename vector_t >
size_t
pr_kd_tree_node::find_child( const vector_t& point ) const
{
    return ( point( _axis ) < _split( _axis ) ) ? 0 : 1;
}
#endif




inline const vec2f
pr_kd_tree_node::get_split() const
{
    return _split;
}



inline void
pr_kd_tree_node::set_split( const vec2f& split_ ) 
{
    _split = split_;
}



inline const size_t 
pr_kd_tree_node::get_axis() const
{
    return _axis;
}



inline void 
pr_kd_tree_node::set_axis( const size_t axis )
{
    _axis = axis;
}



inline const aabb2f&
pr_kd_tree_node::get_aabb() const
{
    return _aabb;
}



inline aabb2f&
pr_kd_tree_node::get_aabb()
{
    return _aabb;
}



inline void
pr_kd_tree_node::set_aabb( const aabb2f& aabb_ )
{
    _aabb = aabb_;
}



inline void 
pr_kd_tree_node::_setup( pr_kd_tree_node* parent, 
    const size_t child_num )
{
    assert( parent );
    
    _parent = parent;
    memset( _child_nodes, 0, sizeof( void* ) * 2 );

    _path = ( _parent->_path << 1 ) | child_num;
    _has_children = false;
    _dead = true;

    _split = _aabb.get_center();
    _axis = ( _parent->_axis + 1 ) % 2;
    // _aabb -> already setup in split() of the parent
}



template< typename visitor_t >
inline void 
pr_kd_tree_node::accept_visitor( visitor_t* visitor_ )
{
    visitor_->visit( this );
}



#ifdef TREE_STATS
inline const size_t    
pr_kd_tree_node::get_load_count()
{
    return _load_count;
}



inline const size_t    
pr_kd_tree_node::get_node_count()
{
    return _node_count;
}
#endif


} // namespace stream_process

#endif


