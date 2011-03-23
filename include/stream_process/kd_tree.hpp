#ifndef __STREAM_PROCESS__KD_TREE__HPP__
#define __STREAM_PROCESS__KD_TREE__HPP__

#include <stream_process/foreach.hpp>

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
class kd_tree : public bounding_volume_t
{
public:
	typedef vector_t                        vec;
    typedef bounding_volume_t               bv;
    typedef typename vector_t::value_type   sp_float_t;
    
    const static size_t M = 2; // number of child nodes
    
    // root node ctor
    kd_tree( const vec& center_, const vec& radius_vector_,
        const position_accessor_t& accessor_ )
        : bounding_volume_t( center_, radius_vector_ )
        , _split_axis( 0 )
        , _accessor( accessor_ )
    {
    }

    // child node ctor
    kd_tree( const kd_tree& parent, size_t child_num )
        : bounding_volume_t( parent._center, parent._radius_vector)
        , _split_axis( 0 )
        , _accessor( parent._accessor )
    {
		const bucket_t&	bucket_ = parent._bucket;
		_split_axis = bv::_radius_vector.getLargestComponentIndex();
		sp_float_t cz = 0.0;
		sp_float_t aabb_min_z = bv::_aabb_min[ _split_axis ];
		foreach( const payload_t& p, bucket_ )
		{
			const vec& position = _accessor( p );
			const sp_float_t z = position[ _split_axis ] - aabb_min_z;
			cz += z;
		}
		// FIXME compute only in one child
		cz /= static_cast< sp_float_t >( bucket_.size() );
		const size_t index = _split_axis;
		if ( child_num == 0 )
		{
			bv::_aabb_max[ index ] = bv::_aabb_min[ index ] + cz;
			bv::_center[ index ] = bv::_aabb_min[ index ] + cz * 0.5;
		}
		else
		{
			bv::_aabb_min[ index ] += cz;
			bv::_center[ index ] = bv::_aabb_min[ index ] 
				+ ( bv::_aabb_max[ index ] - bv::_aabb_min[ index ] ) * 0.5;
		}
    }
    
    inline size_t find_child_index( const payload_t& payload )
    {
        const vec& position = _accessor( payload );

        assert( is_inside( position ) );

        return position[ _split_axis ] < bv::_center[ _split_axis ] ? 0 : 1;
    }
    

protected:
    const position_accessor_t&  _accessor;
	bucket_t					_bucket;
	uint8_t						_split_axis;
    

}; // class kd_tree

} // namespace stream_process

#endif

