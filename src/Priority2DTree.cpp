/*
 *  Priority2DTree.cpp
 *  PointProcessing
 *
 *  Created by Renato Pajarola on  2003.
 *  Copyright (c) 2003 UC Irvine. All rights reserved.
 *
 */

#include <assert.h>
#include <float.h>

#include "Priority2DTree.h"

// debug
#include "rt_struct_factory.h"
//debug end


namespace stream_process
{

rt_struct_member< vec3f > Priority2DTree::position;
rt_struct_member< uint32_t > Priority2DTree::nbCount;
rt_struct_member< float > Priority2DTree::nbDistances;
rt_struct_member< stream_point* > Priority2DTree::neighbours;

size_t Priority2DTree::maxNeighbours = 0;

Priority2DTree::PTreePool* Priority2DTree::_pool = 0;

Priority2DTree::Priority2DTree()
{
    set( 0, 0, 0 );
}

Priority2DTree::Priority2DTree( const size_t key, stream_point* e, 
                                const size_t dimension )
{
    set( key, e, dimension );
}

void Priority2DTree::set( const size_t key, stream_point* e, const size_t dimension )
{
    assert( dimension >= 0 && dimension < N_CUTDIMENSIONS );

    priority = key;
    element = e;
    cutdim = dimension;

    numlo = 0;
    numup = 0;

    parent = lower = upper = 0;
    smallest = this;
}


void Priority2DTree::insert( const size_t key, stream_point* e )
{
    size_t tmppriority;
    stream_point* tmpelement;
    Priority2DTree *cur, *tree;

    // insert in sorted order
    vec3f& ePos = position.get( e );
    vec3f& elePos = position.get( element ); 

    //assert( !(*element == *e) );
    assert( ! ( ePos == elePos ) );

    if ( ePos[cutdim] > elePos[cutdim]) 
    {
        if (upper) 
        {
          upper->insert(key, e);
        } 
        else 
        {
            upper = _pool->create();
            upper->set( key, e, (cutdim+1) % N_CUTDIMENSIONS );
            upper->parent = this;
        }
        numup++;
        assert( numup == upper->numlo + upper->numup + 1 );   
        
        if ( upper->smallest->priority < smallest->priority )
            smallest = upper->smallest;
    } 
    else 
    {
        if ( lower ) 
        {
            lower->insert(key, e);
        } 
        else 
        {
            lower = _pool->create();
            lower->set( key, e, (cutdim+1) % N_CUTDIMENSIONS );
            lower->parent = this;
        }
        numlo++;       
        assert( numlo == lower->numlo + lower->numup + 1 );
        if ( lower->smallest->priority < smallest->priority )
          smallest = lower->smallest;
    }
/*
  if (numlo > 50 && MAX(lower->numlo, lower->numup) / MIN(lower->numlo, lower->numup) > 2.0) {
    lower = rebalance(lower, numlo);
    lower->parent = this;
    bubbleUp();
    assert( numlo == lower->elements() );
  }
  if (numup > 50 && MAX(upper->numlo, upper->numup) / MIN(upper->numlo, upper->numup) > 2.0) {
    upper = rebalance(upper, numup);
    upper->parent = this;
    bubbleUp();
    assert( numup == upper->elements() );
  }
  return;
*/
  // check for balance (a rough heuristic)
    if ( numlo > 2 * numup ) 
    {
        // find a replace split key
        assert( lower );
        cur = lower->findMax( cutdim );
        assert( cur );

        vec3f& cpos = position.get( cur->element );

        if ( cpos[cutdim] == elePos[cutdim])
            return;
        tmpelement = element;
        tmppriority = priority;
        element = cur->element;
        priority = cur->priority;
        
        // recursively remove replaced key's node
        assert( cur->parent );
        tree = cur->parent;
        if ( tree->lower == cur )
            tree->lower = cur->remove();
        else if ( tree->upper == cur )
            tree->upper = cur->remove();
        else
          exit(1);
        insert(tmppriority, tmpelement);
    }
}


void Priority2DTree::update( stream_point* e)
{
    static const vec3f min = -FLT_MAX; 
    static const vec3f max = FLT_MAX;
    update(e, min, max);
}


void Priority2DTree::update( stream_point* e, const vec3f& pMin, const vec3f& pMax )
{
    assert( e );
       
    vec3f& ePos = position.get( e );
    vec3f& elePos = position.get( element );
    const uint32_t count = nbCount.get( e );

    //assert( !( *element == *e ) );
    assert( ! ( elePos == ePos ) );  
    assert( pMin[cutdim] <= elePos[cutdim] );
    assert( elePos[cutdim] <= pMax[cutdim] );

    // check if this area may contribute to a new k-nearest neighbor
    if ( count >= maxNeighbours ) 
    {
        float dist = nbDistances.get( e, count - 1 );

        float low, high;
        for ( size_t i = 0; i < N_CUTDIMENSIONS; ++i ) 
        {
            low = pMin[i] - ePos[i];
            high = ePos[i] - pMax[i];
            if ( ( low > 0 && low * low >= dist ) 
              || ( high > 0 && high * high >= dist ) )
            {
                return;
            }
        }
    }

    unsigned char cd = cutdim;
    assert( cd < N_CUTDIMENSIONS );
    float cdDist = ePos[cd] - elePos[cd];

    vec3f Min( pMin );
    vec3f Max( pMax );
    
    if ( cdDist > 0 ) 
    {
        if (upper) 
        {
              Min[cd] = elePos[cd];
              upper->update( e, Min, Max);
              Min[cd] = pMin[cd];
        }
        if ( lower ) 
        {
            Max[cd] = elePos[cd];
            lower->update( e, Min, Max );
            Max[cd] = pMax[cd];
        }
    } 
    else 
    {
        if ( lower ) 
        {
            Max[cd] = elePos[cd];
            lower->update( e, Min, Max );
            Max[cd] = pMax[cd];
        }
        if ( upper ) 
        {
            Min[cd] = elePos[cd];
            upper->update( e, Min, Max );
            Min[cd] = pMin[cd];
        }
  }

    // update mutually k-nearest neighbors
    vec3f distvec = ePos - elePos;
    float dist = distvec.lengthSquared();
    //dist = squareDistance(e->v, element->v);
    insertNeighbor( e, dist, element );
    insertNeighbor( element, dist, e );

    assert( nbCount << e && nbCount << element );
    //assert( e->cnt );
    //assert( element->cnt );
}


size_t Priority2DTree::passedRange( const float front, std::list<Priority2DTree*>& L )
{
    assert( smallest );
    stream_point* sElement = smallest->element;
    size_t n = 0;

    #ifdef SPROCESS_OLD_VMMLIB
    const float val = position.get( sElement ).z; // [2]
    #else
    const float val = position.get( sElement ).z(); // [2]
    #endif

    const uint32_t count = nbCount.get( sElement );

    // FIXME???
    if ( count == 0 ) 
        return 0;
    
    const float sedist = nbDistances.get( sElement, count - 1 );

    float dist = (front - val) * (front - val);
    if ( dist < sedist )
        return 0;
    else if ( count >= maxNeighbours ) 
    {
        assert( smallest->smallest );
        assert( smallest->parent || smallest == this );
        L.push_back(smallest);
        ++n;
    }

    // Caution: only add elements in separate subtrees to list L
    if (smallest == this && n)
        return n;

    if ( lower && lower->smallest != smallest )
        n += lower->passedRange(front, L);
    if ( upper && upper->smallest != smallest )
        n += upper->passedRange(front, L);
    return n;
}

// PRECONDITION: *this must be root
Priority2DTree* Priority2DTree::remove( Priority2DTree* cur )
{
    Priority2DTree* tmptree;
    assert( parent == NULL );          // only call on root of PriorityHeap
    // remove root node
    if ( cur == this )
        return remove();

    // remove other nodes
    assert( cur->parent );
    tmptree = cur->parent;
    if ( tmptree->lower == cur )
        tmptree->lower = cur->remove();
    else if ( tmptree->upper == cur )
        tmptree->upper = cur->remove();
    else
        exit(1); // FIXME??
    return this;
}


Priority2DTree* Priority2DTree::remove()
{
    Priority2DTree *cur, *par;

    // remove and delete node if leaf node
    if (!lower && !upper) 
    {
        assert( smallest == this );
        assert( numup == 0 );
        assert( numlo == 0 );
        // smallest indicates if a node is 'under deletion'
        smallest = 0;
        if (parent)
            parent->bubbleUp();
        parent = 0;
        _pool->destroy( this );
        return 0;
    }

    // replace split key
    if ( lower ) 
    {
        cur = lower->findMax(cutdim);
    } 
    else 
    {
        // findMin in upper would be great but duplicates in upper sub-tree makes this impossible
        cur = upper->findMax(cutdim);
        lower = upper;
        upper = 0;
    }
    // completely exchange the two nodes
    assert( cur );
    assert( cur->parent );
    par = cur->parent;

    element = cur->element;
    priority = cur->priority;

    // recursively remove replaced key's node
    if ( par->lower == cur )
        par->lower = cur->remove();
    else if ( par->upper == cur )
        par->upper = cur->remove();
    else
        exit(1); // FIXME??
    return this;
}


Priority2DTree* Priority2DTree::findMin( size_t axis )
{
    Priority2DTree *lkey = 0, *ukey = 0;
    float value, lvalue = FLT_MAX, uvalue = FLT_MAX;
    assert( axis >= 0 && axis < N_CUTDIMENSIONS );

    if ( axis == cutdim ) 
    {
        if ( lower )
            return lower->findMin(axis);
        else
            return this;
    } 
    else 
    {
        if ( lower ) 
        {
            lkey = lower->findMin(axis);
            lvalue = position.get( lkey->element )[axis];
        }
        if ( upper ) 
        {
            ukey = upper->findMin(axis);
            uvalue = position.get( ukey->element )[axis];
        }

        value = position.get( element )[axis];

        if ( value <= lvalue && value <= uvalue )
            return this;
        else if ( lvalue < value && lvalue <= uvalue )
            return lkey;
        else if ( uvalue < value && uvalue < lvalue )
            return ukey;
        else
            exit(1); // FIXME???
    }
}


Priority2DTree* Priority2DTree::findMax( size_t axis )
{
    assert( axis < N_CUTDIMENSIONS );
    
    if ( axis == cutdim ) 
    {
        if ( upper )
            return upper->findMax( axis );
        else
            return this;
    } 

    Priority2DTree* maxtree = this;

    float maxvalue = position.get( element )[axis];

    if ( lower ) 
    {
        Priority2DTree* lkey = lower->findMax(axis);
        float lowerval = position.get( lkey->element )[axis];
        if ( lowerval > maxvalue )
        {
            maxvalue = lowerval;
            maxtree = lkey;
        }
    }
    if ( upper ) 
    {
        Priority2DTree* ukey = upper->findMax(axis);
        float upperval = position.get( ukey->element )[axis];
        if ( upperval > maxvalue )
        {
            return ukey;
        }
    }
    return maxtree;
}


void Priority2DTree::bubbleUp()
{
    // previous values
    Priority2DTree* tmp = smallest;
    size_t lo = numlo;
    size_t hi = numup;

    // adjust heap values
    smallest = this;
    numup = 0;
    if ( upper && upper->smallest ) 
    {
        numup = upper->numlo + upper->numup + 1;
        if ( upper->smallest->priority < smallest->priority )
            smallest = upper->smallest;
    }
    numlo = 0;
    if ( lower && lower->smallest ) 
    {
        numlo = lower->numlo + lower->numup + 1;
        if ( lower->smallest->priority < smallest->priority )
            smallest = lower->smallest;
    }
    // continue if node has changed
    if ( parent && ( tmp != smallest || lo != numlo || hi != numup ) )
        parent->bubbleUp();
}


size_t Priority2DTree::elements()
{
#ifndef NDEBUG
    size_t s, t;
#endif 
    if ( lower ) 
    {
        assert( lower->parent == this );
        assert( numlo == (s = lower->elements()) );
    }
    if ( upper ) 
    {
        assert( upper->parent == this );
        assert( numup == ( t = upper->elements() ) );
    }
    return numlo+numup+1;
}


size_t Priority2DTree::depth()
{
    size_t s = 0, t = 0;
    if ( lower ) 
    {
        assert( lower->parent == this );
        s = lower->depth();
    }
    if ( upper ) 
    {
        assert( upper->parent == this );
        t = upper->depth();
    }
    if ( s > t )
        return s+1;
    else
        return t+1;
}


size_t Priority2DTree::balance()
{
    size_t s = 0, t = 0;
    if ( lower ) 
    {
        assert( lower->parent == this );
        s = lower->depth();
    }
    if ( upper ) 
    {
        assert( upper->parent == this );
        t = upper->depth();
    }
    return t-s;
}


Priority2DTree* Priority2DTree::rebalance(Priority2DTree* node, const size_t n)
{
    Priority2DTree* tree;

#ifndef NDEBUG
    size_t test;
    assert( test = node->elements() );
#endif

    // assemble all nodes breadth-first
    // FIXME, pool???
    Priority2DTree** nodes = (Priority2DTree**)calloc(n, sizeof(Priority2DTree*));
    nodes[0] = node;
    size_t k = 1, j = 1;
    size_t i = 0;
    // FIXME check with renato if i=0 and then multiple use of changed 
    // i is what's supposed to happen 
    while ( j < n ) 
    {
        for ( ; i < k; ++i ) 
        {
            if ( nodes[i]->lower )
                nodes[j++] = nodes[i]->lower;
            if ( nodes[i]->upper )
                nodes[j++] = nodes[i]->upper;
            nodes[i]->parent = nodes[i]->lower = nodes[i]->upper = nodes[i]->smallest = 0;
            nodes[i]->numlo = nodes[i]->numup = 0;
        }
        k = j;
    }
    for ( ; i < k; ++i ) 
    {
        nodes[i]->parent = nodes[i]->lower = nodes[i]->upper = nodes[i]->smallest = 0;
        nodes[i]->numlo = nodes[i]->numup = 0;
    }

    // generate sorted list of indices
    // FIXME Pool
    size_t* xsorted = ( size_t* )calloc( n, sizeof(int) );
    size_t* ysorted = ( size_t* )calloc( n, sizeof(int) );
    for ( i = 0; i < n; ++i)
        xsorted[i] = ysorted[i] = i;
  
    Qsort( nodes, xsorted, 0, n-1, 0 );
    Qsort( nodes, ysorted, 0, n-1, 1 );

    // perform rebalancing step
    tree = rebalance( nodes, xsorted, ysorted, 0, n );
    assert( test == tree->elements() );
  
    free( nodes );
    free( xsorted );
    free( ysorted );

    tree->Test(0);
    return tree;
}


Priority2DTree* Priority2DTree::rebalance(Priority2DTree *nodes[], 
                                size_t xsorted[], size_t ysorted[], 
                                const size_t lo, const size_t hi)
{
    unsigned char cd = 0;
    size_t i, k, l, median, m1, m2, *indices, *swap, *array=NULL;
    float range, tmp;

#ifndef NDEBUG
    assert( lo >= 0 );
    assert( hi > lo );

    for ( i = lo; i < hi-1; ++i ) 
    {
        // FIXME
        //const float xi = nodes[xsorted[i]]->element->read< float >( positionOffset );
        //const float xi1 = nodes[xsorted[i+1]]->element->read< float >( positionOffset );
        //const float yi = nodes[ysorted[i]]->element->getFloatFromArray( positionOffset, 1 );
        //const float yi1 = nodes[ysorted[i+1]]->element->getFloatFromArray( positionOffset, 1 );
        //assert( xi <= xi1 );
        //assert( yi <= yi1 );
        assert( ( position << nodes[xsorted[i]]->element )[0] <= ( position << nodes[xsorted[i+1]]->element )[0] ); 
        assert( ( position << nodes[ysorted[i]]->element )[1] <= ( position << nodes[ysorted[i+1]]->element )[1] ); 

        //assert( nodes[xsorted[i]]->element->v[0] <= nodes[xsorted[i+1]]->element->v[0] );
        //assert( nodes[ysorted[i]]->element->v[1] <= nodes[ysorted[i+1]]->element->v[1] );
    }
#endif

    if ( hi == lo + 1 ) 
    {
        // leaf node reached
        assert( xsorted[lo] == ysorted[lo] );

        //nodes[xsorted[lo]]->cutdim = (nodes[xsorted[lo]]->parent->cutdim+1) % N_CUTDIMENSIONS;
        nodes[xsorted[lo]]->cutdim = 0;
        assert( nodes[xsorted[lo]]->numlo == 0 );
        assert( nodes[xsorted[lo]]->numup == 0 );
        assert( nodes[xsorted[lo]]->lower == NULL );
        assert( nodes[xsorted[lo]]->upper == NULL );
        nodes[xsorted[lo]]->smallest = nodes[xsorted[lo]];

        return nodes[xsorted[lo]];
    }

    // assumes TWO cutdimensions exactly
    indices = NULL;
    range = 0.0;
    
    tmp = position.get( nodes[xsorted[hi-1]]->element )[0] 
            - position.get( nodes[xsorted[lo]]->element )[0]; 
    
    if (tmp > range) 
    {
        cd = 0;
        indices = xsorted;
        array = ysorted;
        range = tmp;
    }
    
    tmp = position.get( nodes[ysorted[hi-1]]->element ).y() //[1] 
            - position.get( nodes[ysorted[lo]]->element ).y(); //[1]; 

    if ( tmp > range) 
    {
        cd = 1;
        indices = ysorted;
        array = xsorted;
        range = tmp;
    }
    assert( indices );
    if ( range == 0.0 ) 
    {
        // found identical vertices
       std::cout << "Priority2DTree::rebalance - found identical vertices. Aborting..." <<std::endl;
        exit(2); // FIXME??
    }

    // find median element
    median = (lo + hi) / 2;
    assert( median > lo );
    assert( median < hi );

    m1 = median;
    while ( m1 > lo 
            && position.get( nodes[indices[m1]]->element )[cd] 
               == position.get( nodes[indices[m1-1]]->element )[cd] 
            )
    {
        --m1;
    }
    --m1;
    m2 = median;
    while ( m2 < hi - 1
            && position.get( nodes[indices[m2]]->element )[cd] 
                    == position.get( nodes[indices[m2+1]]->element )[cd] 
            )
    {
        ++m2;
    }
    assert( m1 > lo || m2 < hi );
    if ( m1 - lo >= hi - m2 )
        median = m1;
    else
        median = m2;

  // FIXME pool?

  // separate indices in other array into smaller-or-equal and greater sets
    swap = (size_t*)calloc(hi-lo, sizeof(size_t));
    l = 0;
    k = lo;
    for ( i = lo; i < hi; i++ ) 
    {
        if ( position.get( nodes[array[i]]->element )[cd] > position.get( nodes[indices[median]]->element) [cd] )
        {
            swap[l++] = array[i];
        }
        else if (array[i] != indices[median])	
        { // skip median element since consumed by this node
            array[k++] = array[i];
        }
    }
    assert( k == median );
    array[k++] = indices[median];
    assert( k+l == hi );
    for (i = 0; i < l; i++) 
    {
        array[k] = swap[i];
        k++;
    }
    assert( k == hi );
    assert( array[median] == indices[median] );
    
    // FIXME pool?
    free(swap);

    // set values for this node
    nodes[indices[median]]->cutdim = cd;

    if (median > lo) 
    {
        nodes[indices[median]]->lower = rebalance(nodes, xsorted, ysorted, lo, median);
        nodes[indices[median]]->lower->parent = nodes[indices[median]];
        nodes[indices[median]]->numlo = nodes[indices[median]]->lower->numlo + nodes[indices[median]]->lower->numup + 1;
        assert( nodes[indices[median]]->numlo == median-lo );
    }

    if (hi > median+1) 
    {
        nodes[indices[median]]->upper = rebalance(nodes, xsorted, ysorted, median+1, hi);
        nodes[indices[median]]->upper->parent = nodes[indices[median]];
        nodes[indices[median]]->numup = nodes[indices[median]]->upper->numlo + nodes[indices[median]]->upper->numup + 1;
        assert( nodes[indices[median]]->numup == hi-median-1 );
    }
    assert( nodes[indices[median]]->numlo + nodes[indices[median]]->numup + 1 == hi - lo );

    // set heap values
    nodes[indices[median]]->smallest = nodes[indices[median]];
    if (nodes[indices[median]]->upper &&
      nodes[indices[median]]->upper->smallest->priority < nodes[indices[median]]->smallest->priority)
    {
        nodes[indices[median]]->smallest = nodes[indices[median]]->upper->smallest;
    }
    if (nodes[indices[median]]->lower &&
      nodes[indices[median]]->lower->smallest->priority < nodes[indices[median]]->smallest->priority)
    {
        nodes[indices[median]]->smallest = nodes[indices[median]]->lower->smallest;
    }

    return nodes[indices[median]];
}


void Priority2DTree::Test(const Priority2DTree *root)
{
#ifndef NDEBUG
    size_t s, t;
#endif

    if ( root )
        assert( parent || this == root );
    assert( smallest );
    assert( smallest->priority <= priority );

    if ( upper ) 
    {
        assert( upper->parent == this );
        assert( numup == (t = upper->elements()) );

        assert( smallest->priority <= upper->priority );
        assert( smallest->priority <= upper->smallest->priority );

    //assert( ((cutdim+1) % N_CUTDIMENSIONS) == upper->cutdim );
        //assert( element->v[cutdim] < upper->element->v[cutdim] );
        //assert( element->v[cutdim] < upper->smallest->element->v[cutdim] );
        //assert( element->getFloatFromArray( positionOffset, cutdim ) < upper->element->getFloatFromArray( positionOffset, cutdim ) ); 
        //assert( element->getFloatFromArray( positionOffset, cutdim ) < upper->smallest->element->getFloatFromArray( positionOffset, cutdim ) );
        assert( ( position << element )[cutdim] < ( position << upper->element )[cutdim] );
        assert( ( position << element )[cutdim] < ( position << upper->smallest->element )[cutdim] );   

        upper->Test(root);
    }

    if ( lower ) 
    {
        assert( lower->parent == this );
        assert( numlo == (s = lower->elements()) );

        assert( smallest->priority <= lower->priority );
        assert( smallest->priority <= lower->smallest->priority );

    //assert( ((cutdim+1) % N_CUTDIMENSIONS) == lower->cutdim );
        //assert( element->v[cutdim] >= lower->element->v[cutdim] );
        //assert( element->v[cutdim] >= lower->smallest->element->v[cutdim] );
        //assert( element->getFloatFromArray( positionOffset, cutdim ) >= lower->element->getFloatFromArray( positionOffset, cutdim ) ); 
        //assert( element->getFloatFromArray( positionOffset, cutdim ) >= lower->smallest->element->getFloatFromArray( positionOffset, cutdim ) );
        assert( ( position << element )[cutdim] < ( position << lower->element )[cutdim] );
        assert( ( position << element )[cutdim] < ( position << lower->smallest->element )[cutdim] );      

        lower->Test( root );
    }
}

void Priority2DTree::setStaticData( const NeighbourData& nb )
{
    position.set_offset( nb.posOffset );
    nbCount.set_offset( nb.countOffset );
    nbDistances.set_offset( nb.distOffset );
    neighbours.set_offset( nb.nblistOffset );
    maxNeighbours = nb.maxNbCount;
} 


void insertNeighbor( stream_point* e1, float dist, stream_point* e2)
{ 
    size_t k, l, r;
    uint32_t& e1Count         = Priority2DTree::nbCount.get( e1 ); 
    float* e1Dists       = & Priority2DTree::nbDistances.get( e1 );
    stream_point** e1Points = & Priority2DTree::neighbours.get( e1 );

    if ( e1Count ) 
    { // find index k pointing to next farther element
        if ( e1Count < 8 ) //&& dist < e1->dist[e1->cnt-1]) {
        { // do linear search
            for (k = 0; k < e1Count && dist >= e1Dists[k]; ++k )
            //e1->dist[k]; k++)
            {
                if ( e1Points[k] == e2 )
                    return;
            }
        } 
        else 
        { //if (dist < e1->dist[e1->cnt-1]) {
            // do binary search
            l = 0;
            r = e1Count; //e1->cnt;
            k = ( l + r ) / 2;
            while (l < r) 
            {
                if (dist < e1Dists[k] ) //e1->dist[k])
                    r = k;
                else
                    l = k+1;
                k = ( l + r ) / 2;
            }
            if ( e1Points[k] == e2 ) //e1->list[k] == e2 )
                return;
        }

        if ( k >= Priority2DTree::maxNeighbours ) 
        {
            return;
        } 
        else if ( k < e1Count ) //e1->cnt ) 
        { // new k-nearest, shift others back
            for ( l = std::min( (size_t) e1Count, Priority2DTree::maxNeighbours -1 ); l > k; --l ) 
            {
                e1Points[l] = e1Points[l-1];
                e1Dists[l] = e1Dists[l-1];                
            }
        } 
    } 
    else 
    {
        // element has no neighbors yet
        assert( e1Count == 0 ); // e1->cnt == 0 );
        k = 0;
    }
    e1Points[k] = e2;
    e1Dists[k] = dist;
    if ( e1Count < Priority2DTree::maxNeighbours )
        ++e1Count;
}

void Qsort( Priority2DTree **nodes, size_t indices[], size_t left, 
            size_t right, size_t cutdim )
{
    size_t pivot;
    if ( left < right ) 
    {
        pivot = Qpartition( nodes, indices, left, right, cutdim );
        Qsort( nodes, indices, left, pivot, cutdim );
        Qsort( nodes, indices, pivot+1, right, cutdim );
    }
}

int Qpartition( Priority2DTree **nodes, size_t indices[], size_t left, 
                size_t right, size_t cutdim )
{
    size_t tmp;
    float pivot;

    // change pivot element to random within left and right
    size_t j = left + ( random() & ( right-left ) );
    assert( left <= j && j <= right );
    tmp = indices[left];
    indices[left] = indices[j];
    indices[j] = tmp;

    pivot = Priority2DTree::position.get( nodes[indices[left]]->Element() )[cutdim];
    size_t i = left;
    j = right;
    while (1) 
    {
        while ( Priority2DTree::position.get( nodes[indices[j]]->Element() )[cutdim] > pivot )
  
        {
            j--;
        }
        while (  Priority2DTree::position.get( nodes[indices[i]]->Element() )[cutdim] < pivot )
        {
            i++;
        }
        assert( left <= i && i <= right );
        assert( left <= j && j <= right );
        if ( i < j ) 
        {
            // index information
            tmp = indices[i];
            indices[i] = indices[j];
            indices[j] = tmp;
            // skip these two elements
            j--;
            i++;
        } 
        else 
        {
            return j;
        }
    }
}


} // namespace stream_process
