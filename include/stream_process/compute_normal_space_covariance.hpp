#ifndef __STREAM_PROCESS__COMPUTE_NS_COVARIANCE__HPP__
#define __STREAM_PROCESS__COMPUTE_NS_COVARIANCE__HPP__

namespace stream_process
{

template< typename sp_types_t >
class compute_normal_space_covariance : public sp_types_t
{
public:
    STREAM_PROCESS_TYPES
	
	compute_normal_space_covariance(
		const attribute_accessor< vec3 >&       get_position,
		const attribute_accessor< nbh_type >&	get_neighbors,
		attribute_accessor< vec3 >&             get_normal
	);

	inline const mat4hp& operator()( stream_data* point );

    void set_max_neighbors( size_t max_k );

protected:
    inline bool _test( const stream_data* point ) const;

	const attribute_accessor< vec3 >&		_get_position;
	const attribute_accessor< nbh_type >&   _get_neighbors;
	attribute_accessor< vec3 >&				_get_normal;

    size_t			_max_neighbors;
    mat4hp			_ns_covariance;
    mat4hp			_ns_covar_tmp;

    vec4hp			_tangent_plane;

    hp_float_type	_weight;

}; // class compute_ns_covariance



template< typename sp_types_t >
compute_normal_space_covariance< sp_types_t >::
compute_normal_space_covariance( 
	const attribute_accessor< vec3 >&       get_position,
	const attribute_accessor< nbh_type >&	get_neighbors,
	attribute_accessor< vec3 >&             get_normal
    )
    : _get_position(	get_position )
    , _get_neighbors(	get_neighbors )
    , _get_normal(		get_normal )
    , _max_neighbors( 8 )
	, _ns_covariance()
	, _ns_covar_tmp()
	, _tangent_plane()
	, _weight( 0 )
{}




template< typename sp_types_t >
inline const typename compute_normal_space_covariance< sp_types_t >::mat4hp&
compute_normal_space_covariance< sp_types_t >::
operator()( stream_data* point )
{
    _ns_covariance.zero();	// C
    _ns_covar_tmp.zero();	// tmp
    
    assert( _test( point ) );

	const nbh_type& nbh = _get_neighbors( point );
	size_t max_nb       = nbh.size();
    
    if ( max_nb == 0 )
        throw exception( "empty neighborhood.", SPROCESS_HERE );

	if ( max_nb > _max_neighbors )
		max_nb = _max_neighbors;

    const float_t distance = nbh[ max_nb - 1 ].get_distance();

    assert( distance != 0.0 );
    
    hp_float_type density = M_PI * distance / max_nb;
    
    _tangent_plane  = 0.0;
    _weight         = 0.0;
    
	typename nbh_type::const_iterator
        it      = nbh.begin(), 
        it_end	= nbh.end();

    if ( it == it_end )
        throw exception( "empty neighborhood.", SPROCESS_HERE );

	for( size_t index = 0; it != it_end && index < max_nb; ++it, ++index )
	{
		const neighbor_type& nb		= *it;
		const stream_data* nb_ptr	= nb.get_ptr();
		
		const vec3& nb_pos      = _get_position( nb_ptr );
		const vec3& nb_normal   = _get_normal( nb_ptr );
        
        assert( _test( nb_ptr ) );

        _tangent_plane.set( 
            nb_normal.x(),
            nb_normal.y(),
            nb_normal.z(), 
            dot( nb_pos, nb_normal )
            );
       
        _ns_covar_tmp.tensor( _tangent_plane, _tangent_plane );

        // compute gaussian weight and use it to scale _C
        const hp_float_type f = exp(-0.5 * nb.get_distance() / density );

        assert( ! std::isnan( f ) );

        _weight        += f;
        _ns_covar_tmp  *= f;
        _ns_covariance += _ns_covar_tmp;
    }
    
    // scale matrix
    _ns_covariance *= static_cast< hp_float_type >( _max_neighbors ) / _weight;
    
    // DEBUG
    if ( _ns_covariance( 0, 0 ) == 0.0 &&
        _ns_covariance( 1, 1 ) == 0.0 )
    {
        typename nbh_type::const_iterator
            it      = nbh.begin(), 
            it_end	= nbh.end();
        for( size_t index = 0; it != it_end && index < max_nb; ++it, ++index )
        {
            const neighbor_type& nb		= *it;
            const stream_data* nb_ptr	= nb.get_ptr();
            
            const vec3& nb_pos      = _get_position( nb_ptr );
            const vec3& nb_normal   = _get_normal( nb_ptr );
            
            std::cout << "neighbor " << index 
                << "\npos    " << nb_pos
                << "\nnormal " << nb_normal
                << std::endl;
    
        }
    }
        
    
    
	return _ns_covariance;
}



template< typename sp_types_t >
void
compute_normal_space_covariance< sp_types_t >::
set_max_neighbors( size_t max_k )
{
    _max_neighbors = max_k;
}


template< typename sp_types_t >
inline bool
compute_normal_space_covariance< sp_types_t >::
_test( const stream_data* point ) const
{
    const vec3& position    = _get_position( point );
    const vec3& normal      = _get_normal( point );
    bool ok = true;    
    for( size_t index = 0; index < 3; ++index )
    {
        const sp_float_type& p = position[ index ];
        if ( std::isnan( p ) || std::isinf( p ) )
            return false;

        const sp_float_type& n = normal[ index ];
        if ( std::isnan( n ) || std::isinf( n ) )
            return false;
    }
    return true;
}

} // namespace stream_process

#endif

