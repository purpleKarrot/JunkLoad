#ifndef __STREAM_PROCESS__KNN_SEARCH_VISITOR__H__
#define __STREAM_PROCESS__KNN_SEARCH_VISITOR__H__

#include "VMMLibIncludes.h"
#include "pair_functors.h"

#include "stream_point.h"
#include "neighbor.h"
#include "tree_common.h"

#include <queue>

namespace stream_process
{

template< typename node_t, typename compare_t >
class knn_search_visitor
{
public:
    typedef std::priority_queue< 
        std::pair< float, node_t* >, 
        std::vector< std::pair< float, node_t* > >,
        pair_compare_first_adapter< float, node_t*, std::greater< float > >
        > node_priority_queue;
        
    typedef pair_compare_first_adapter< float, stream_point*, compare_t > 
        pair_compare_t;
        
    knn_search_visitor( size_t k_, 
        const rt_struct_member< vec3f >&    position,
        const rt_struct_member< neighbor >& neighbors,
        const rt_struct_member< uint32_t >& nb_count );
    
    inline void visit( node_t* node );

    void initialize_query( stream_point* query_point );
    
    const pair_compare_t& get_pair_compare_functor() const
    { return _pair_compare; }
    
    const compare_t& get_compare_functor() const
    { return _compare; }

protected:
    // tests _query_point for inclusion of neighbor
    inline void _test_neighbor( stream_point* neighbor );
    // tests point for inclusion for _query_point
    inline void _reverse_test_neighbor( float distance, stream_point* rev_nb );
    
    // for cases where nb_count < _k
    inline void _add_neighbor( stream_point* point );

    inline void _compute_distance_and_add_node( node_t* node );
    inline float _compute_min_dist( node_t* node ) const;
    
    inline float _distance_to( stream_point* neighbor );

    #if 1
    public:
        void _DEBUG_check_neighbors( stream_point* point );
        rt_struct_member< uint32_t > _point_index;
    protected:
        void _DEBUG_print_neighbors( stream_point* point );
        void _DEBUG_check_tree_integrity( node_t* node );
    #endif
    
    size_t _k;
    const rt_struct_member< vec3f >&    _position;
    const rt_struct_member< neighbor >& _neighbors;
    const rt_struct_member< uint32_t >& _nb_count;

    /**  
    * naming according to Samet, 2006
    *
    * q = _query_point -> point of interest, whose knn we are looking for  
    * L-> queue of pairs < distance, point_ptr > of 
    *         knn-candidates ( = result, when algo finished )
    *   = "neighbor" member in the stream_point structure
    * A -> list (pq) of pairs< min_dist, node_ptr > of nodes that might 
    *         contain knns
    *   = _nodes 
    */
    
    stream_point*   _query_point; // q
    neighbor*       _query_neighbors; // L
    neighbor*       _query_neighbors_end;
    neighbor*       _query_last_neighbor;
    vec3f           _query_position;
    
    compare_t       _compare;
    pair_compare_t  _pair_compare;

    node_priority_queue _nodes; // A
    
    
}; // class visitor



template< typename node_t, typename compare_t >
knn_search_visitor< node_t, compare_t >
    ::knn_search_visitor( size_t k_, const rt_struct_member< vec3f >& position,
            const rt_struct_member< neighbor >& neighbors, 
            const rt_struct_member< uint32_t >& nb_count )
    : _k( k_ )
    , _position( position )
    , _neighbors( neighbors )
    , _nb_count( nb_count )
{}



template< typename node_t, typename compare_t >
void
knn_search_visitor< node_t, compare_t >::visit( node_t* e )
{
    const uint32_t& nb_count = _nb_count.get( _query_point );
    if ( e->is_leaf() )
    {
        const typename node_t::bucket& bucket_ = e->get_bucket();
        if ( ! bucket_.empty() )
        {
            typename node_t::bucket::const_iterator it      = bucket_.begin();
            typename node_t::bucket::const_iterator it_end  = bucket_.end();
            
            for( ; it != it_end && nb_count != _k; ++it )
            {
                _add_neighbor( *it );
            }
            for( ; it != it_end; ++it )
            {
                _test_neighbor( *it );
            }
        }
    }
    else
    {
        _compute_distance_and_add_node( e->get_child( 0 ) );
        _compute_distance_and_add_node( e->get_child( 1 ) );
    }

    while( !_nodes.empty() )
    {
        const std::pair< float, node_t* > next_node = _nodes.top();
        _nodes.pop();
       
        if ( nb_count == _k && next_node.first > _query_neighbors->first )
        {
            // the min_dist to the best node is farther away than the 
            // distance to the kth neighbor -> we can stop the neighbor 
            // search
            while( ! _nodes.empty() ) 
            { 
                _nodes.pop(); 
            }
        }
        else // search the next node for knns
        {
            next_node.second->accept_visitor( this );
        }
    }
}



template< typename node_t, typename compare_t >
inline void 
knn_search_visitor< node_t, compare_t >::_test_neighbor( stream_point* neighbor_ )
{
    assert( neighbor_ );
    assert( _query_point->get( _nb_count ) == _k );
   
    float dist = _query_position.distanceSquared( neighbor_->get( _position ) );
    
    if ( _compare( dist, _query_neighbors->first ) ) // _nb_pq->get_distance()
    {
        // replace kth neighbor in _query_point's nb pqueue 
        std::pop_heap( _query_neighbors, _query_neighbors_end, _pair_compare );

        _query_last_neighbor->set( dist, neighbor_ );
        
        std::push_heap( _query_neighbors, _query_neighbors_end, _pair_compare );
    }

    // test if _query_point should be added to the neighbors pq    
    _reverse_test_neighbor( dist, neighbor_ );
}



template< typename node_t, typename compare_t >
inline void 
knn_search_visitor< node_t, compare_t >::_reverse_test_neighbor( 
    float distance, stream_point* reverse_nb )
{
    assert( reverse_nb != _query_point );

    uint32_t& nb_count  = reverse_nb->get( _nb_count );
    neighbor* neighbors = reverse_nb->get_ptr( _neighbors );
    
    if ( nb_count == _k )
    {
        if ( _compare( distance, neighbors->first ) )
        {
            // insert _query_point into point's nb pq
            neighbor* neighbors_end = neighbors + _k;
            std::pop_heap( neighbors, neighbors_end, _pair_compare );

            neighbors[ _k - 1 ].set( distance, _query_point );
            std::push_heap( neighbors, neighbors_end, _pair_compare );
        }
    }
    else // nb_count != k -> nb_count < k
    {
        neighbors[ nb_count ].set( distance, _query_point );
        ++nb_count;

        if ( nb_count == _k )
        {
            std::make_heap( neighbors, neighbors + _k, _pair_compare );
        }
    }
}



template< typename node_t, typename compare_t >
inline void 
knn_search_visitor< node_t, compare_t >::_add_neighbor( stream_point* neighbor_ )
{
    assert( neighbor_ );
    assert( neighbor_ != _query_point );

    uint32_t& nb_count = _query_point->get( _nb_count );
    assert( nb_count < _k );

    float dist = _query_position.distanceSquared( neighbor_->get( _position ) );

    // add new entry into _query_point's neighbor list
    _query_neighbors[ nb_count ].set( dist, neighbor_ );
    ++nb_count;

    if ( nb_count == _k )
    {
        // we have enough neighbors, we need a heap to efficiently replace 
        // far away neighbors with ones closer to the query point.
        std::make_heap( _query_neighbors, _query_neighbors_end,
            _pair_compare );
    }
    
    // test if _query_point should be added to the neighbors pq
    _reverse_test_neighbor( dist, neighbor_ );

}



template< typename node_t, typename compare_t >
inline void 
knn_search_visitor< node_t, compare_t >
    ::_compute_distance_and_add_node( node_t* node )
{
    //FIXME 
    // we add the new node to the node_pq. 
    const aabb2f& aabb_ = node->get_aabb();
    const vec2f& min    = aabb_.get_min();
    const vec2f& max    = aabb_.get_max();
    
    // we want the min_dist between the node and the query point.
    // we init the tmp vec with the query point coords and then 
    // subtract the node-min_dist_point before computing the length.
    #ifdef SPROCESS_OLD_VMMLIB
    vec2f tmp( _query_position.x, _query_position.y );
    
    if ( _query_position.x < min.x )
        tmp.x -= min.x;
    else
        tmp.x -= ( _query_position.x < max.x ) 
            ? _query_position.x : max.x;

    if ( _query_position.y < min.y )
        tmp.y -= min.y;
    else
        tmp.y -= ( _query_position.y < max.y ) 
            ? _query_position.y : max.y;
    #else
    vec2f tmp( _query_position.x(), _query_position.y() );
    
    if ( _query_position.x() < min.x() )
        tmp.x() -= min.x();
    else
        tmp.x() -= ( _query_position.x() < max.x() ) 
            ? _query_position.x() : max.x();

    if ( _query_position.y() < min.y() )
        tmp.y() -= min.y();
    else
        tmp.y() -= ( _query_position.y() < max.y() ) 
            ? _query_position.y() : max.y();
    #endif
    
    _nodes.push( std::pair< float, node_t* >( tmp.lengthSquared(), node ) );
}



template< typename node_t, typename compare_t >
void 
knn_search_visitor< node_t, compare_t >::initialize_query( 
    stream_point* query_point )
{
    _query_point            = query_point;
    _query_position         = query_point->get( _position );
    _query_neighbors        = query_point->get_ptr( _neighbors );
    _query_neighbors_end    = _query_neighbors + _k;
    _query_last_neighbor    = _query_neighbors_end - 1;

}


#if 1
template< typename node_t, typename compare_t >
void 
knn_search_visitor< node_t, compare_t >
    ::_DEBUG_print_neighbors( stream_point* point )
{
    const uint32_t& nb_count  = point->get( _nb_count );
    std::cout << nb_count << " neighbors for point " 
        << point->get( _point_index ) << std::endl;
    neighbor* nb = & point->get( _neighbors );
    for( size_t i = 0; i < nb_count; ++i )
    {
        std::cout << i << ": " << nb[i].first << " to " 
            << nb[i].second->get( _point_index ) << "\n";
    }
    std::cout << "end neighbors for " << point << std::endl;
}



template< typename node_t, typename compare_t >
void 
knn_search_visitor< node_t, compare_t >
    ::_DEBUG_check_neighbors( stream_point* point )
{   
    const vec3f& position       = point->get( _position );
    neighbor* nb                = point->get_ptr( _neighbors );
    const uint32_t& nb_count    = point->get( _nb_count );

    for( size_t i = 0; i < nb_count; ++i )
    {
        const vec3f& nb_pos  = nb[i].get_point()->get( _position );
        float dist = position.distanceSquared( nb_pos );
        if ( dist != nb[ i ].get_distance() )
        {
            std::cout
                << "point    "     << point->get( _point_index ) << "\n"
                << "neighbor "  << nb[ i ].get_point()->get( _point_index ) << "\n"
                << "nb " << i << " of " << nb_count << "\n"
                << "real dist: " << dist << " dist in list " << nb->get_distance() << "\n"
                << std::endl;
            
            _DEBUG_print_neighbors( point );

            throw exception( "Invalid entry in neighbor list", 
                SPROCESS_HERE );
        }

    }
}



template< typename node_t, typename compare_t >
void
knn_search_visitor< node_t, compare_t >
    ::_DEBUG_check_tree_integrity( node_t* in_node )
{
    #if 1
    std::deque< node_t* > nodes;
    nodes.push_back( in_node );
    while ( ! nodes.empty() )
    {
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
                    _DEBUG_check_neighbors ( *it );
                }
            }
        }
    
    }
    #endif
}


#endif

} // namespace stream_process

#endif
