#ifndef __STREAM_PROCESS__POINT_REGION_KD_TREE__HPP__
#define __STREAM_PROCESS__POINT_REGION_KD_TREE__HPP__

#include <stream_process/tree_functors.hpp>
#include <stream_process/bounding_volume.hpp>
#include <cstddef>

namespace stream_process
{

template< 
	size_t DIMENSION,
	typename vector_t,
	typename payload_t              = vector_t,
	typename position_accessor_t    = passthrough_accessor< vector_t >,
	typename bucket_t				= std::vector< payload_t >,
    typename bounding_volume_t      = bounding_volume< DIMENSION, vector_t >
    >
class point_region_kd_tree : public bounding_volume_t
{
public:
	typedef vector_t                        vec;
    typedef bounding_volume_t               bv;
    typedef typename vector_t::value_type   sp_float_t;
    
    typedef bucket_t                        bucket_type;
    
    const static size_t M = 2; // number of child nodes
    
    // root node ctor
    point_region_kd_tree( const vec& center_, const vec& radius_vector_,
        const position_accessor_t& accessor_ )
        : bounding_volume_t( center_, radius_vector_ )
        , _split_axis( 0 )
        , _accessor( accessor_ )
		, _position_flag( 0 )
		, _depth( 0 )
    {}

    // child node ctor
    point_region_kd_tree( const point_region_kd_tree& parent, size_t child_num )
        : bounding_volume_t( parent._center, parent._radius_vector)
        , _split_axis( _get_split_axis( parent ) )
        , _accessor( parent._accessor )
		, _position_flag( parent._position_flag )
		, _depth( parent._depth + 1 )
    {
        size_t parent_axis = parent._split_axis;
        
        bv::_radius_vector[ parent_axis ] *= 0.5;

        if ( child_num == 0 )
            bv::_center[ parent_axis ] -= bv::_radius_vector[ parent_axis ];
        else
		{
            bv::_center[ parent_axis ] += bv::_radius_vector[ parent_axis ];
			_position_flag |= 1 << ( _depth - 1 );
        }    
        bv::_update_aabb();
#if 0
        assert( bv::_center - bv::_radius_vector == bv::_aabb_min );
        assert( bv::_center + bv::_radius_vector == bv::_aabb_max );
        for( size_t index = 0; index < DIMENSION; ++index )
        {
            assert( bv::_aabb_min[ index ] >= parent._aabb_min[ index ] );
            assert( bv::_aabb_max[ index ] <= parent._aabb_max[ index ] );
        }
#endif
		assert( parent._bucket.size() != 0 );
    }
    
    inline size_t find_child_index( const payload_t& payload )
    {
        const vec& position = _accessor( payload );

        assert( is_inside( position ) );

        return position[ _split_axis ] < bv::_center[ _split_axis ] ? 0 : 1;
    }
    
    inline size_t get_child_index( size_t parent_index, size_t child_number ) const
    {
        return ( parent_index << 1 ) + child_number;
    }
    
    inline size_t get_parent_index( size_t index_ ) const
    {
        return index_ >> 1;
    }
    
    inline size_t get_root_index() const
    {
        return 1;
    }

protected:
    inline size_t _get_split_axis( const point_region_kd_tree& parent )
    {
        #if 0
        size_t largest = 0;
        for( size_t index = 1; index != DIMENSION; ++index )
        {
            if ( bv::_radius_vector[ index ] > bv::_radius_vector[ largest ] )
                largest = index;
        }
        return largest;
        #else
        return ( parent._split_axis + 1 ) % DIMENSION;
        #endif
    }
    
    uint8_t                     _split_axis;
    const position_accessor_t&  _accessor;
	bucket_t					_bucket;

	uint16_t					_position_flag;
	uint16_t					_depth;
	
}; // class point_region_kd_tree

} // namespace stream_process

#endif

