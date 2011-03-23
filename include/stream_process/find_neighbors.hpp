#ifndef __STREAM_PROCESS__FIND_NEIGHBORS__HPP__
#define __STREAM_PROCESS__FIND_NEIGHBORS__HPP__

#include <stream_process/stream_process_types.hpp>
#include <stream_process/nb_shared_data.hpp>
#include <stream_process/knn_search_visitor.hpp>

#include <queue>

namespace stream_process
{

template< typename sp_types_t, typename node_t, bool autoremove = true >
class find_neighbors : public sp_types_t
{
public:
	STREAM_PROCESS_TYPES

    typedef typename nbh_type::globals                  nb_globals_type;
	typedef nb_shared_data< sp_types >                  nb_shared_data_type;
    typedef knn_search_visitor< sp_types, node_t >      knn_search_visitor_type;

    typedef node_t                                      node_type;
    typedef typename node_type::shared_data             node_shared_data_type;
    
    typedef typename slice_type::iterator               slice_iterator;
	
	typedef std::deque< slice_type* >					slice_buffer_type;
    
	find_neighbors( nb_shared_data_type& nb_shared_data_ );
    ~find_neighbors();
	
	void push( slice_type* slice );
	inline slice_type* top();
	inline void pop();
    
    inline size_t size() const { return _slice_buffer.size() + _out_buffer.size(); }
    
    inline void remove_from_tree( slice_type* slice );
	
	void setup( size_t max_k, size_t max_bucket_size );
    
    void clear_stage();
    
    node_type* get_root();
    bool    has_root() const;
    
protected:
	stream_data*	_prepare_next_point();
	slice_type*     _prepare_next_slice();
	void			_pop_slices();
	inline void		_finalize_slice();
	inline void		_compute_safe_range();
    void            _setup_root_node();
    bool            _DEBUG_check_slice();
	
	nb_shared_data_type&				_nb_shared_data;

	const attribute_accessor< vec3 >&	_get_position;
	attribute_accessor< nbh_type >&		_get_neighbors;

	node_type*                          _root;
    node_shared_data_type               _node_shared_data;
	knn_search_visitor_type				_knn_search_visitor;
    nb_globals_type*                    _nb_globals;
    
	size_t                              _max_k; 
	
	slice_buffer_type                   _slice_buffer;
	slice_buffer_type                   _out_buffer;
	
	stream_data*						_point;
	slice_type*                         _slice;
	slice_iterator                      _point_it;
	sp_float_type                       _safe_range;
	sp_float_type                       _max_z;
	
	bool								_test_bounds;
    
    size_t                              _DEBUG_points_completed;

private:
    find_neighbors() {}
    find_neighbors( const find_neighbors& original ) { assert( 0 ); }

}; // class find_neighbors


#define SP_TEMPLATE_STRING	template< typename sp_types_t, typename node_t, bool autoremove >
#define SP_CLASS_NAME		find_neighbors< sp_types_t, node_t, autoremove >


SP_TEMPLATE_STRING
SP_CLASS_NAME::
find_neighbors( nb_shared_data_type& nb_shared_data_ )
	: _nb_shared_data( nb_shared_data_ )
	, _get_position( _nb_shared_data.get_position_accessor() )
	, _get_neighbors( _nb_shared_data.get_neighbors_accessor() )
	, _root( 0 )
    , _node_shared_data()
	, _knn_search_visitor( _get_position, _get_neighbors )
    , _nb_globals( 0 )
    , _max_k( 0 )
    , _slice_buffer()
    , _out_buffer()
	, _point( 0 )
	, _slice( 0 )
    , _point_it()
    , _safe_range( 0 )
    , _max_z( - std::numeric_limits< sp_float_type >::max() )
	, _test_bounds( true )
    , _DEBUG_points_completed( 0 )
{}



SP_TEMPLATE_STRING
SP_CLASS_NAME::
~find_neighbors()
{
    assert( _slice_buffer.empty() );
    assert( _out_buffer.empty() );

    if ( _root ) 
        delete _root;
}


SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
push( slice_type* slice )
{
    assert( _test_bounds == true );
    //std::cout << "find_nb: pushing slice " << slice->get_slice_number() << std::endl;
    
    slice->get_ring_one_min() = std::numeric_limits< sp_float_type >::max();
    slice->get_ring_one_max() = - std::numeric_limits< sp_float_type >::max();

	typename slice_type::iterator it = slice->begin(), it_end = slice->end();
	for( ; it != it_end; ++it )
	{
		stream_data* point = *it;
		
		// setup neighborhood class
        nbh_type& nbs = _get_neighbors( point );
        new (&nbs) nbh_type( *_nb_globals );
	}

	stream_data* point = 0;
	for( it = slice->begin(), it_end = slice->end(); it != it_end; ++it )
	{
		point = *it;
        if ( ! _root->is_inside( _get_position( point ) ) )
        {
            throw exception(
                "point is outside of world aabb. insertion failed.",
                SPROCESS_HERE 
                );
        }

        // initialize search visitor and send it down the tree
        _knn_search_visitor.initialize_query( point );
        _root->accept_visitor( &_knn_search_visitor ); 
        
        // insert point into the tree
        _root->insert( point );
	}

    assert( point );
    // record maximum z
    assert( _get_position( point ).z() >= _max_z );
    _max_z = _get_position( point ).z();

	// add the slice to the buffer
	_slice_buffer.push_back( slice );
	// check and remove_if old slices

    if ( slice->is_last() )
    {
        std::cout << "last slice: " 
            << "slice " << slice->get_slice_number()
            << std::endl;
        _test_bounds = false;
    }

	_pop_slices();

}





SP_TEMPLATE_STRING
inline void
SP_CLASS_NAME::
pop()
{
	assert( ! _out_buffer.empty() );
	_out_buffer.pop_front();
}



SP_TEMPLATE_STRING
inline typename SP_CLASS_NAME::slice_type*	
SP_CLASS_NAME::
top()
{
	if ( _out_buffer.empty() )
		return 0;
	return _out_buffer.front();
}


SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
_pop_slices()
{
    if ( ! _point )
        _prepare_next_point();

	if ( ! _point )
		return;
        
    nbh_type* nbs = & _get_neighbors( _point );
    _compute_safe_range();

    //std::cout << "xsafe range " << _safe_range << " kth dist " << nbs->get_kth_nb().get_distance() << std::endl;
    while( nbs->is_full() && nbs->get_farthest().get_distance() < _safe_range )
    {
        // have all neighbors, but we need them in order
        nbs->sort();
        
        // determine and update bounds
        const vec3& position        = _get_position( _point );
        sp_float_type& r1_min_z     = _slice->get_ring_one_min();
        sp_float_type& r1_max_z     = _slice->get_ring_one_max();
        
        typename nbh_type::const_iterator it = nbs->begin(), it_end = nbs->end();
        for( ; it != it_end; ++it )
        {
            const neighbor_type& nb = *it;
            const sp_float_type& nb_z = _get_position( nb.get_ptr() ).z();
            if ( nb_z < r1_min_z )
                r1_min_z = nb_z;
            if ( nb_z > r1_max_z )
                r1_max_z = nb_z;
        }

        #if 0
        std::cout << "safe range " << _safe_range 
            << " kth dist " << nbs->get_farthest().get_distance() 
            << " diff " << nbs->get_farthest().get_distance()  - _safe_range
            << std::endl;
        #endif
        
        ++_DEBUG_points_completed;
        
        // get next or stop
        _point  = 0;
        nbs     = 0;
		if ( ! _prepare_next_point() )
			return;
        
        nbs = & _get_neighbors( _point );
        _compute_safe_range();
    }
}


SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
_finalize_slice()
{
    assert( _slice == _slice_buffer.front() );
    _slice_buffer.pop_front();

    //std::cout << "finalizing slice " << _slice->get_slice_number() << std::endl;

    sp_float_type& min_z    = _slice->get_min();
    sp_float_type& max_z    = _slice->get_max();
    sp_float_type& r1_min_z = _slice->get_ring_one_min();
    sp_float_type& r1_max_z = _slice->get_ring_one_max();
    
    typename slice_type::container_type& vertex_stream = _slice->get_stream( 0 );

    min_z = _get_position( vertex_stream.front() ).z();
    max_z = _get_position( vertex_stream.back() ).z();

    if ( r1_min_z > min_z )
        r1_min_z = min_z;
    if ( r1_max_z < max_z )
        r1_max_z = max_z;
    
    assert( min_z <= max_z );
    
    assert( _DEBUG_check_slice() );
    
    #if 0
    std::cout
        << " slice " << _slice->get_slice_number()
        << " min z " << _slice->get_min()
        << " max z " << _slice->get_max()
        << " r1 min z " << _slice->get_ring_one_min()
        << " r1 max z " << _slice->get_ring_one_max()
        << " world max z " << _max_z << std::endl;
    #endif
    
    if ( autoremove )
        remove_from_tree( _slice );

	_out_buffer.push_back( _slice );
    _slice = 0;
}



SP_TEMPLATE_STRING
inline void
SP_CLASS_NAME::
remove_from_tree( slice_type* slice )
{
    assert( slice );

    // remove all points in slice from tree
    typename slice_type::iterator it = slice->begin(), it_end = slice->end();
    for( ; it != it_end; ++it )
    {
        _root->remove( *it );
    }
}



SP_TEMPLATE_STRING
inline typename SP_CLASS_NAME::slice_type*
SP_CLASS_NAME::
_prepare_next_slice()
{
	if ( _slice_buffer.empty() )
		return 0;

	_slice		= _slice_buffer.front();

	assert( ! _slice->empty() );

	_point_it	= _slice->begin();

    _DEBUG_points_completed = 0;

	return _slice;
}



SP_TEMPLATE_STRING
inline stream_data*
SP_CLASS_NAME::
_prepare_next_point()
{
    assert( _point == 0 );
    
	if ( ! _slice )
    { 
        if ( ! _prepare_next_slice() )
            return 0;
    }
	else
		++_point_it;

    if ( _point_it == _slice->end() )
	{
		_finalize_slice();

		if ( ! _prepare_next_slice() )
			return 0;
	}
	
    _point = *_point_it; 
	return _point;  
}



SP_TEMPLATE_STRING
inline void
SP_CLASS_NAME::
_compute_safe_range()
{
	const sp_float_type& z = _get_position( _point ).z();

    if ( ! _test_bounds )
    {
        _safe_range = std::numeric_limits< sp_float_type >::max();
        return;
    }

    assert( ! std::isnan( _max_z ) );
    assert( ! std::isnan( z ) );

    assert( ! std::isinf( _max_z ) );
    assert( ! std::isinf( z ) );

    _safe_range = _max_z - z;
    //std::cout << "max z " << _max_z << ", z " << z << ", safe range " << _safe_range << std::endl;
    _safe_range *= _safe_range; // distances are stored squared for performance
    
    assert( ! std::isnan( _safe_range ) );
    assert( ! std::isinf( _safe_range ) );
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
setup( size_t max_k, size_t max_bucket_size )
{
    _nb_shared_data._k = max_k;
    _nb_shared_data._max_bucket_size = max_bucket_size;
    
    _nb_globals = new nb_globals_type( _nb_shared_data._k );
    _node_shared_data.max_bucket_size = _nb_shared_data._max_bucket_size;
    
    _setup_root_node();
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
clear_stage()
{
    //_test_bounds = false;
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
_setup_root_node()
{
    assert( _root == 0 );
    
    const vec3& aabb_min =  _nb_shared_data.get_aabb_min();
    const vec3& aabb_max =  _nb_shared_data.get_aabb_max();
    
    vec3 min_( aabb_min.x(), aabb_min.y(), aabb_min.z() );
    vec3 max_( aabb_max.x(), aabb_max.y(), aabb_max.z() );
    
    vec3 diag       = max_ - min_;
    vec3 center     = min_ + diag * 0.5;
    //vec3 half_size  = max_ - center;
    vec3 radius_vec = diag * 0.51; // tweak a bit for numeric reasons
    
    typename node_t::tree_type root_data( center, radius_vec, _get_position );
    _root = new node_type( root_data, _node_shared_data );
}



SP_TEMPLATE_STRING
node_t*
SP_CLASS_NAME::
get_root()
{
    return _root;
}


SP_TEMPLATE_STRING
bool
SP_CLASS_NAME::
has_root() const
{
    return _root;
}



SP_TEMPLATE_STRING
bool
SP_CLASS_NAME::
_DEBUG_check_slice()
{
    typename slice_type::iterator it = _slice->begin(), it_end = _slice->end();
    for( ; it != it_end; ++it )
    {
        stream_data* data = *it;
        assert( _get_neighbors( data ).is_sorted() );
        assert( _get_neighbors( data ).is_full() );
    }

    assert( _slice->get_stream( 0 ).size() == _DEBUG_points_completed );
    
    return true;
}


#undef SP_TEMPLATE_STRING
#undef SP_CLASS_NAME


} // namespace stream_process

#endif

