#ifndef __STREAM_PROCESS__BOUNDING_VOLUME__HPP__
#define __STREAM_PROCESS__BOUNDING_VOLUME__HPP__

#include <stream_process/VMMLibIncludes.h>

namespace stream_process
{
template< size_t DIM, typename T >
class mini_bv
{
public:
    static const size_t DIMENSION   = DIM;
    
    typedef T                                   value_type;
    typedef vmml::vector< DIM, value_type >     vector_type;
    
    mini_bv( const vector_type& min_, const vector_type& max_ )
        : _min( min_ )
        , _max( max_ )
    {}
    
    const vector_type& get_aabb_min() const { return _min; }
    const vector_type& get_aabb_max() const { return _max; }
    #if 0
    template< typename query_vector_t >
    query_vector_t  get_aabb_min() const
    {
        query_vector_t min_( 0.0 );
        for( size_t index = 0; index < DIM; ++index )
        {
            min_[ index ] = _min[ index ];
        }
        return min_;
    }
    #endif
    bool is_inside( const vector_type& point ) const
    {
        for( size_t index = 0; index < DIM; ++index )
        {
            if ( point[ index ] < _min[ index ] )
                return false;
            if (  point[ index ] > _max[ index ] )
                return false;
        }
        return true;
    }
    
    template< typename query_vector_t >
    value_type get_squared_min_distance( const query_vector_t& point ) const
    {
        // we want the min_dist between the tree node and the point.
        // init the tmp vec with the point coords and then subtract the 
        // node-min_dist_point before computing the length.

        value_type distance_squared = 0.0;
        for( size_t index = 0; index < DIM; ++index )
        {
            const value_type& min_     = _min[ index ];
            const value_type& max_     = _max[ index ];
            const value_type& pos_     = point[ index ];

            value_type tmp_            = pos_;

            if ( pos_ < min_ )
                tmp_ -= min_;
            else
                tmp_ -= pos_ < max_ ? pos_ : max_;
            
            tmp_ *= tmp_; 
            distance_squared += tmp_;
        }
        return distance_squared;
    }


protected:
    vector_type _min;
    vector_type _max;

};

template< size_t M, typename vector_t >
class bounding_volume
{
public:
    typedef vector_t                        vec;
    typedef typename vector_t::value_type   sp_float_t;

    static const size_t DIMENSION = M;

	bounding_volume( const vec& center_, const vec& radius_vector_ );
	
	const vec&	get_center() const;
	const vec&	get_radius_vector() const;
    
	const vec&	get_aabb_min() const;
	const vec&	get_aabb_max() const;
    
    bool is_inside( const vector_t& point ) const;
    sp_float_t get_squared_min_distance( const vector_t& point ) const;
    sp_float_t get_squared_max_distance( const vector_t& point ) const;

protected:
	void		_update_aabb();
    bool        _nan_test() const;
	
	vec			_center;
	vec			_radius_vector;

	// we store the aabb explicitly. this doubles the size but 
	// saves computations. ( size vs performance )
	vec			_aabb_min;
	vec			_aabb_max;

}; // class bounding_volume



template< size_t M, typename vector_t >
bounding_volume< M, vector_t >::
bounding_volume( const vec& center_,
    const vec& radius_vector_ )
    : _center( 0.0 )
    , _radius_vector( 0.0 )
{
    _center         = 0.0;
    _radius_vector  = 0.0;
    for( size_t index = 0; index < M; ++index )
    {
        _center[ index ]        = center_[index ];
        _radius_vector[ index ] = radius_vector_[ index ];
    }

    assert( _nan_test() );
    _update_aabb();
}



template< size_t M, typename vector_t >
const typename bounding_volume< M, vector_t >::vec&
bounding_volume< M, vector_t >::
get_center() const
{
    return _center;
}


template< size_t M, typename vector_t >
const typename bounding_volume< M, vector_t >::vec&
bounding_volume< M, vector_t >::
get_radius_vector() const
{
    return _radius_vector;
}



template< size_t M, typename vector_t >
const typename bounding_volume< M, vector_t >::vec&
bounding_volume< M, vector_t >::
get_aabb_min() const
{
    return _aabb_min;
}



template< size_t M, typename vector_t >
const typename bounding_volume< M, vector_t >::vec&
bounding_volume< M, vector_t >::
get_aabb_max() const
{
    return _aabb_max;
}



template< size_t M, typename vector_t >
void
bounding_volume< M, vector_t >::
_update_aabb()
{
#if 1
    _aabb_min = _aabb_max = _center;
    _aabb_min -= _radius_vector;
    _aabb_max += _radius_vector;
#else
    _aabb_min = _center - _radius_vector;
    _aabb_max = _center + _radius_vector;
#endif
}



template< size_t M, typename vector_t >
bool
bounding_volume< M, vector_t >::
is_inside( const vector_t& point ) const
{
    return true; //FIXME 
    for( size_t index = 0; index < M; ++index )
    {
        if ( _aabb_min[ index ] > point[ index ] )
            return false;
        if ( _aabb_max[ index ] < point[ index ] )
            return false;
    }
    return true;
}



template< size_t M, typename vector_t >
typename bounding_volume< M, vector_t >::sp_float_t
bounding_volume< M, vector_t >::
get_squared_min_distance( const vector_t& point ) const
{
    // we want the min_dist between the tree node and the point.
    // init the tmp vec with the point coords and then subtract the 
    // node-min_dist_point before computing the length.

    sp_float_t distance_squared = 0.0;
    for( size_t index = 0; index < M; ++index )
    {
        const sp_float_t& min_     = _aabb_min[ index ];
        const sp_float_t& max_     = _aabb_max[ index ];
        const sp_float_t& pos_     = point[ index ];

        sp_float_t tmp_            = pos_;

        if ( pos_ < min_ )
            tmp_ -= min_;
        else
            tmp_ -= pos_ < max_ ? pos_ : max_;
        
        tmp_ *= tmp_; 
        distance_squared += tmp_;
    }
    return distance_squared;
}



template< size_t M, typename vector_t >
typename bounding_volume< M, vector_t >::sp_float_t
bounding_volume< M, vector_t >::
get_squared_max_distance( const vector_t& point ) const
{}



template< size_t M, typename vector_t >
bool
bounding_volume< M, vector_t >::
_nan_test() const
{
    for( size_t index = 0; index < M; ++index )
    {
        if( std::isnan( _center[ index ] ) )
            return false;
        if( std::isnan( _radius_vector[ index ] ) )
            return false;
    }
    return true;
}

} // namespace stream_process

#endif

