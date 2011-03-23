#ifndef __STREAM_PROCESS__KNN_SEARCH_VISITOR_OLD__H__
#define __STREAM_PROCESS__KNN_SEARCH_VISITOR_OLD__H__

#include "VMMLibIncludes.h"
#include "fixed_size_priority_queue.h"
#include "priority_queue_functors.h"

#include "stream_point.h"

#include <queue>

namespace stream_process
{

typedef std::priority_queue< 
    std::pair< float, stream_point* >,
    std::vector< std::pair< float, stream_point* > >,
    pair_compare_first_adapter< float, stream_point*, std::less >
    > stream_point_priority_queue;

template< typename node_t >
class knn_search_visitor
{
public:
    typedef std::priority_queue< 
        std::pair< float, node_t* >, 
        std::vector< std::pair< float, node_t* > >,
        pair_compare_first_adapter< float, node_t*, std::greater >
        > node_priority_queue;
        
    knn_search_visitor( size_t k_, const rt_struct_member< vec3f >& position );
    
    inline void visit( node_t* node );

    void initialize_query( stream_point* query_point );

    inline stream_point_priority_queue& get_L();
    inline const stream_point_priority_queue& get_L() const;

protected:
    inline void _add_node( node_t* node );
    inline float _compute_min_dist( node_t* node ) const;
    
    size_t _k;
    const rt_struct_member< vec3f >& _position;

    /**  
    * naming according to Samet, 2006
    *
    * q = _query_point -> point of interest, whose knn we are looking for  
    * L = _L -> queue of pairs < distance, point_ptr > of 
    *         knn-candidates ( = result, when algo finished )
    * A = _A -> list (pq) of pairs< min_dist, node_ptr > of nodes that might 
    *         contain knns
    */
    stream_point_priority_queue _L;
    node_priority_queue _A;
    
    stream_point*   _query_point;
    vec3f           _query_position;
    
    
}; // class visitor



template< typename node_t >
knn_search_visitor< node_t >
    ::knn_search_visitor( size_t k_, const rt_struct_member< vec3f >& position )
    : _k( k_ )
    , _position( position )
    , _L()
{}



template< typename node_t >
void
knn_search_visitor< node_t >::visit( node_t* e )
{
    if ( e->is_leaf() )
    {
        const typename node_t::bucket& bucket_ = e->get_bucket();
        if ( ! bucket_.empty() )
        {
            typename node_t::bucket::const_iterator it      = bucket_.begin();
            typename node_t::bucket::const_iterator it_end  = bucket_.end();
            for( ; it != it_end; ++it )
            {
                _L.push( std::pair< float, stream_point* >
                    ( (_query_position - _position.get( *it ) ).lengthSquared(), *it ) );                    
                #if 0
                if ( _L.size() == _k )
                {

                    float distSquared = ( _query_position - _position.get( *it ) ).lengthSquared();
                    if ( _L.top().first > distSquared )
                    {
                        _L.pop();
                        _L.push( std::pair< float, stream_point* >
                            ( distSquared, *it ) );
                    }
                }
                else
                {
                    // FIXME only works when bucketsize < k
                    if ( *it == _query_point ) 
                    {
                        // ignore this point
                    }
                    else
                    {
                        _L.push( std::pair< float, stream_point* >
                            ( (_query_position - _position.get( *it ) ).lengthSquared(), *it ) );                    
                    }
                }
                #endif
                
            }
        }
    }
    else
    {
        _add_node( e->get_child( 0 ) );
        _add_node( e->get_child( 1 ) );

    }
    while( !_A.empty() )
    {
        const std::pair< float, node_t* > next_node = _A.top();
        _A.pop();

        float node_dist = next_node.first;
        float kth_nb_dist = _L.top().first;
        
        if ( _L.size() == _k && next_node.first > _L.top().first )
        {
                // the min_dist to the best node is farther away than the 
                // distance to the kth neighbor -> we can stop the neighbor 
                // search
                while( ! _A.empty() ) 
                {
                    float xnode_dist = _A.top().first;
                    _A.pop();
                }
        }
        else // search the next node for knns
            next_node.second->accept_visitor( this );
    }
}



template< typename node_t >
inline void 
knn_search_visitor< node_t >::_add_node( node_t* node )
{
    //FIXME 
    // we add the new node to the node_pq. 
    const aabb2f& aabb_ = node->get_aabb();
    const vec2f& min    = aabb_.get_min();
    const vec2f& max    = aabb_.get_max();
    
    // we want the min_dist between the node and the query point.
    // we init the tmp vec with the query point coords and then 
    // subtract the node-min_dist_point immediately.
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
    
    _A.push( std::pair< float, node_t* >( tmp.lengthSquared(), node ) );
}



template< typename node_t >
inline float
knn_search_visitor< node_t >::_compute_min_dist( node_t* node ) const
{
    const aabb2f& aabb_ = node->get_aabb();
    const vec2f& min    = aabb_.get_min();
    const vec2f& max    = aabb_.get_max();
    
    // we want the min_dist between the node and the query point.
    // we init the tmp vec with the query point coords and then 
    // subtract the node-min_dist_point immediately.
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
    
    return tmp.lengthSquared();
}


template< typename node_t >
void 
knn_search_visitor< node_t >::initialize_query( stream_point* query_point )
{
    _query_point = query_point;
    _query_position = _position << _query_point;
 
    while ( ! _L.empty() ) _L.pop();
    while ( ! _A.empty() ) _A.pop();

}



template< typename node_t >
inline stream_point_priority_queue& 
knn_search_visitor< node_t >::get_L()
{
    return _L;
}



template< typename node_t >
inline const stream_point_priority_queue& 
knn_search_visitor< node_t >::get_L() const
{
    return _L;
}


} // namespace stream_process

#endif
