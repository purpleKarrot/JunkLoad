#ifndef __VMML__KDTREE_SPLIT__H__
#define __VMML__KDTREE_SPLIT__H__


#include <cfloat>

#include "kdtree_common.h"
#include "kdtree_compare.h"

namespace vmml
{

template< 
    typename float_t,
    template< typename float_t > class vector_t,
    typename payload_t,
    template< typename payload_t > class bucket_t,
    typename compare_t
    >
struct cyclic_split
{   
    inline void operator()( node_data& node,
                            bucket_t< payload_t >& parent_bucket, 
                            bucket_t< payload_t >& child0_bucket,
                            bucket_t< payload_t >& child1_bucket
                            )
{
        compare_t compare;

        if ( node.axis == X_AXIS_SPLIT )
            node.axis  = Y_AXIS_SPLIT;
        else
            node.axis  = X_AXIS_SPLIT;
            
        node.split_value = node.center[ node.axis ];
        
        typename bucket_t< payload_t >::const_iterator it = parent_bucket.begin();
        typename bucket_t< payload_t >::const_iterator ite = parent_bucket.end();
        it = parent_bucket.begin();
        for( ; it != ite; ++it )
        {
            if ( compare( (*it), node )  )
                child1_bucket.push_back( *it );
            else
                child0_bucket.push_back( *it );
        }
        parent_bucket.clear();
}

}; // struct cyclic_split


template< 
    typename float_t,
    template< typename float_t > class vector_t,
    typename payload_t,
    template< typename payload_t > class bucket_t,
    typename compare_t
>
struct range_split
{   
    inline void operator()( node_data& node,
                            bucket_t< payload_t >& parent_bucket, 
                            bucket_t< payload_t >& child0_bucket,
                            bucket_t< payload_t >& child1_bucket
                            )
    {
        compare_t compare;
        
        vector_t < float_t > min = FLT_MAX;
        vector_t < float_t > max = FLT_MIN;         
        typename bucket_t< payload_t >::const_iterator it = parent_bucket.begin();
        typename bucket_t< payload_t >::const_iterator ite = parent_bucket.end();
        for( ; it != ite; ++it )
        {
            for( size_t i = 0; i < 2; ++i )
            {
                if ( compare( (*it), min, i ) )
                    min[ i ] = (*it)[ i ];
                else if ( compare( (*it), max, i ) )
                    max[ i ] > (*it)[ i ];
            }
        }
        vector_t< float_t > min_diff = node.center - min;
        vector_t< float_t > max_diff = max - node.center;
        
        float_t x_diff = ( min_diff.x > max_diff.x ) ? min_diff.x : max_diff.x;
        float_t y_diff = ( min_diff.y > max_diff.y ) ? min_diff.y : max_diff.y;
        
        node.axis = x_diff < y_diff ? X_AXIS_SPLIT : Y_AXIS_SPLIT;
        node.split_value = node.center[ node.axis ];
        
        it = parent_bucket.begin();
        for( ; it != ite; ++it )
        {
            if ( compare( (*it), node )  )
                child1_bucket.push_back( *it );
            else
                child0_bucket.push_back( *it );
        }
        parent_bucket.clear();
    }
    
}; // struct range_split


template< 
typename float_t,
template< typename float_t > class vector_t,
typename payload_t,
template< typename payload_t > class bucket_t,
typename compare_t
>
struct range_and_distribution_split
{   
    inline void operator()( node_data& node,
                            bucket_t< payload_t >& parent_bucket, 
                            bucket_t< payload_t >& child0_bucket,
                            bucket_t< payload_t >& child1_bucket
                            )
    {
        compare_t compare;

        vector_t< float_t > counter( 0, 0 );

        vector_t < float_t > min = FLT_MAX;
        vector_t < float_t > max = FLT_MIN;         

        typename bucket_t< payload_t >::const_iterator it = parent_bucket.begin();
        typename bucket_t< payload_t >::const_iterator ite = parent_bucket.end();
        for( ; it != ite; ++it )
        {
            for( size_t i = 0; i < 2; ++i )
            {
                if ( compare( (*it), node.center[ i ] , i ) )
                    counter[ i ] += 1;
                if ( compare( (*it), min, i ) )
                    min[ i ] = (*it)[ i ];
                else if ( compare( (*it), max, i ) )
                    max[ i ] > (*it)[ i ];                
            }
        }
        
        counter[ 0 ] = std::max( counter[ 0 ], parent_bucket.size() - counter[ 0 ] );
        counter[ 1 ] = std::max( counter[ 1 ], parent_bucket.size() - counter[ 1 ] );

        float_t dist_aspect = counter[ 1 ] / counter[ 0 ];
 
        vector_t< float_t > min_diff = node.center - min;
        vector_t< float_t > max_diff = max - node.center;
        
        float_t x_diff = ( min_diff.x > max_diff.x ) ? min_diff.x : max_diff.x;
        float_t y_diff = ( min_diff.y > max_diff.y ) ? min_diff.y : max_diff.y;
        
        float_t range_aspect = x_diff / y_diff;
        //_parent_split_axis = ;
        
        float_t aspect = range_aspect * dist_aspect;
        
        if ( aspect > 1 )
            node.axis = X_AXIS_SPLIT;
        else
            node.axis = Y_AXIS_SPLIT;
        node.split_value = node.center[ node.axis ];

        it = parent_bucket.begin();
        for( ; it != ite; ++it )
        {
            if ( compare( (*it), node ) )
                child1_bucket.push_back( *it );
            else
                child0_bucket.push_back( *it );
        }
        parent_bucket.clear();
    }
    
}; // struct distribution_split


template< 
    typename float_t,
    template< typename float_t > class vector_t,
    typename payload_t,
    template< typename payload_t > class bucket_t,
    typename compare_t
>
struct distribution_split
{   
    inline void operator()( node_data& node,
                            bucket_t< payload_t >& parent_bucket, 
                            bucket_t< payload_t >& child0_bucket,
                            bucket_t< payload_t >& child1_bucket
                            )
    {
        compare_t compare;
        vector_t< size_t > counter( 0, 0 );
        
        typename bucket_t< payload_t >::const_iterator it = parent_bucket.begin();
        typename bucket_t< payload_t >::const_iterator ite = parent_bucket.end();
        for( ; it != ite; ++it )
        {
            for( size_t i = 0; i < 2; ++i )
            {
                if ( compare( (*it), node.center[ i ] , i ) )
                    counter[ i ] += 1;
            }
        }
        counter[ 0 ] = std::max( counter[ 0 ], parent_bucket.size() - counter[ 0 ] );
        counter[ 1 ] = std::max( counter[ 1 ], parent_bucket.size() - counter[ 1 ] );
        
        // if( counter[ 0 ] == counter[ 1 ] )
        // random? not-parent-split? parent-split?
        if ( counter[ 0 ] < counter[ 1 ] )
            node.axis = X_AXIS_SPLIT;
        else
            node.axis = Y_AXIS_SPLIT;
        node.split_value = node.center[ node.axis ];
       
        it = parent_bucket.begin();
        for( ; it != ite; ++it )
        {
            if ( compare( (*it), node ) )
                child1_bucket.push_back( *it );
            else
                child0_bucket.push_back( *it );
        }
        parent_bucket.clear();
        
    }
    
}; // struct distribution_split



}; //namespace vmml

#endif
