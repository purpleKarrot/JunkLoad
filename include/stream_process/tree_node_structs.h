#ifndef __VMML__TREE_NODE_STRUCTS__H__
#define __VMML__TREE_NODE_STRUCTS__H__

#include "tree_common.h"

namespace vmml
{

template< typename T >
struct basic_node
{
protected:
    T   _split;
};



template< 
    typename T,
    template< typename T > vector_t,
    template aabb_t
    >
struct pr_node
{
    pr_node( pr_node* parent, size_t child_number );

protected:
    aabb_t          _aabb;
    vector_t< T >   _split;
};

template< 
    typename T,
    template< typename T > vector_t,
    template aabb_t
    >
pr_node< T, vector_t, aabb_t >::pr_node( pr_node* parent, size_t child_number )
{

}



template< 
    typename T,
    template< typename T > vector_t
    template aabb_t
    >
struct kd_pr_node
{
protected:
    T               _split;
    size_t          _axis;
    aabb_t          _aabb;
};


};//namespace vmml

#endif


