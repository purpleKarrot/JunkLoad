#ifndef __STREAM_PROCESS__MOVED_POINT_REINSERTION_VISITOR__HPP__
#define __STREAM_PROCESS__MOVED_POINT_REINSERTION_VISITOR__HPP__


#include "stream_point.h"
#include "rt_struct_member.h"

#include <algorithm>
namespace stream_process
{

template< class node_t, class compare_t >
class moved_point_reinsertion_visitor
{
public:
    moved_point_reinsertion_visitor( 
        const rt_struct_member< vec3f >& original_position, 
        const rt_struct_member< vec3f >& new_position );

    inline void visit( node_t* node );
    
    void setup( stream_point* point );

protected:
    const rt_struct_member< vec3f >&    _original_position;
    const rt_struct_member< vec3f >&	_new_position;

    stream_point*                       _point;
}; // class moved_point_reinsertion_visitor


template< class node_t, class compare_t >
moved_point_reinsertion_visitor< node_t, compare_t >::
moved_point_reinsertion_visitor( const rt_struct_member< vec3f >& original_pos,
    const rt_struct_member< vec3f >& new_position )
    : _original_position( original_pos )
    , _new_position( new_position )
{}



template< class node_t, class compare_t >
inline void
moved_point_reinsertion_visitor< node_t, compare_t >::visit( node_t* node )
{
    if ( node->is_leaf() )
    {
        const vec3f& new_position = _point->get( _new_position );
        if ( node->get_aabb().is_inside( vec2f( new_position.x(), new_position.y() ) ) )
        {
            // the point is still in the same node.
            return;
        }
        
        // we found the right node
        typename node_t::bucket& bucket_ = node->get_bucket();
        typename node_t::bucket::iterator it 
            = std::find( bucket_.begin(), bucket_.end(), _point );
        if ( it != bucket_.end() )
        {
            //bucket_.erase( it );
            std::iter_swap( it, bucket_.begin() );
            bucket_.pop_front();
            node->insert_bottom_up( _point );
        }
        else
        {
            throw exception( "could not find point in node", SPROCESS_HERE );
        }
    }
    else
    {
        // we have to keep on traversing the tree
        node->get_child( 
            node->find_child( _point->get( _original_position ) ) 
                )->accept_visitor( this );
    }
    
}



template< class node_t, class compare_t >
inline void
moved_point_reinsertion_visitor< node_t, compare_t >::setup( stream_point* point )
{
    _point = point;
}

} // namespace stream_process

#endif

