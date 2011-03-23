#ifndef __STREAM_PROCESS__QUAD_TREE__HPP__
#define __STREAM_PROCESS__QUAD_TREE__HPP__

#include <stream_process/VMMLibIncludes.h>
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
class quad_tree : public bounding_volume_t
{
public:
	typedef vector_t                        vec;
    typedef bounding_volume_t               bv;
    typedef typename vector_t::value_type   float_t;
    
    const static size_t M = 1 << DIMENSION ; // number of child nodes
    
    // root node ctor
    quad_tree( const vec& center_, const vec& radius_vector_, 
		const position_accessor_t& accessor_ )
        : bv( center_, radius_vector_ )
        , _accessor( accessor_ )
    {}


    // child node ctor
    quad_tree( const quad_tree& parent, size_t child_num )
        : bv( parent._center, parent._radius_vector )
        , _accessor( parent._accessor )
    {
		for( size_t index = 0; index < DIMENSION; ++index )
		{
			bv::_radius_vector[ index ] *= 0.5;
		}
		
		/**
		*	---------
		*	| 2 | 3 |
		*	---------
		*	| 0 | 1 |
		*   ---------
		*/
		for( size_t index = 0; index < DIMENSION; ++index )
		{
			if ( child_num & ( 1 << index ) )
				bv::_center[ index ] += bv::_radius_vector[ index ];
			else
				bv::_center[ index ] -= bv::_radius_vector[ index ];			
		}
    }
    

    inline size_t find_child_index( const payload_t& payload )
    {
        const vec& position = _accessor( payload );

        assert( is_inside( position ) );

		size_t child = 0;
		for( size_t index = 0; index < DIMENSION; ++index )
		{
			if ( position[ index ] >= bv::_center[ index ] )
				child |= ( 1 << index );
		}
		return child;
    }
protected:
    const position_accessor_t&  _accessor;
	
	bucket_t					_bucket;

}; // class quad_tree

} // namespace stream_process

#endif

