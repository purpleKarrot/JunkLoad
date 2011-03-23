#ifndef __VMML__KDTREE_COMMON__H__
#define __VMML__KDTREE_COMMON__H__

#include "Vector2.h"
#include "Vector3.h"

#include "aabb.h"

namespace vmml
{

typedef aabb< float, Vector2, 2 > aabb2f;


typedef ssize_t KDTREE_SPLIT_AXIS;
static const ssize_t UNDEFINED_SPLIT =   -1;
static const ssize_t X_AXIS_SPLIT    =   0;
static const ssize_t Y_AXIS_SPLIT    =   1;

struct node_data
{
    node_data( float& split_value_, KDTREE_SPLIT_AXIS& axis_, 
        const vec2f& center_, const uint64_t& path_ )
        : split_value( split_value_ )
        , axis( axis_ )
        , center( center_ )
        , path( path_ )
    {}
    
    float& split_value;
    KDTREE_SPLIT_AXIS& axis;
    const vec2f& center;
    const uint64_t& path;
};

};//namespace vmml

#endif

