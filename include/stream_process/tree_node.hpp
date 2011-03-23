#ifndef __STREAM_PROCESS__TREE_NODE__HPP__
#define __STREAM_PROCESS__TREE_NODE__HPP__

#include <boost/pool/pool.hpp>

#include <vector>
#include <cassert>

namespace stream_process
{


template< typename tree_type_t, typename payload_t >
class tree_node : public tree_type_t
{
public:
    typedef payload_t                                   value_type;

    typedef tree_node                                   node_type;
    typedef tree_type_t                                 tree_type;

    typedef typename tree_type::bucket_type             bucket_type;
    typedef typename bucket_type::iterator              bucket_iterator;
    typedef typename bucket_type::const_iterator        bucket_const_iterator;

    typedef typename std::vector< node_type* >          node_container;
    typedef typename node_container::iterator           iterator;
    typedef typename node_container::const_iterator     const_iterator;
    
    static const size_t MAX_NUMBER_OF_CHILDREN = tree_type::M;

    static const size_t IS_ROOT = 1 << 0;
    static const size_t IS_LEAF = 1 << 1;

    // since we really don't want static members, yuck ;)
    struct shared_data
    {
        shared_data(
            size_t max_bucket_size_ = 64,
            size_t initial_size = 1 << 16,
            node_type* ptr = 0
            )
            : max_bucket_size( max_bucket_size_ )
            , node_pool( sizeof( node_type ) )
            , nodes( 1 << 16, ptr )
        {}
        
        inline void resize_if( size_t index )
        {
            const size_t size = nodes.size();
            if ( index >= size )
            {
                //std::cout << "resize nodes " << std::endl;
                node_type* null_ptr = 0;
                nodes.resize( 2 * size, null_ptr );
                const size_t new_size = nodes.size();
                for( size_t index = size; index < new_size; ++index )
                {
                    assert( nodes[ index ] == 0 );
                }
            }
        }

        size_t                      max_bucket_size;
        boost::pool<>               node_pool;
        node_container              nodes;
    };
        
    // root node ctor
    tree_node( const tree_type& root_data, shared_data& shared_data_ );

    // child node ctor
    tree_node( node_type& parent, size_t child_number );
    ~tree_node();
    
    inline void                 insert( const payload_t& payload );
    inline void                 remove( const payload_t& payload );
    
    inline bool                 is_root() const;
    inline bool                 is_leaf() const;
    
    inline bool                 empty() const;
    
    inline node_type*           get_node( size_t index );
    inline const node_type*     get_node( size_t index ) const;

    inline node_type*           get_parent();
    inline const node_type*     get_parent() const;

    inline node_type*           get_child( size_t number );
    inline const node_type*     get_child( size_t number ) const;
    
    // WARNING: these will iterate over all the child nodes and return a ptr
    // to them, so for non-existing children, it will return a 0. 
    inline iterator             begin();
    inline iterator             end();

    inline const_iterator       begin() const;
    inline const_iterator       end() const;
    
    inline bucket_type&         get_bucket();
    inline const bucket_type&   get_bucket() const;
    
    inline void                 create_children();
    inline void                 destroy_children();

    // removes empty nodes - TODO better
    inline void             optimize();
    
    template< typename visitor_t >
    inline void accept_visitor( visitor_t* visitor_ );
    
    using tree_type::find_child_index;
    using tree_type::get_child_index;  
    using tree_type::get_parent_index;

protected:
    inline void         _create_child( size_t child_number );
    inline void         _destroy_child( size_t child_number );
    
    inline void         _set_flag( size_t flag_, bool flag_is_on );

    shared_data&        _shared_data;
    size_t              _flags;
    size_t              _index;


private:
    tree_node() {} // disallow std ctor

}; // class tree_node

#define SP_TEMPLATE_TYPES \
    template< typename tree_type_t, typename payload_t >
#define SP_CLASS_NAME \
    tree_node< tree_type_t, payload_t >


SP_TEMPLATE_TYPES
SP_CLASS_NAME::
tree_node( const tree_type& root_data, shared_data& shared_data_ )
    : tree_type( root_data )
    , _shared_data( shared_data_ )
    , _index( tree_type::get_root_index() )
    , _flags( IS_LEAF | IS_ROOT )
{
    _shared_data.nodes[ _index ] = this;
}



SP_TEMPLATE_TYPES
SP_CLASS_NAME::
tree_node( node_type& parent, size_t child_number_ )
    : tree_type( parent, child_number_ )
    , _shared_data( parent._shared_data )
    , _index( get_child_index( parent._index, child_number_ ) )
    , _flags( IS_LEAF )
{}


SP_TEMPLATE_TYPES
SP_CLASS_NAME::
~tree_node()
{
    if ( ! is_leaf() )
    {
        destroy_children();
    }
    assert( is_leaf() );
    assert( tree_type::_bucket.empty() );

    if ( tree_type::_bucket.empty() && is_leaf() )
        return;

    std::cerr << "WARNING: destroyed non-empty tree node." << std::endl;
}



SP_TEMPLATE_TYPES
inline typename SP_CLASS_NAME::iterator
SP_CLASS_NAME::
begin()
{
    size_t index_ = get_child_index( _index, 0 );
    return _shared_data.nodes.begin() + index_;
}


SP_TEMPLATE_TYPES
inline typename SP_CLASS_NAME::iterator
SP_CLASS_NAME::
end()
{
    return begin() + MAX_NUMBER_OF_CHILDREN;
}


    
SP_TEMPLATE_TYPES
inline typename SP_CLASS_NAME::const_iterator
SP_CLASS_NAME::
begin() const
{
    size_t index_ = get_child_index( _index, 0 );
    return _shared_data.nodes.begin() + index_;
}



SP_TEMPLATE_TYPES
inline typename SP_CLASS_NAME::const_iterator
SP_CLASS_NAME::
end() const
{
    return begin() + MAX_NUMBER_OF_CHILDREN;
}



SP_TEMPLATE_TYPES
inline typename SP_CLASS_NAME::node_type*
SP_CLASS_NAME::
get_node( size_t index )
{
    node_container& n = _shared_data.nodes;
    assert( index < n.size() );
    return n[ index ];
}



SP_TEMPLATE_TYPES
inline const typename SP_CLASS_NAME::node_type*
SP_CLASS_NAME::
get_node( size_t index ) const
{
    const node_container& n = _shared_data.nodes;
    assert( index < n.size() );
    return n[ index ];
}



SP_TEMPLATE_TYPES
inline SP_CLASS_NAME*
SP_CLASS_NAME::
get_parent()
{
    return is_root() ? 0 : get_node( get_parent_index( _index ) );
}


SP_TEMPLATE_TYPES
inline const SP_CLASS_NAME*
SP_CLASS_NAME::
get_parent() const
{
    return is_root() ? 0 : get_node( get_parent_index( _index ) );
}



SP_TEMPLATE_TYPES
inline SP_CLASS_NAME*
SP_CLASS_NAME::
get_child( size_t num )
{
    return is_leaf() ? 0 : get_node( get_child_index( _index, num ) );
}



SP_TEMPLATE_TYPES
inline const SP_CLASS_NAME*
SP_CLASS_NAME::
get_child( size_t num ) const
{
    return is_leaf() ? 0 : get_node( get_child_index( _index, num ) );
}



SP_TEMPLATE_TYPES
inline void
SP_CLASS_NAME::
insert( const value_type& value_ )
{
    bucket_type& bucket     = tree_type::_bucket;
    node_container& nodes  = _shared_data.nodes;

    if ( is_leaf() )
    {
        if ( bucket.size() < _shared_data.max_bucket_size )
        {
            bucket.push_back( value_ );
            return;
        }
        else
        {
            assert( bucket.size() == _shared_data.max_bucket_size );
         
            create_children();
            
            typename bucket_type::iterator
                it      = bucket.begin(),
                it_end  = bucket.end();
            for( ; it != it_end; ++it )
            {
                value_type& v = *it;
                get_child( find_child_index( v ) )->_bucket.push_back( v );
            }
            bucket.clear();
        }
    }

    assert( get_child( find_child_index( value_ ) ) != 0 );
    get_child( find_child_index( value_ ) )->insert( value_ );
}



SP_TEMPLATE_TYPES
inline void
SP_CLASS_NAME::
remove( const value_type& value_ )
{
    bucket_type& bucket     = tree_type::_bucket;
    node_container& nodes  = _shared_data.nodes;

    if ( is_leaf() )
    {
        // find point
        bucket_iterator it = bucket.begin(), it_end  = bucket.end();
        for( ; it != it_end; ++it )
        {
            if ( *it != value_ )
                continue;
            else
            {
                *it = bucket.back();
                bucket.pop_back();
                break;
            }
        }
        if ( bucket.empty() && ! is_root() )
        {
            get_parent()->optimize();
        }
    }
    else
    {
        tree_node* child = get_child( find_child_index( value_ ) );
        assert( child );
        child->remove( value_ );
    }
}



SP_TEMPLATE_TYPES
inline typename SP_CLASS_NAME::bucket_type&
SP_CLASS_NAME::
get_bucket()
{
    return tree_type::_bucket;
}



SP_TEMPLATE_TYPES
inline const typename SP_CLASS_NAME::bucket_type&
SP_CLASS_NAME::
get_bucket() const
{
    return tree_type::_bucket;
}


SP_TEMPLATE_TYPES
inline bool
SP_CLASS_NAME::
is_root() const
{
    return _flags & IS_ROOT;
}



SP_TEMPLATE_TYPES
inline bool
SP_CLASS_NAME::
is_leaf() const
{
    return _flags & IS_LEAF;
}


SP_TEMPLATE_TYPES
inline bool
SP_CLASS_NAME::
empty() const
{
    return tree_type::_bucket.empty();
}


SP_TEMPLATE_TYPES
inline void
SP_CLASS_NAME::
create_children()
{       
    _shared_data.resize_if( get_child_index( _index, 0 ) );

    iterator it = begin(), it_end = end();
    for( size_t child_number = 0; it != it_end; ++it, ++child_number )
    {
        _create_child( child_number );
    }

    _set_flag( IS_LEAF, false );
}



SP_TEMPLATE_TYPES
inline void
SP_CLASS_NAME::
_create_child( size_t child_number )
{
    assert( is_leaf() );
    assert( child_number < MAX_NUMBER_OF_CHILDREN );

    node_container& nodes  = _shared_data.nodes;

    assert( get_child_index( _index, child_number ) < nodes.size() );

    // allocate and construct child node
    tree_node* child
        = reinterpret_cast< tree_node* >( _shared_data.node_pool.malloc() );
    new ( child ) tree_node( *this, child_number );

    assert( nodes[ get_child_index( _index, child_number ) ] == 0 );

    nodes[ get_child_index( _index, child_number ) ] = child;
    
}



SP_TEMPLATE_TYPES
inline void
SP_CLASS_NAME::
destroy_children()
{
    assert( ! is_leaf() );
        
    iterator it = begin(), it_end = end();
    for( size_t child_number = 0; it != it_end; ++it, ++child_number )
    {
        _destroy_child( child_number );
    }

    _set_flag( IS_LEAF, true );

    //optimize();
}



SP_TEMPLATE_TYPES
inline void
SP_CLASS_NAME::
_destroy_child( size_t child_number )
{
    assert( ! is_leaf() );
    assert( child_number < MAX_NUMBER_OF_CHILDREN );

    node_container& nodes  = _shared_data.nodes;

    const size_t child_index = get_child_index( _index, child_number );

    assert( child_index < nodes.size() );

    tree_node* child = nodes[ child_index ];
    
    assert( child );

    if ( ! child->is_leaf() ) 
        child->destroy_children(); // delete the whole subtree

    assert( child->get_bucket().empty() );

    // destruct and deallocate child node
    child->~tree_node();
    _shared_data.node_pool.free( child );

    nodes[ child_index ] = 0;
}


SP_TEMPLATE_TYPES
inline void
SP_CLASS_NAME::
_set_flag( size_t flag_, bool flag_is_on )
{
    if ( _flags & flag_ )
    {
        if ( ! flag_is_on )
        {
            _flags ^= flag_;
        }
    }
    else
    {
        if ( flag_is_on )
            _flags |= flag_;
    }
}


SP_TEMPLATE_TYPES
inline void
SP_CLASS_NAME::
optimize()
{
    bucket_type& bucket     = tree_type::_bucket;
    node_container& nodes  = _shared_data.nodes;

    if ( ! is_leaf() )
    {
        size_t child_load = 0;
        iterator it = begin(), it_end = end();
        for( ; it != it_end; ++it )
        {
            tree_node* child = *it;

            if ( ! child )
                continue;

            if ( ! child->is_leaf() )
                return;
            
            child_load += child->get_bucket().size();
        }
        
        if ( child_load < _shared_data.max_bucket_size >> 1 )
        {
            for( it = begin(), it_end = end(); it != it_end; ++it )
            {
                tree_node* child = *it;
                
                if ( ! child )
                    continue;
                
                bucket_type& child_bucket = child->get_bucket();
                bucket.insert( bucket.end(), child_bucket.begin(), child_bucket.end() );
                child_bucket.clear();
            }
            
        destroy_children();
        }
    }
    if ( ! is_root() )
        get_parent()->optimize();

#if 0
    if ( _children )
    {
        if ( is_leaf() )
        {
            _globals.child_array_pool.free( _children );
            _children = 0;
        }
        else
        {
            size_t child_load = 0;
            iterator
                it      = _children->begin(),
                it_end  = _children->end();
            for( ; it != it_end; ++it )
            {
                tree_node_t* child = *it;

                if ( ! child )
                    continue;

                if ( ! child->is_leaf() )
                    return;
                
                child_load += child->get_bucket().size();
            }
            
            if ( child_load < _globals.max_bucket_size >> 1 )
            {
                it = _children->begin(), it_end = _children->end();
                for( ; it != it_end; ++it )
                {
                    tree_node_t* child = *it;
                    
                    if ( ! child )
                        continue;
                    
                    bucket_t& child_bucket = child->get_bucket();
                    _bucket.insert( _bucket.end(), child_bucket.begin(), child_bucket.end() );
                    child_bucket.clear();
                }
                
                destroy_children();
            }
        }
    }
    else
        if ( ! is_root() )
            get_parent()->optimize();
    
#endif
}




SP_TEMPLATE_TYPES
template< typename visitor_t >
inline void 
SP_CLASS_NAME::
accept_visitor( visitor_t* visitor_ )
{
    visitor_->visit( this );
}

#undef SP_CLASS_NAME
#undef SP_TEMPLATE_TYPES

} // namespace stream_process

#endif

