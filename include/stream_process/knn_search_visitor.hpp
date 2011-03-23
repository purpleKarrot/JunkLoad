#ifndef __STREAM_PROCESS__KNN_SEARCH_VISITOR__HPP__
#define __STREAM_PROCESS__KNN_SEARCH_VISITOR__HPP__

#include <stream_process/VMMLibIncludes.h>

#include <stream_process/pair_functors.h>

#include <stream_process/stream_data.hpp>
#include <stream_process/neighbors.hpp>
#include <stream_process/attribute_accessor.hpp>

#include <queue>
#include <functional>

namespace stream_process
{

template<
    typename sp_types_t, 
    typename node_t, 
    typename position_accessor_t    = attribute_accessor< vmml::vector< 3, typename sp_types_t::sp_float_type > >,
    typename neighbors_accessor_t   = attribute_accessor< neighbors< sp_types_t > >
    >
class knn_search_visitor
{
public:
    typedef typename sp_types_t::sp_float_type  sp_float_type;
    typedef vmml::vector< 3, sp_float_type >    vec;

    typedef neighbors< sp_types_t >             neighbors_type;

    typedef node_t                              node_type;
    typedef typename node_type::iterator        node_iterator;

    typedef typename node_t::bucket_type        bucket_type;
    typedef typename bucket_type::iterator      bucket_iterator;
    
    typedef std::pair< sp_float_type, node_type* >  candidate_type;

    typedef std::priority_queue< 
        candidate_type, 
        std::vector< candidate_type >,
        pair_compare_first_adapter< sp_float_type, node_t*, std::greater< sp_float_type > > 
        >   candidate_priority_queue;
        
    knn_search_visitor( 
        const position_accessor_t& get_position,
        neighbors_accessor_t& get_neighbors
        );
    
    inline void visit( node_type* node );
    inline void initialize_query( stream_data* stream_data_ );
    
protected:
    inline void     _is_knn( stream_data* neighbor );
    inline void     _compute_distance_and_add_node( node_type* node );

    const position_accessor_t&  _get_position;
    neighbors_accessor_t&       _get_neighbors;

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
    
    stream_data*                _query_point; // q    
    candidate_priority_queue    _nodes; // A
    
    
}; // class visitor

#define KNN_SEARCH_VISITOR_TEMPLATE \
    template< typename sp_types_t, typename node_t, \
        typename position_accessor_t, typename neighbors_accessor_t >
    
#define KNN_SEARCH_VISITOR_CLASS_NAME \
    knn_search_visitor< sp_types_t, node_t, position_accessor_t, \
        neighbors_accessor_t >

KNN_SEARCH_VISITOR_TEMPLATE
KNN_SEARCH_VISITOR_CLASS_NAME::
knn_search_visitor(
    const position_accessor_t&  get_position,
    neighbors_accessor_t&       get_neighbors
    )
    : _get_position( get_position )
    , _get_neighbors( get_neighbors )
{}



KNN_SEARCH_VISITOR_TEMPLATE
void
KNN_SEARCH_VISITOR_CLASS_NAME::
visit( node_t* e )
{
    if ( e->is_leaf() )
    {
        bucket_type& bucket_ = e->get_bucket();
        if ( ! bucket_.empty() )
        {
            bucket_iterator it = bucket_.begin(), it_end = bucket_.end();
            for( ; it != it_end; ++it )
            {
                _is_knn( *it );
            }
        }
    }
    else
    {
        node_iterator it = e->begin(), it_end = e->end();
        for( ; it != it_end; ++it )
        {
            node_type* n = *it;
            if ( n )
                _compute_distance_and_add_node( n );
        }
    }

    neighbors_type& nbs = _get_neighbors( _query_point );

    while( !_nodes.empty() )
    {
        const candidate_type next_node = _nodes.top();
        _nodes.pop();
       
        if ( nbs.is_full() && next_node.first > nbs.get_farthest().get_distance() )
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



KNN_SEARCH_VISITOR_TEMPLATE
inline void 
KNN_SEARCH_VISITOR_CLASS_NAME::
_is_knn( stream_data* neighbor_ )
{
    assert( neighbor_ );
    assert( neighbor_ != _query_point );

    const vec& q_pos        = _get_position( _query_point );
    const vec& nb_pos       = _get_position( neighbor_ );

    sp_float_type distance     = q_pos.squared_distance( nb_pos );

    // add new entry into _query_point's neighbor list ( if dist < kth-nb dist)
    neighbors_type& nbs_    = _get_neighbors( _query_point );
    nbs_.push_if( distance, neighbor_ );
    
    // and we do the same the other way around
    neighbors_type& nb_nbs  = _get_neighbors( neighbor_ );
    nb_nbs.push_if( distance, _query_point );

}



KNN_SEARCH_VISITOR_TEMPLATE
inline void 
KNN_SEARCH_VISITOR_CLASS_NAME::
_compute_distance_and_add_node( node_t* node )
{
    // get the minimum distance between the node and the query point.

    const vec& position = _get_position( _query_point );
    sp_float_type squared_distance = node->get_squared_min_distance( position );

    // we add the new node to the node_pq. 
    _nodes.push( candidate_type( squared_distance, node ) );
}



KNN_SEARCH_VISITOR_TEMPLATE
void 
KNN_SEARCH_VISITOR_CLASS_NAME::
initialize_query( stream_data* query_point_ )
{
    _query_point = query_point_;
}


#undef KNN_SEARCH_VISITOR_TEMPLATE
#undef KNN_SEARCH_VISITOR_CLASS_NAME


} // namespace stream_process

#endif
