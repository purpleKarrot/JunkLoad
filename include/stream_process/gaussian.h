#ifndef GAUSSIAN_H
#define GAUSSIAN_H

/*
 *  gaussian.h
 *  PointProcessing
 *
 *  Created by Renato Pajarola on Fri Jan 16 2004.
 *  Copyright (c) 2003 UC Irvine. All rights reserved.
 *
 */


//float evaluateGauss(float radius, float var);

#include "StreamProcessing.h"

#include <vector>

namespace stream_process
{

template < typename T >
class Gaussian 
{
public:
    Gaussian( T variance, size_t subdivisions = 256 );
    ~Gaussian();

    // PRECONDITION: r is already squared
    T evaluate( T r );
    
    const std::vector< float >& getTable() const;

protected:
    size_t _subdiv; // int n
    T _variance;
    T _varSquared;
    T _fourTimesVarSquared;
    T _invDelta;
    std::vector< T > _table;
};



template < typename T >
Gaussian< T >::Gaussian( T variance, size_t subdivisions )
    : _subdiv ( subdivisions )
    , _variance ( variance )
    , _varSquared( _variance * _variance )
    , _fourTimesVarSquared( 4.0 * _varSquared )
    , _invDelta()
{
    T delta = 4.0 * _varSquared / _subdiv;
    _invDelta = 1.0 / delta;
    
    _table.resize( _subdiv );

    // record values over squared range
    for ( size_t i = 0; i < _subdiv; ++i )
    {
        T r = i * delta;
        // normalized to 1.0 in center
        _table[i] = exp( -0.5 * r / _varSquared );
    }
}




template < typename T >
Gaussian< T >::~Gaussian()
{}



template < typename T >
const std::vector< float >&
Gaussian< T >::getTable() const
{
    return _table;
}



template < typename T >
T Gaussian< T >::evaluate( T r )
{
    // assumes squared r
    return ( r < _fourTimesVarSquared ) 
        ? _table[ (size_t) ( r * _invDelta ) ] : 0.0f;
}



} // namespace stream_process

#endif
