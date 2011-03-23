#ifndef PRIORITY2DTREE_H
#define PRIORITY2DTREE_H

/*
 *  Priority2DTree.h
 *  PointProcessing
 *
 *  Created by Renato Pajarola on Tue Nov 18 2003.
 *  Copyright (c) 2003 UC Irvine. All rights reserved.
 *
 */

//#include "stream.h"

#include "StreamProcessing.h"
#include "Pool.h"

#include "stream_point.h"

#include "rt_struct.h"
#include "rt_struct_member.h"

#include <list>

// Cannot be changed if balancing framework is used
#define N_CUTDIMENSIONS 2

namespace stream_process
{

struct NeighbourData
{  
    size_t posOffset;
    size_t countOffset;
    size_t distOffset;
    size_t nblistOffset;
    
    unsigned maxNbCount;
};

class Priority2DTree 
{
public:
    typedef Priority2DTree PTree;
    typedef Pool< PTree > PTreePool;

    Priority2DTree();
    Priority2DTree(const size_t key, stream_point* e, const size_t dimension = 0 );

    // pool compatible post constructor
    void set( const size_t key, stream_point* e, const size_t dimension = 0 );

    void insert(const size_t key, stream_point* e);
    void update(stream_point* e );

    Priority2DTree* remove( Priority2DTree* cur );
    size_t passedRange(const float front, std::list<Priority2DTree*>& L );

    Priority2DTree* rebalance( Priority2DTree* node, const size_t n );

    size_t elements();
    size_t depth();
    size_t balance();

    void Test( const Priority2DTree* root );

    inline uint32_t Priority() { return priority; };
    inline stream_point* Element() { return element; };

    inline Priority2DTree* Parent() { return parent; };
    inline Priority2DTree* Smallest() { return smallest; };

    static void setStaticData( const NeighbourData& nb );

    static rt_struct_member< vec3f > position;
    static rt_struct_member< uint32_t > nbCount;
    static rt_struct_member< float > nbDistances;
    static rt_struct_member< stream_point* > neighbours;

    static size_t maxNeighbours;
  
    static PTreePool& getPool() { return *_pool; };
    static void setPool( PTreePool* pool ) { _pool = pool; };
    
protected:
    void update( stream_point* e, const vec3f& min, const vec3f& max );
    Priority2DTree* remove();

    Priority2DTree* findMin( size_t axis );
    Priority2DTree* findMax( size_t axis );

    void bubbleUp();

    static Priority2DTree* rebalance( Priority2DTree *nodes[], 
        size_t xsorted[], size_t ysorted[], const size_t lo, 
        const size_t hi );

    unsigned char cutdim;
    uint32_t numlo, numup;

    uint32_t priority;
    stream_point* element;  

    Priority2DTree *parent, *lower, *upper;
    Priority2DTree *smallest;

    static PTreePool* _pool;

};

void insertNeighbor(stream_point* e1, float dist, stream_point* e2);

void Qsort( Priority2DTree **nodes, size_t indices[], size_t left, size_t right, size_t cutdim );
int Qpartition( Priority2DTree **nodes, size_t indices[], size_t left, size_t right, size_t cutdim );



} // namespace stream_process

#endif
