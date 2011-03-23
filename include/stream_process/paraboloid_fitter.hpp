#ifndef __STREAM_PROCESS__PARABOLOID_FITTER__HPP__
#define __STREAM_PROCESS__PARABOLOID_FITTER__HPP__

#include <stream_process/stream_process_types.hpp>

#include <stream_process/paraboloid.hpp>
#include <stream_process/principal_curvature_estimate.hpp>
#include <vmmlib/lapack_linear_least_squares.hpp>
#include <vmmlib/lapack_svd.hpp>

namespace stream_process
{

template< typename sp_types_t >
class paraboloid_fitter
{
public:
    STREAM_PROCESS_TYPES
    
    paraboloid_fitter();
    paraboloid_fitter( const paraboloid_fitter& orig_ );
    
    typedef paraboloid< sp_float_type >                 paraboloid_type;
    typedef principal_curvature_estimate< sp_types_t >  pce_type;
    
    void initialize( const pce_type& pce_,
        const stream_data* vertex_ );
        
    sp_float_type test_nb( const stream_data* nb_ );
    
    void setup_attributes( op_base_type& op, stream_structure& vs );
    void setup_accessors();
    
    template< size_t K, typename T >
    void fit( const stream_data* vertex_ );

protected:
    template< size_t K, typename T >
    void
    _get_vec9_from_position(vmml::vector< 9, T >& v9, const vec3& pos ) const;

    paraboloid_type     _paraboloid;
    mat4                _transform;
    bool                _same_sign;
    const stream_data*  _vertex;
    vec3                _vp0;
    
    attribute_accessor< vec3 >      _get_position;
    attribute_accessor< vec3 >      _get_normal;
    attribute_accessor< nbh_type >  _get_neighbors;
    
}; // class paraboloid_fitter


#define SP_CLASS_NAME      paraboloid_fitter< sp_types_t >
#define SP_TEMPLATE_STRING template< typename sp_types_t >


SP_TEMPLATE_STRING
SP_CLASS_NAME::
paraboloid_fitter()
{}


SP_TEMPLATE_STRING
SP_CLASS_NAME::
paraboloid_fitter( const paraboloid_fitter& orig_ )
    : _get_position(    orig_._get_position )
    , _get_normal(      orig_._get_normal )
    , _get_neighbors(   orig_._get_neighbors )
{

}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
initialize( const pce_type& pce_, const stream_data* vertex_ )
{
#if 1
    sp_float_type a2    = 4.0 / pce_.k1;
    sp_float_type b2    = 4.0 / pce_.k2;
    
    _same_sign = pce_.k1 < 0 == pce_.k2 < 0;

    _paraboloid.initialize( a2, b2 );

    _transform = mat4f::IDENTITY;
    
    // TODO remove normalize? should be norm already
    const vec3& normal = _get_normal( vertex_ );

    _transform.set_row( 0, normalize( pce_.e1 ) );
    _transform.set_row( 1, normalize( pce_.e2 ) );
    _transform.set_row( 2, normalize( normal ) );
    
    _vp0 = _transform * _get_position( vertex_ );
#else

    _vertex = vertex_;

    const nbh_type& nbh = _get_neighbors( _vertex );
    if ( nbh.size() < 9 )
        throw exception( "needs more neighbors.", SPROCESS_HERE );

    fit< 10, hp_float_type >( vertex_ );
    //fit< 10, sp_float_type >( vertex_ );
#endif

}


SP_TEMPLATE_STRING
template< size_t M, typename T >
void
SP_CLASS_NAME::
_get_vec9_from_position( vmml::vector< 9, T >& row, 
    const vec3& position_ ) const
{
    const T x       = static_cast< T >( position_.x() );
    const T y       = static_cast< T >( position_.y() );
    const T z       = static_cast< T >( position_.z() );
    
    row( 0 )    = x * x;
    row( 1 )    = y * y;
    row( 2 )    = z * z;
    row( 3 )    = x * y;
    row( 4 )    = y * z;
    row( 5 )    = z * x;
    row( 6 )    = x;
    row( 7 )    = y;
    row( 8 )    = z;
}


SP_TEMPLATE_STRING
template< size_t M, typename T >
void
SP_CLASS_NAME::
fit( const stream_data* vertex_ )
{
    typedef vmml::matrix< M, 9, T > matMx9;
    typedef vmml::matrix< 9, M, T > mat9xM;
    typedef vmml::matrix< 9, 9, T > mat9x9;
    typedef vmml::vector< 3, T >    vec3t;
    typedef vmml::vector< 4, T >    vec4t;
    typedef vmml::vector< 9, T >    vec9;
    typedef vmml::vector< M, T >    vecM;

    typedef vmml::matrix< 3, 3, T > mat3t;
    typedef vmml::matrix< 4, 4, T > mat4t;
    
    // Dai 2007, p. 506, (4) - (8)
    
    mat9xM A0t;
    vec9 col, vcol;
       
    const vec3& vp = _get_position( _vertex );

    _get_vec9_from_position< M, T >( vcol, vp );
    
    A0t.set_column( 0, vcol );
    
    const nbh_type& nbh = _get_neighbors( _vertex );
    typename nbh_type::const_iterator
        nb_it       = nbh.begin(),
        nb_it_end   = nbh.begin() + M - 1;
    for( size_t index = 1; nb_it != nb_it_end; ++nb_it, ++index )
    {
        const vec3& p   = _get_position( nb_it->get_ptr() );

        _get_vec9_from_position< M, T >( col, p );
        
        A0t.set_column( index, col );
    }
    
    // X -> A0
    // y -> B
    // b^-> X0

    vecM B( 1.0 );
    vec9 X0;
    matMx9 A0;
    A0t.transpose_to( A0 );

    vmml::lapack::linear_least_squares_xgels< M, 9, T >   lls;
    lls.compute( A0, B, X0 );
    
    std::cout << "A0\n" << A0 
        << "B\n" << B 
        << "X0 " << X0 
        << std::endl;
    
    #if 0
    vec9 L = vcol * X0;
    T L10 = L(0) + L(1) + L(2) + L(3) + L(4) + L(5) + L(6) + L(7) 
        + L(8) + L(9);
    L10 *= -1.0;
    #else
    T L10 = -1.0;
    #endif
    
    // Dai et al 2007, p. 506, (9) + (10)
    
    mat4t delta;
    delta( 0, 0 )   = X0( 0 );
    delta( 0, 1 )   = X0( 3 ) * 0.5;
    delta( 0, 2 )   = X0( 5 ) * 0.5;
    delta( 0, 3 )   = X0( 6 ) * 0.5;
    delta( 1, 0 )   = X0( 3 ) * 0.5;
    delta( 1, 1 )   = X0( 1 );
    delta( 1, 2 )   = X0( 4 ) * 0.5;
    delta( 1, 3 )   = X0( 7 ) * 0.5;
    delta( 2, 0 )   = X0( 5 ) * 0.5;
    delta( 2, 1 )   = X0( 4 ) * 0.5;
    delta( 2, 2 )   = X0( 2 );
    delta( 2, 3 )   = X0( 8 ) * 0.5;
    delta( 3, 0 )   = X0( 6 ) * 0.5;
    delta( 3, 1 )   = X0( 7 ) * 0.5;
    delta( 3, 2 )   = X0( 8 ) * 0.5;
    delta( 3, 3 )   = L10;
    
    T delta_det = delta.det();
    
    mat3t D;
    delta.get_sub_matrix( D );

    T D_det = D.det();

    std::cout << "delta det " << delta_det << ", D_det " << D_det 
        << ", L10 " << L10 << std::endl;

    // Dai et al 2007, p. 508, (36) + ..., (54) + ...
    
    mat3t A2( D );
    
    // (50)
    T s = delta.det();
    
    mat4t P( delta );
    
    vec3t sigma;
    vmml::lapack_svd< 3, 3, T > svd;

    bool ok = svd.compute( A2, sigma );

    vec4t r( sigma.x(), sigma.y(), sigma.z(), 0.0 );
    r *= 0.5;

    P.set_column( 3, r );
    P.set_row( 3, r );
    
    std::cout << "r" << r << "\nP" << P << std::endl;
    
    T p = P.det();
    
    std::cout
        << "s " << s << ", p " << p <<  std::endl;

    T v_plusminus = sqrt( fabs( s / p ) );
    
    if ( sigma.x() < 0 )
        exit( 42 );
    T v = sigma.x() < 0 ? -v_plusminus : v_plusminus;
    
    std::cout
        << "s " << s << "p " << p << " v+- " << v_plusminus << ", v " << v
        << " sigma " << sigma << " ok " << ok << std::endl;
    
    T a2 = v / sigma.x();
    T b2 = v / sigma.y();
    
    vec4t t;
    
    std::cout << "elliptic a2 " << a2 << " b2 " << b2 << std::endl;
    
    T k1 = 4.0 / a2;
    T k2 = 4.0 / b2;
    
    std::cout << "elliptic k1 " << k1 << " k2 " << k2 << std::endl;
    std::cout << "------------ " << std::endl;
}





SP_TEMPLATE_STRING
typename SP_CLASS_NAME::sp_float_type 
SP_CLASS_NAME::
test_nb( const stream_data* nb_ )
{
    // FIXME
    const vec3& nb_pos  = _get_position( nb_ );
    const vec3& p       = _transform * nb_pos;
    const vec3& p0      = _vp0 - p;
    

    sp_float_type z     =
        _same_sign ? _paraboloid.compute_z_elliptic( p0 )
            : _paraboloid.compute_z_hyperbolic( p0 );

    sp_float_type zdiff = p.z() - z;
    #if 0

    {
        const vec3& vpos    = _get_position( _vertex );
        const vec3f& xdiff  = nb_pos - vpos;
        sp_float_type d     = xdiff.length();
        
        const vec3& nb_posx = _transform * nb_pos;
        const vec3& vposx   = _transform * vpos;
        const vec3& xxdiff  = nb_posx - vposx;
        sp_float_type dx    = xxdiff.length();
        
        
        std::cout << "nb pos " << nb_pos << std::endl;
        std::cout << "v pos " << vpos << std::endl;
        std::cout << "nb pos x" << nb_posx << std::endl;
        std::cout << "v pos x" << vposx << std::endl;
        std::cout << "xdiff" << xdiff << std::endl;
        std::cout << "xxdiff" << xxdiff << std::endl;
        std::cout << "d " << d << ", dx " << dx << std::endl;
    
    
        std::cout << "\nzdiff " << zdiff << ", zd2 " << zdiff * zdiff << std::endl;
        
        std::cout << std::endl;
    }
    #endif
    return zdiff * zdiff;
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
setup_attributes( op_base_type& op, stream_structure& vs )
{
    op.read( vs, "position",    _get_position );
    op.read( vs, "normal",      _get_normal );
    op.read( vs, "neighbors",   _get_neighbors );
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
setup_accessors()
{
    _get_position.setup();
    _get_normal.setup();
    _get_neighbors.setup();
}


#undef SP_CLASS_NAME
#undef SP_TEMPLATE_STRING

} // namespace stream_process

#endif

