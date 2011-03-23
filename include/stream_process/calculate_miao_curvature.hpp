#ifndef __STREAM_PROCESS__CALCULATE_MIAO_CURVATURE__HPP__
#define __STREAM_PROCESS__CALCULATE_MIAO_CURVATURE__HPP__

#include <stream_process/stream_process_types.hpp>

#include <stream_process/compute_principal_curvatures.hpp>

#include <vmmlib/lapack_linear_least_squares.hpp>

namespace stream_process
{

template< typename sp_types_t >
class calculate_miao_curvature : public sp_types_t
{
public:
    STREAM_PROCESS_TYPES
    
    typedef curve< sp_types_t >                         curve_type;
    
    calculate_miao_curvature();
    calculate_miao_curvature( const calculate_miao_curvature& original_ );
    
    void operator()( stream_data* vertex );
    
    void set_k( size_t k_ );

    // setup the stream processing data accessors
    void setup_attributes( op_base_type& op, stream_structure& vs );
    void setup_accessors();
    
    void sample_curve( stream_data* vertex, const vec3& tangent_ );

protected:
    void _compute_curvature( stream_data* vertex );

    size_t      _nbh_size; // k, number of neighbors used for computations
    vec4        _normal_plane;
    
    vec3        _x1;
    vec3        _y1;
    vec3        _z1;
    
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

    attribute_accessor< size_t >            _get_point_index;

    vmml::matrix< 2, 3, sp_float_type >     _transform;
    std::vector< vec2 >                     _projected_nbs;
    
    

}; // class compute_curvature


#define SP_CLASS_NAME      calculate_miao_curvature< sp_types_t >
#define SP_TEMPLATE_STRING template< typename sp_types_t >


SP_TEMPLATE_STRING
SP_CLASS_NAME::
calculate_miao_curvature()
    : _nbh_size( 8 )
{
    _projected_nbs.resize( _nbh_size );
}


SP_TEMPLATE_STRING
SP_CLASS_NAME::
calculate_miao_curvature( const calculate_miao_curvature& orig )
    : _get_position(    orig._get_position )
    , _get_normal(      orig._get_normal )
    , _get_neighbors(   orig._get_neighbors )
    , _get_radius(      orig._get_radius )
    , _get_k1(          orig._get_k1 )
    , _get_e1(          orig._get_e1 )
    , _get_k2(          orig._get_k2 )
    , _get_e2(          orig._get_e2 )
    , _get_point_index( orig._get_point_index )
    , _nbh_size(        orig._nbh_size )
{}


SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
operator()( stream_data* vertex )
{
    // sample 3 curves, see miao paper p. 752
    const vec3& position    = _get_position( vertex );
    const vec3& normal      = _get_normal( vertex );
    
    vec3 t( 1.0, 0.0, 0.0 );
    
    if ( dot( t, normal ) != 1.0 )
        sample_curve( vertex, t );
    else
        sample_curve( vertex, normalize( vec3( 0.9, 0.1, 0.1 ) ) );
        

}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
sample_curve( stream_data* vertex, const vec3& tangent_ )
{
    const vec3& position    = _get_position( vertex );
    const vec3& normal      = _get_normal( vertex );

    const nbh_type& nbh     = _get_neighbors( vertex );


    _x1 = tangent_; //normal.cross( vec3(0,0,1));
    _y1 = normal;
    //_z1 = normal.cross( _x1 );
    
    _x1.normalize();
    _y1.normalize();
    //_z1.normalize();

    //assert( _y1.length() - 1.0 < _tolerance );

    //_normal_plane.set( _z1, - _z1.dot( position ) );   
    
    
    _transform.set_row( 0, _x1 );
    _transform.set_row( 1, _y1 );
    
    typedef vmml::matrix< 8, 2, sp_float_type > mat8x2;
    typedef vmml::matrix< 2, 8, sp_float_type > mat2x8;
    
    typedef vmml::vector< 8, sp_float_type >    vec8;
    
    mat8x2  A;
    vec8    B;

    typename nbh_type::const_iterator
        nb      = nbh.begin(),
        nb_end  = nbh.end();
    for( size_t index = 0; nb != nb_end && index < _nbh_size; ++nb, ++index )
    {
        const vec3& nb_pos      = position - _get_position( nb->get_ptr() );
        _projected_nbs[ index ] = _transform * nb_pos;
        
        vec2& nb_projpos  = _projected_nbs[ index ];
        
        // FIXME hack 
        if ( nb_projpos.x() == 0.0 )
        {
            nb_projpos.x() = nb_projpos.y() * 0.0001;
            if ( nb_projpos.x() == 0.0 )
            {
                nb_projpos.x() = 0.000001;
            }
            if ( index % 2 )
                nb_projpos.x() *= -1.0;
        }
        
        //std::cout << "nb " << index << " " << nb_projpos << std::endl;

        A( index, 0 )   = nb_projpos.x();
        A( index, 1 )   = nb_projpos.x() * nb_projpos.x();
        
        B( index )      = nb_projpos.y();

    }
    
    typename vmml::lapack::linear_least_squares_xgels< 8, 2, sp_float_type > llsq;
    
    vec2 X;
    
    if ( llsq.compute( A, B, X ) )
    {
        sp_float_type k = ( -2.0 * X.y() ) / pow( 1 + X.x() * X.x(), 1.5 );
        
        std::cout << "X " << X << " k " << k << std::endl;
    }

    
    
    #if 0

    typename nbh_type::const_iterator
        nb      = nbh.begin(),
        nb_end  = nbh.end();
    
    for( ; nb != nb_end; ++nb )
    {
        const vec3& nb_pos  = _get_position( nb->get_ptr() );
        _projected_nbs.push_back( _normal_plane.project_point_onto_plane( nb_pos ) );
    }
    
    typename std::vector< vec2 >::const_iterator
        vnb     = _projected_nbs.begin(),
        vnb_end = _projected_nbs.end();
    for( ; vnb != vnb_end; ++vnb )
    {
        std::cout << *vnb << std::endl;
    
    }
    #endif

}


SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
set_k( size_t k_ )
{
    _nbh_size = k_;
    _projected_nbs.resize( _nbh_size );
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

#if 0
    op.write( vs, "axis",           _get_axis, true );
    op.write( vs, "axis_ratio",		_get_axis_ratio, true );
    op.write( vs, "axis_length",	_get_axis_length, true );
#endif
    
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

}


#undef SP_CLASS_NAME
#undef SP_TEMPLATE_STRING


} // namespace stream_process



#endif

