#ifndef __STREAM_PROCESS__COMPUTE_CURVATURE__HPP__
#define __STREAM_PROCESS__COMPUTE_CURVATURE__HPP__

#include <stream_process/stream_process_types.hpp>

#include <stream_process/curve_sampling.hpp>

#include <stream_process/fit_to_paraboloid.hpp>

namespace stream_process
{

template< typename sp_types_t >
class compute_curvature : public sp_types_t
{
public:
    STREAM_PROCESS_TYPES
    
    typedef curve< sp_types_t >                         curve_type;
    typedef curve_sampling< sp_types_t >                curve_sampling_type;
    
    compute_curvature();
    compute_curvature( const compute_curvature& original_ );
    
    void operator()( stream_data* vertex );
    
    void set_k( size_t k_ );

    // setup the stream processing data accessors
    void setup_attributes( op_base_type& op, stream_structure& vs );
    void setup_accessors();

protected:
    void _compute_curvature( stream_data* vertex );

    size_t      _nbh_size; // k, number of neighbors used for computations

    curve_sampling_type                     _curve_sampling;

    // stream processing data accessors

    attribute_accessor< vec3 >              _get_position;
    attribute_accessor< vec3 >              _get_normal;
    attribute_accessor< nbh_type >          _get_neighbors;

	attribute_accessor< sp_float_type >     _get_radius;

    // k1 is the maximal curvature, e1 its principal curvature direction 
	attribute_accessor< sp_float_type >     _get_k1;    // max curvature
	attribute_accessor< vec3 >              _get_e1;    // direction 

    // k2 is the maximal curvature, e2 its principal curvature direction 
	attribute_accessor< sp_float_type >     _get_k2;    // min curvature
	attribute_accessor< vec3 >              _get_e2;    // direction

    // splat axis direction, length and ratio
	attribute_accessor< vec3 >              _get_axis;
	attribute_accessor< sp_float_type >     _get_axis_length;
	attribute_accessor< sp_float_type >     _get_axis_ratio;
    
    attribute_accessor< size_t >            _get_point_index;
    
    fit_to_paraboloid< 10, hp_float_type >  _fitter;


}; // class compute_curvature


#define SP_CLASS_NAME      compute_curvature< sp_types_t >
#define SP_TEMPLATE_STRING template< typename sp_types_t >


SP_TEMPLATE_STRING
SP_CLASS_NAME::
compute_curvature()
    : _nbh_size( 10 )
{}


SP_TEMPLATE_STRING
SP_CLASS_NAME::
compute_curvature( const compute_curvature& orig )
    : _get_position(    orig._get_position )
    , _get_normal(      orig._get_normal )
    , _get_neighbors(   orig._get_neighbors )
    , _get_radius(      orig._get_radius )
    , _get_k1(          orig._get_k1 )
    , _get_e1(          orig._get_e1 )
    , _get_k2(          orig._get_k2 )
    , _get_e2(          orig._get_e2 )
    , _get_axis(        orig._get_axis )
    , _get_axis_ratio(  orig._get_axis_ratio )
    , _get_axis_length( orig._get_axis_length )
    , _get_point_index( orig._get_point_index )
    , _nbh_size(        orig._nbh_size )
    , _curve_sampling(  orig._curve_sampling )
{}


SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
operator()( stream_data* vertex )
{
    #if 0
    // set vertex position
    const vec3& position = _get_position( vertex );
    
    //std::cout << "vertex position " << position << std::endl;
    
    _fitter.set_position( 0, position );
    
    // set neighbor positions
    const nbh_type& nbh = _get_neighbors( vertex );
    typename nbh_type::const_iterator
        nb_it       = nbh.begin(),
        nb_it_end   = nbh.end();
    for( size_t index = 1; nb_it != nb_it_end && index < 10; ++nb_it, ++index )
    {
        _fitter.set_position( index, _get_position( nb_it->get_ptr() ) );
    }
    
    
    hp_float_type   k1, k2;
    vec3hp          t;
    
    _fitter.fit( k1, k2, t );
    
    _get_k1( vertex )   = static_cast< sp_float_type >( k1 );
    _get_k2( vertex )   = static_cast< sp_float_type >( k2 );
    
    const vec3& axis            = _get_axis( vertex );
    vec3& e1                    = _get_e1( vertex );
    e1 = axis;

    //std::cout << "---\nk1 " << k1 << "\nk2 " << k2 << "\nt " << t << std::endl;
    
    #endif
    #if 1
    _curve_sampling.sample_curves( vertex );
    
    const sp_float_type radius = _get_radius( vertex );

    vec3& axis                  = _get_axis( vertex );
    sp_float_type& axis_length  = _get_axis_length( vertex );
    sp_float_type& axis_ratio   = _get_axis_ratio( vertex );
    
    const vec3& e1              = _get_e1( vertex );
    const vec3& normal          = _get_normal( vertex );
    axis.cross( e1, normal );
    axis.normalize();
    
    axis_length                 = radius;
    axis_ratio                  = 0.5;
    #endif
}


SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
_compute_curvature( stream_data* vertex )
{

}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
set_k( size_t k_ )
{
    std::cout << "setting k " << k_ << std::endl;
    _nbh_size = k_;

    _curve_sampling.initialize( _nbh_size );
}


SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
setup_attributes( op_base_type& op, stream_structure& vs )
{
    op.read( vs, "position",    _get_position );
    op.read( vs, "normal",      _get_normal );
    op.read( vs, "neighbors",   _get_neighbors );
    op.read( vs, "point_index", _get_point_index );

    op.read( vs, "radius",      _get_radius );

    op.write( vs, "k1",         _get_k1 );
    op.write( vs, "e1",         _get_e1 );
    op.write( vs, "k2",         _get_k2 );
    op.write( vs, "e2",         _get_e2 );

    op.write( vs, "axis",           _get_axis, true );
    op.write( vs, "axis_ratio",		_get_axis_ratio, true );
    op.write( vs, "axis_length",	_get_axis_length, true );
    
    _curve_sampling.setup_attributes( op, vs );
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
setup_accessors()
{
    _get_position.setup();
    _get_normal.setup();
    _get_neighbors.setup();
    _get_point_index.setup();
    
    _get_radius.setup();
    
    _get_k1.setup();
    _get_e1.setup();
    _get_k2.setup();
    _get_e2.setup();

    _get_axis.setup();
    _get_axis_length.setup();
    _get_axis_ratio.setup();

    _curve_sampling.setup_accessors();
}


#undef SP_CLASS_NAME
#undef SP_TEMPLATE_STRING


} // namespace stream_process

#endif

