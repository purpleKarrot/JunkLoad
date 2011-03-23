/*
 *  NeighborOperator.cpp
 *  StreamProcessing
 *
 *  Created by Renato Pajarola on Mon May 24 2004.
 *  Copyright (c) 2004 UC Irvine. All rights reserved.
 *
 */

#include "kd_heap_neighbor_op.h"
#include "string_utils.h"

#include <cfloat>
#include <utility>

#define MAX_COUNT 8

namespace stream_process
{
/*
* NeighbourOp reads the following inputs from the stream:
*   "position"          - point position                    - \          
*   "point_index"       - point index in stream             - uint32_t          
*
* NeighbourOp writes the following outputs into the stream:
*   "list"             - list of neighbours                - stream_point*[n]   
*   "dist"             - list of neighbour distances       - float[n]          
*   "cnt"              - nb count, number of neighbours    - uint32_t
*   "min_ref_index"    - neighbour with smallest index     - uint32_t
*   "max_ref_index"    - neighbour with largest index      - uint32_t
*/    

kd_heap_neighbor_op::kd_heap_neighbor_op() 
    : stream_op()
    , _heap( 0 )
    , _max_neighbors( MAX_COUNT )
    , max_element( -FLT_MAX )
    , _position( "position" )
    , _minReference( "min_ref_index" )
    , _maxReference( "max_ref_index" )
    , _pointIndex( "point_index" )
    , _neighbours( "list" )
    , _nbCount( "cnt" )
    , _nbDistances( "dist" )
    , _new_neighbors( "neighbors" )
{
    set_name( "kd_heap_neighbor" );
}



kd_heap_neighbor_op::~kd_heap_neighbor_op()
{
    if ( _heap )
    //    delete _heap;
        _pool.destroy( _heap );
}



void 
kd_heap_neighbor_op::pull_push()
{
    stream_point* point;
    Priority2DTree *node;
    IndexReference zref;

    // pull elements from previous operator
    while ( ( point = prev->front() ) ) 
    {
        prev->pop_front();

        vec3f& position = _position << point;
        // update current front, must be in correct z-order for this operator
        #ifdef SPROCESS_OLD_VMMLIB
        assert( max_element <= position.z );
        max_element = position.z;
        #else
        assert( max_element <= position.z() );
        max_element = position.z();
        #endif

        // initialize point and compute self-covariance
        const uint32_t& index = _pointIndex << point;

        uint32_t& min_ref_index = _minReference << point;
        uint32_t& max_ref_index = _maxReference << point;
        uint32_t& count = _nbCount << point;

        min_ref_index = UINT_MAX;
        max_ref_index = 0;
        count = 0;

        // compute k-nearest neighbors and insert new point into heap
        if ( _heap ) 
        {
            _heap->update( point );
            _heap->insert( index,  point );
        } 
        else  
        {
            //_heap = new Priority2DTree( index, point );
            _heap = _pool.create();
            _heap->set( index, point, 0 );
        }

        // update its current smallest referenced index
        stream_point** neighbours =  & _neighbours.get( point );

        uint32_t nbIndex;
        for ( size_t k = 0; k < count; ++k )
        {
            nbIndex = _pointIndex << neighbours[k];

            if ( nbIndex < min_ref_index )
                min_ref_index = nbIndex;
        }

        // update priority queue that maintains smallest referenced index from heap
        zref.first = min_ref_index;	// smallest referenced index
        zref.second = index;			// from element
        ZQ.push( zref );
    }

    // remove nodes beyond range from priority heap
    if ( _heap ) 
    {
        L.clear();
        
        ExtraRef eref;
        size_t i = _heap->passedRange(max_element, L);
        
        while ( ! L.empty() ) 
        {
            node = L.front();
            L.pop_front();
            point = node->Element();

            // remove from heap
            _heap = _heap->remove( node );

            // set smallest and largest referenced indices
            uint32_t& min_ref_index = _minReference << point;
            min_ref_index = UINT_MAX; // reset smallest reference

            uint32_t& max_ref_index     = _maxReference << point;
            const uint32_t count        = _nbCount << point;
            stream_point** neighbours   = point->get_ptr( _neighbours );
            float* distances            = point->get_ptr( _nbDistances );
            
            neighbor* new_nb = point->get_ptr( _new_neighbors );
            
            for ( uint32_t k = 0; k < count; ++k )
            {   /*
                #ifndef NDEBUG
                if ( ! _factory->isValidPtr( neighbours[k] ) )
                    assert( 0 );
                #endif
                */
                uint32_t& nbIndex = _pointIndex << neighbours[k];
                
                new_nb[ k ].first   = distances[ k ];
                new_nb[ k ].second  = neighbours[ k ];
                                
                if ( nbIndex < min_ref_index)
                    min_ref_index = nbIndex;
                else if ( nbIndex > max_ref_index)
                    max_ref_index = nbIndex;
            }

            const uint32_t index = _pointIndex << point;
            
            // update priority queue if necessary
            if ( min_ref_index < ZQ.top().first ) 
            {
                zref.first = min_ref_index;	// smallest referenced index
                zref.second = index;			// from element
                ZQ.push(zref);
            }

            // transfer to sorting buffer
            eref.first = index;
            eref.second = point;
            Buffer.push( eref );
            --i;
        }
        assert( i == 0 && L.empty() );
    }
}


stream_point*  kd_heap_neighbor_op::front()
{
    if ( ! Buffer.empty() 
        && ( !_heap || Buffer.top().first < _heap->Smallest()->Priority() ) )
    {
        return Buffer.top().second;
    }
  return 0;
}



void 
kd_heap_neighbor_op::pop_front()
{
    size_t range;

    if ( ! Buffer.empty() ) 
    {
        range = Buffer.top().first;
        Buffer.pop();
        // remove unused references
        if ( _heap )
          range = std::min( (uint32_t) range, _heap->Smallest()->Priority() );
        // range is the index of the smallest point that hasn't 
        // yet finished processing??
        while ( ! ZQ.empty() && ZQ.top().second < range )
          ZQ.pop();
    }
}



void 
kd_heap_neighbor_op::clear_stage()
{
    stream_point* tmp;
    ExtraRef eref;

    while ( _heap ) 
    {
        tmp = _heap->Smallest()->Element();
        _heap = _heap->remove( _heap->Smallest() );
        
        const uint32_t index = tmp->get( _pointIndex );
        
        eref.first = index;
        eref.second = tmp;
        Buffer.push(eref);
    }
    assert( _heap == NULL );
}



size_t 
kd_heap_neighbor_op::smallest_element()
{
    bool buff = !Buffer.empty();

    if ( buff && _heap )
        return std::min( Buffer.top().first, _heap->Smallest()->Priority() );
    else if ( _heap )
        return _heap->Smallest()->Priority();
    else if ( buff )
        return Buffer.top().first;
    else
        return prev->smallest_element();
        //stream_op::smallest_element();
}



size_t 
kd_heap_neighbor_op::smallest_reference()
{
  if ( ! ZQ.empty() )
    return std::min( (size_t) ZQ.top().first, stream_op::smallest_reference() );
  else
    return stream_op::smallest_reference();
}



size_t 
kd_heap_neighbor_op::size() const
{
    return ( _heap ) ? ( _heap->elements() + Buffer.size() ) : Buffer.size();
}



void 
kd_heap_neighbor_op::setup_stage_0()
{
    // -- required inputs --
    _require( _position );
    _require( _pointIndex );
    
    // -- outputs --
    _reserve( _minReference );
    _reserve( _maxReference );
    _reserve( _nbCount );
    //additional reserves in ::setup()
    
    var_map::iterator it = _config->find( "nb-count" );
    if( it != _config->end() )
        _max_neighbors = (*it).second.as< size_t >();
    else
        _max_neighbors = MAX_COUNT;
    
 }



void 
kd_heap_neighbor_op::setup_stage_1()
{
    var_map::iterator it = _config->find( "nb-count" );
    if( it != _config->end() )
        _max_neighbors = (*it).second.as< size_t >();
    
    LOGINFO << "k (number of neighbors)         " << _max_neighbors << std::endl;
    
    // we cannot do this in the constructor since ^^ (e.g. other ops)
    // might change _max_neighbors
    stream_op::_reserve_custom_type( _neighbours, SP_UNKNOWN_DATA_TYPE, 
        sizeof( stream_point* ), _max_neighbors );
    
    stream_op::_reserve_custom_type( _nbDistances, SP_UNKNOWN_DATA_TYPE, 
        sizeof( float ), _max_neighbors );

    // reserve the memory for the neighbor distance-pointer pairs.
    stream_op::_reserve_custom_type( _new_neighbors, SP_UNKNOWN_DATA_TYPE, 
        sizeof( std::pair< float, stream_point* > ), _max_neighbors );
}



void 
kd_heap_neighbor_op::setup_stage_2()
{
    NeighbourData nb;
    nb.posOffset    = _position.get_offset();
    nb.countOffset  = _nbCount.get_offset();
    nb.distOffset   = _nbDistances.get_offset();
    nb.nblistOffset = _neighbours.get_offset();
    nb.maxNbCount   = _max_neighbors;

    Priority2DTree::setStaticData( nb );
    Priority2DTree::setPool( &_pool );
}



kd_heap_neighbor_op_options::kd_heap_neighbor_op_options()
    : op_options( "K-d-heap neighbor")
{
    _op_options.add_options()
        ( "nb-count,k", 
            boost::program_options::value< size_t >()->default_value( MAX_COUNT ),
            "Specify the value of k.\n"
            "  k is the number of neighbors used in the k-nearest neighbor search. Please note that some operators might override this setting." )
        ;
    _pos_op_options.add( "nb-count", 1 );
}


} // namespace stream_process



