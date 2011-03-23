#ifndef __VMML__KDTREE_NODE__H__
#define __VMML__KDTREE_NODE__H__

#include <boost/pool/object_pool.hpp>

#include "kdtree_common.h"
#include "kdtree_split.h"
#include "kdtree_aabb.h"

namespace vmml
{

template<   typename payload_t,
            typename compare_t,
            typename split_t,
            template< typename payload_t > class bucket_t
            >
class kdtree_node
{
public:
    typedef kdtree_node node;
    typedef bucket_t< payload_t > bucket;
    typedef aabb< float, Vector2, 2 > kd_aabb;
    
    typedef payload_t*  payload_pointer;
    typedef payload_t&  payload_reference;

    kdtree_node();
    kdtree_node( kdtree_node* parent );
    
    void reparent( kdtree_node* parent, size_t child_number  );
    
    // payload 
    void insert( payload_t load );
    void remove( payload_t load );

    // payload queries
    inline size_t get_size() const;
    inline bool   is_empty() const;
    
    // child nodes queries
    inline bool   is_leaf() const;
    inline bool   has_children() const;

    inline const kdtree_node* get_child( size_t num ) const;
    
    // bucket access
    const bucket_t< payload_t >& get_bucket() const;
    
    inline KDTREE_SPLIT_AXIS get_split_axis() const;
   
   
    // aabb
    const kdtree_aabb< float, Vector2, 2 >& get_aabb() const;
    void set_aabb( const kdtree_aabb< float, Vector2, 2 >& aabb );
    void set_aabb( const vec2f& min, const vec2f& max );    
    
    static void set_bucket_capacity( const size_t capacity );
    static const size_t get_bucket_capacity();

    friend std::ostream& operator<<( std::ostream& o, 
        const kdtree_node& node )
    {
        o << "kdtree_node\n"
        << "  bucket size: " << node._bucket.size() << "\n"
        << "  split axis: " << node._split_axis << "\n"
        << "  path: " << node._path << "\n"
        << "  " << node._aabb;
        if ( node.has_children() )
        {
            o << "child0 :\n  " << *node._child_nodes[ 0 ] 
                << "child1 :\n  " << *node._child_nodes[ 1 ];
        }
        o << std::endl;
    }
    

protected:

    kdtree_node* _parent;

    kdtree_node* _child_nodes[2];

    float _split_point;
    KDTREE_SPLIT_AXIS _split_axis;
    
    bucket_t< payload_t > _bucket;
    
    kdtree_aabb< float, Vector2, 2 > _aabb;
    
    uint64_t _path;
    
    static size_t _bucket_capacity;
    static boost::object_pool< kdtree_node > _pool;
    static compare_t  _compare;
    static split_t    _split;


};//class kdtree_node


template< typename payload_t, typename compare_t, typename split_t,
    template< typename payload_pointer > class bucket_t >
boost::object_pool< kdtree_node< payload_t, compare_t, split_t, bucket_t > > 
kdtree_node< payload_t, compare_t, split_t, bucket_t >
    ::_pool( 1024 );



template< typename payload_t, typename compare_t, typename split_t,
    template< typename payload_pointer > class bucket_t >
compare_t 
kdtree_node< payload_t, compare_t, split_t, bucket_t >
    ::_compare;



template< typename payload_t, typename compare_t, typename split_t,
template< typename payload_pointer > class bucket_t >
split_t 
kdtree_node< payload_t, compare_t, split_t, bucket_t >
    ::_split;



template< typename payload_t, typename compare_t, typename split_t,
template< typename payload_pointer > class bucket_t >
size_t 
kdtree_node< payload_t, compare_t, split_t, bucket_t >
    ::_bucket_capacity = 2;




template< typename payload_t, typename compare_t, typename split_t,
    template< typename payload_pointer > class bucket_t >
kdtree_node< payload_t, compare_t, split_t, bucket_t >
    ::kdtree_node()
    : _parent( 0 )
    , _path( 1 )
    , _split_axis( UNDEFINED_SPLIT )
{
    memset( _child_nodes, 0, 2 * sizeof( void* ) );
}



template< typename payload_t, typename compare_t, typename split_t,
    template< typename payload_pointer > class bucket_t >
kdtree_node< payload_t, compare_t, split_t, bucket_t >
    ::kdtree_node( kdtree_node* parent )
    : _parent( parent )
    , _path( 1 )
    , _split_axis( UNDEFINED_SPLIT )
{
    memset( _child_nodes, 0, 2 * sizeof( void* ) );
}



template< typename payload_t, typename compare_t, typename split_t,
    template< typename payload_t > class bucket_t >
void 
kdtree_node< payload_t, compare_t, split_t, bucket_t >
    ::reparent( kdtree_node* parent, size_t child_number )
{
    _parent = parent;
    _split_axis = UNDEFINED_SPLIT;
    memset( _child_nodes, 0, 2 * sizeof( void* ) );
    if ( _parent )
    {
        _path = parent->_path << 1;
        if ( child_number ) 
        {
            _path += 1;
        }
    }
    else
    {
        _path = 1;
    }
}




template< typename payload_t, typename compare_t, typename split_t,
    template< typename payload_t > class bucket_t >
void 
kdtree_node< payload_t, compare_t, split_t, bucket_t >
    ::insert( payload_t load )
{
    if ( has_children() )
    {
        _child_nodes[ _compare( load, _split_point, _split_axis ) ]
            ->insert( load );
    }
    else
    {
        _bucket.push_back( load );
        if ( _bucket.size() > _bucket_capacity )
        {
            kdtree_node* child0;
            kdtree_node* child1;
            
            _child_nodes[ 0 ] = child0 = _pool.construct();
            _child_nodes[ 1 ] = child1 = _pool.construct();
            
            _split_axis = ( _parent ) ? _parent->_split_axis : Y_AXIS_SPLIT; 

            node_data node_data_( _split_point, _split_axis, _aabb.get_center(), 
                _path );
            
            _split( node_data_, _bucket, child0->_bucket, child1->_bucket );
                            
            child0->reparent( this, 0 );
            child1->reparent( this, 1 );
            _aabb.split( _split_axis, child0->_aabb, child1->_aabb );
        }
    }
}



template< typename payload_t, typename compare_t, typename split_t,
    template< typename payload_t > class bucket_t >
void 
kdtree_node< payload_t, compare_t, split_t, bucket_t >
    ::remove( payload_t load )
{
    _bucket.remove( load );
}



template< typename payload_t, typename compare_t, typename split_t,
    template< typename payload_t > class bucket_t >
inline size_t 
kdtree_node< payload_t, compare_t, split_t, bucket_t >
    ::get_size() const
{
    return _bucket.size();
}



template< typename payload_t, typename compare_t, typename split_t,
    template< typename payload_t > class bucket_t >
inline bool 
kdtree_node< payload_t, compare_t, split_t, bucket_t >
    ::is_empty() const
{
    return _bucket.empty();
}



template< typename payload_t, typename compare_t, typename split_t,
    template< typename payload_t > class bucket_t >
inline bool 
kdtree_node< payload_t, compare_t, split_t, bucket_t >
    ::is_leaf() const
{
    return _split_axis == UNDEFINED_SPLIT;
}



template< typename payload_t, typename compare_t, typename split_t,
    template< typename payload_t > class bucket_t >
inline bool 
kdtree_node< payload_t, compare_t, split_t, bucket_t >
    ::has_children() const
{
    return _split_axis != UNDEFINED_SPLIT;
}



template< typename payload_t, typename compare_t, typename split_t,
    template< typename payload_t > class bucket_t >
inline KDTREE_SPLIT_AXIS 
kdtree_node< payload_t, compare_t, split_t, bucket_t >
    ::get_split_axis() const
{
    return _split_axis;
}



template< typename payload_t, typename compare_t, typename split_t,
    template< typename payload_t > class bucket_t >
inline const bucket_t< payload_t >& 
kdtree_node< payload_t, compare_t, split_t, bucket_t >
    ::get_bucket() const
{
    return _bucket;
}



template< typename payload_t, typename compare_t, typename split_t,
    template< typename payload_t > class bucket_t >
inline void 
kdtree_node< payload_t, compare_t, split_t, bucket_t >
    ::set_bucket_capacity( size_t capacity )
{
    _bucket_capacity = capacity;
}


template< typename payload_t, typename compare_t, typename split_t,
    template< typename payload_t > class bucket_t >
inline const size_t 
kdtree_node< payload_t, compare_t, split_t, bucket_t >
    ::get_bucket_capacity()
{
    return _bucket_capacity;
}



template< typename payload_t, typename compare_t, typename split_t,
    template< typename payload_t > class bucket_t >
const kdtree_aabb< float, Vector2, 2 >& 
kdtree_node< payload_t, compare_t, split_t, bucket_t >
    ::get_aabb() const
{
    return _aabb;
}



template< typename payload_t, typename compare_t, typename split_t,
    template< typename payload_t > class bucket_t >
void
kdtree_node< payload_t, compare_t, split_t, bucket_t >
    ::set_aabb( const kdtree_aabb< float, Vector2, 2 >& aabb_ )
{
    _aabb = aabb_;
}



template< typename payload_t, typename compare_t, typename split_t,
    template< typename payload_t > class bucket_t >
void
kdtree_node< payload_t, compare_t, split_t, bucket_t >
    ::set_aabb( const vec2f& min, const vec2f& max )
{
    _aabb.set( min, max );
}



template< typename payload_t, typename compare_t, typename split_t,
    template< typename payload_t > class bucket_t >
inline const kdtree_node< payload_t, compare_t, split_t, bucket_t >*
kdtree_node< payload_t, compare_t, split_t, bucket_t >
    ::get_child( size_t num ) const
{
    return _child_nodes[ num % 2 ];
}


};//namespace vmml

#endif


