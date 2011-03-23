#ifndef __VMML__KDTREE_GET__H__
#define __VMML__KDTREE_GET__H__

#include "kdtree_common.h"

namespace vmml
{

template< typename payload_t >
struct vector_get
{

inline const vec3f& operator()( const payload_t& load )
{
    return load;
}

inline vec3f& operator()( payload_t& load )
{
    return load;
}


};//struct default_get

};//namespace vmml

#endif