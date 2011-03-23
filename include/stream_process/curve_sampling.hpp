#ifndef __STREAM_PROCESS__CURVE_SAMPLING__HPP__
#define __STREAM_PROCESS__CURVE_SAMPLING__HPP__

// an implementation of the curve sampling method proposed by
// agam + tang in 2005 in their paper
// 'A Sampling Framework for Accurate Curvature Estimation' published in
// IEEE Transactions of Visualization and Computer Graphics Volume 11, No. 5

#include <stream_process/stream_process_types.hpp>
#include <stream_process/curve.hpp>
#include <stream_process/compute_principal_curvatures.hpp>
#include <stream_process/principal_curvature_estimate.hpp>
#include <stream_process/paraboloid_fitter.hpp>

namespace stream_process
{

template< typename sp_types_t >
class curve_sampling : public sp_types_t
{
public:
    STREAM_PROCESS_TYPES
    
    typedef curve< sp_types_t >                         curve_type;
    typedef compute_principal_curvatures< sp_types_t >  compute_pc_type;
    typedef principal_curvature_estimate< sp_types_t >  pce_type;
    typedef paraboloid_fitter< sp_types_t >             paraboloid_fitter_type;

    typedef vmml::vector< 3, uint8_t >  vec3ub;

    curve_sampling();
    curve_sampling( const curve_sampling& original_ );
    
    void sample_curves( stream_data* vertex_ );
    
    void initialize( size_t k_ );
    
    void setup_attributes( op_base_type& op, stream_structure& vs );
    void setup_accessors();
    
    const std::vector< curve_type >& get_curves()   { return _curves; }
    const std::vector< vec3ub >& get_curve_tuples() { return _curve_tuples; }

protected:
    void    _setup_curve_tuples();

    void    _build_curve_set();
    void    _sample_curves();
    void    _project_curves_onto_tangent_plane();
    void    _compute_angles();

    void    _find_min_max_curves();
    void    _write_min_max_results();

    void    _compute_directions();
    void    _find_best_estimate();

    stream_data*        _vertex; // current base vertex 

    size_t              _k;
    size_t              _number_of_curves;

    size_t              _k_min_index;
    size_t              _k_max_index;

    std::vector< curve_type >   _curves;
    std::vector< vec3ub >       _curve_tuples;
    std::vector< pce_type >     _estimates;
    
    vmml::matrix< 2, 3, sp_float_type >     _transform;
    vmml::matrix< 3, 2, sp_float_type >     _transformT; // transposed transform

    compute_pc_type                         _compute_pc;
    paraboloid_fitter_type                  _fit_to_paraboloid;

    // stream processing data accessors
    attribute_accessor< vec3 >      _get_position;
    attribute_accessor< vec3 >      _get_normal;
    attribute_accessor< nbh_type >  _get_neighbors;

    attribute_accessor< vec3 >              _get_e1;
    attribute_accessor< sp_float_type >     _get_k1;
    attribute_accessor< sp_float_type >     _get_k2;  

}; // class curve_sampling


#define SP_CLASS_NAME      curve_sampling< sp_types_t >
#define SP_TEMPLATE_STRING template< typename sp_types_t >


SP_TEMPLATE_STRING
SP_CLASS_NAME::
curve_sampling()
    : _k( 0 )
{
    initialize( 10 );
}


SP_TEMPLATE_STRING
SP_CLASS_NAME::
curve_sampling( const curve_sampling& orig )
    : _get_position(    orig._get_position )
    , _get_normal(      orig._get_normal )
    , _get_neighbors(   orig._get_neighbors )
    , _get_e1(          orig._get_e1 )
    , _get_k1(          orig._get_k1 )
    , _get_k2(          orig._get_k2 )
    , _fit_to_paraboloid( orig._fit_to_paraboloid )
{
    initialize( orig._k );
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
sample_curves( stream_data* vertex_ )
{
    _vertex = vertex_;

    _build_curve_set();
    _sample_curves();
    _project_curves_onto_tangent_plane();
    _compute_angles();
    _find_min_max_curves();
    
    #if 1
    _compute_directions();
    _find_best_estimate();
    #else
    _write_min_max_results();
    #endif

}



// step 1
SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
_build_curve_set()
{
    // we generate the set A(v) as in agam+tang 2005, p.576, formula (14)
    const nbh_type& nbh = _get_neighbors( _vertex );
           
    assert( nbh.size() <= _k );
    
    typename std::vector< curve_type >::iterator
        curve_it = _curves.begin();
    
    nb_const_iterator
        nb      = nbh.begin(),
        nb_end  = nbh.begin() + _k;
    for( ; nb != nb_end; ++nb )
    {
        nb_const_iterator tmp = nb;
        for( ++tmp; tmp != nb_end; ++tmp, ++curve_it )
        {
            assert( curve_it != _curves.end() );

            // we store the pointer to the two neighbors in the curve
            curve_type& c   = *curve_it;
            c.vi            = nb;
            c.vj            = tmp;
            
            // the distance we get from the neighbor ptr is squared, therefore:
            c.vi_distance   = sqrt( c.vi->get_distance() );
            c.vj_distance   = sqrt( c.vj->get_distance() );
        }
    }
}



// step 2
SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
_sample_curves()
{
    typename std::vector< curve_type >::iterator
        curve_it        = _curves.begin(),
        curve_it_end    = _curves.end();
    for( ; curve_it != curve_it_end; ++curve_it )
    {
        curve_type& c               = *curve_it;

        const neighbor_type* nb_vi  = c.vi;
        const neighbor_type* nb_vj  = c.vj;
        
        sp_float_type& t = c.t;

        // -> agam+tang 2005, p. 576, formula (15)
        t = c.vi_distance / ( c.vi_distance + c.vj_distance );

        // get the position of the 3 vertices
        const vec3& v   = _get_position( _vertex );
        const vec3& vi  = _get_position( nb_vi->get_ptr() );
        const vec3& vj  = _get_position( nb_vj->get_ptr() );
        
        vec3& tangent   = c.tangent;

        // -> agam+tang 2005, p. 576, formula (17)
        tangent =   vi * ( ( t - 1.0 ) / t );
        tangent +=  v  * ( 1.0 - ( t * 2.0 ) ) /  ( t - t*t );
        tangent +=  vj * ( t / ( 1.0 - t ) );

        assert( ! std::isnan( tangent.x() ) );
        assert( ! std::isnan( tangent.y() ) );
        assert( ! std::isnan( tangent.z() ) );

        vec3& pdp       = c.p_double_prime;

        // -> agam+tang 2005, p. 576, formula (20)
        pdp     = vi * ( 2.0 / t );
        pdp    += v  * ( -2.0 / ( t - t*t ) );
        pdp    += vj * ( 2.0 / ( -t + 1.0 ) );

        vec3& k         = c.k;

        // -> agam+tang 2005, p. 576, formulas (19,21)
        sp_float_type t_len     = tangent.length();
        sp_float_type t_len_3   = t_len * t_len * t_len;

        k.cross( tangent, pdp );
        k /= t_len_3;
            
        const vec3& normal  = _get_normal( _vertex );
        
        sp_float_type& kn   = c.kn;
        kn = dot( k, normal );

    }
}



// step 3
SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
_project_curves_onto_tangent_plane()
{

    // choose arbitrary tangent vector
    const vec3& t0          = _curves[ 0 ].tangent;
    
    const vec3& normal      = _get_normal( _vertex ); 

    // setting up the matrix for agam+tang 2005, formula (25)
    vec3 t0_x_n;
    t0_x_n.cross( t0, normal );
    
    _transform.set_row( 0, normalize( t0 ) );
    _transform.set_row( 1, normalize( t0_x_n ) );
    
    _transform.transpose_to( _transformT );
    
    typename std::vector< curve_type >::iterator
        it      = _curves.begin(),
        it_end  = _curves.end();
    for( ; it != it_end; ++it )
    {
        curve_type& c   = *it;
        vec2& tk        = c.proj_tangent;
        
        // agam+tang 2005, formula (25)
        tk = _transform * c.tangent;
        
        assert( ! std::isnan( tk.x() ) );
        assert( ! std::isnan( tk.y() ) );

        tk.normalize();
        
        assert( ! std::isnan( tk.x() ) );
        assert( ! std::isnan( tk.y() ) );

        assert( c.proj_tangent.length() - 1.0f < 0.0001 );
    }
}



// step 4
SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
_compute_angles()
{
    const vec2& t_ = _curves[0].proj_tangent;
    _curves[0].angle = 0.0;

    assert( fabs( t_.length() - 1.0f ) < 0.0001 );

    // compute angles between the projected tangents
    typename std::vector< curve_type >::iterator
        it      = _curves.begin(),
        it_end  = _curves.end();
    for( ++it; it != it_end; ++it )
    {
        curve_type& c   = *it;
        assert( fabs( c.proj_tangent.length() - 1.0f ) < 0.0001 );
        
        sp_float_type& angle    = c.angle;
        angle = acos( dot( t_, c.proj_tangent ) );

        assert( ! std::isnan( c.angle ) );


    }

    #if 0
    
    std::list< curve_type* >    curves_list;
    it      = _curves.begin();
    it_end  = _curves.end();
    for( ; it != it_end; ++it )
    {
        curves_list.push_back( &(*it) );
    }

    curves_list.sort( typename curve_type::angle_less_ptr() );
    
    typename std::list< curve_type* >::const_iterator
        cit     = curves_list.begin(),
        cit_end = curves_list.end();
    for( size_t index = 0; cit != cit_end; ++cit, ++index )
    {
        std::cout << "curve " << index << " angle " << (*cit)->angle << std::endl;
    
    }
    
    #endif
}



// step 5
SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
_find_min_max_curves()
{
    sp_float_type k_min = (std::numeric_limits< sp_float_type >::max)();
    sp_float_type k_max = 0;

    _k_min_index    = 0;
    _k_max_index    = 0;

    // compute angles between the projected tangets
    typename std::vector< curve_type >::iterator
        it      = _curves.begin(),
        it_end  = _curves.end();
    for( size_t index = 0; it != it_end; ++it, ++index )
    {
        curve_type& c   = *it;

        sp_float_type k = fabs( c.kn );
        
        if ( k < k_min )
        {
            k_min           = k;
            _k_min_index    = index;
        }
        if ( k > k_max )
        {
            k_max           = k;
            _k_max_index    = index;
        }
        
        assert( fabs( k_min ) <= fabs( k_max ) );

    }
}



// step 6
SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
_compute_directions()
{
    vec2 e1_2d;

    const size_t size = _curve_tuples.size();
    
    // FIXME
    _estimates.clear();
    pce_type pce;
    for( size_t index = 0; index < size; ++index )
    {
        const vec3ub& indices   = _curve_tuples[ index ];
        
        const curve_type& a = _curves[ indices.x() ];
        const curve_type& b = _curves[ indices.y() ];
        const curve_type& c = _curves[ indices.z() ];
        
        sp_float_type min_diff = 0.0872664626; //5 degrees // 0.5235; // 30 degrees

        if ( fabs( a.angle - b.angle ) > min_diff
            && fabs( a.angle - c.angle ) > min_diff
            && fabs( b.angle - c.angle ) > min_diff )
        {
            pce.a   = &a;
            pce.b   = &b;
            pce.c   = &c;
            
            _estimates.push_back( pce );
        }

    }
    typename std::vector< pce_type >::iterator
        it      = _estimates.begin(),
        it_end  = _estimates.end();
    for( ; it != it_end; ++it )
    {
        pce_type& pce           = *it;
        
        const curve_type& a = *pce.a;
        const curve_type& b = *pce.b;
        const curve_type& c = *pce.c;

        sp_float_type& k1       = pce.k1;
        sp_float_type& k2       = pce.k2;
        sp_float_type& theta    = pce.theta;

        pce.valid = _compute_pc( a, b, c, k1, k2, theta );

        if ( pce.valid == false )
        {
            //std::cout << "invalid estimate0" << std::endl;
            continue;
        }


        // determine the principal curvature directions (since we now have theta )
        const vec2& t = a.proj_tangent;

        // rotate proj tangent by theta to get e1
        const sp_float_type sin_t = sin( theta );
        const sp_float_type cos_t = cos( theta );

        e1_2d.x()  = t.x() * cos_t - t.y() * sin_t;
        e1_2d.y()  = t.x() * sin_t + t.y() * cos_t;

        // FIXME??
        bool swap = k2 > k1;
        
        vec3& e1    = swap ? pce.e2 : pce.e1;
        vec3& e2    = swap ? pce.e1 : pce.e2;
        
        if ( swap )
        {
            sp_float_type t = k1;
            k1 = k2;
            k2 = t;
        }

        e1   = normalize( _transformT * e1_2d );
        // e2 is obtained by computing the cross product of e1 and the normal
        e2.cross( e1, _get_normal( _vertex ) );
        e2.normalize();
        
    }
}





// step 7
SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
_find_best_estimate()
{
    sp_float_type best = (std::numeric_limits< sp_float_type >::max)();
    
    pce_type* best_pce = 0;
    size_t valid_estimates = 0;

    typename std::vector< pce_type >::iterator 
        it      = _estimates.begin(),
        it_end  = _estimates.end();
    for( ; it != it_end; ++it )
    {
        pce_type& pce_ = *it;
        if ( pce_.valid == false )
        {
            continue;
        }
        best_pce = &pce_;
        _fit_to_paraboloid.initialize( pce_, _vertex );
        
#if 1
        sp_float_type sum = 0.0;

        const nbh_type& nbh = _get_neighbors( _vertex );
        typename nbh_type::const_iterator
            nb_it       = nbh.begin(),
            nb_it_end   = nbh.end();
        for( ; nb_it != nb_it_end; ++nb_it )
        {
            sp_float_type dist = _fit_to_paraboloid.test_nb( nb_it->get_ptr() );
            
            sum += fabs( dist );
        }
        
        sp_float_type& q    = pce_.quality;
        q = sum / static_cast< sp_float_type >( nbh.size() );
               
        if ( best > q )
        {
            best        = q;
            best_pce    = &pce_;
        }

        ++valid_estimates;
#endif
    }

    assert( valid_estimates != 0 );

    if ( best_pce != 0 )
    {
        //std::cout << "q " << best_pce->quality << std::endl;

        vec3& e1            = _get_e1( _vertex );
        sp_float_type&  k1  = _get_k1( _vertex );
        sp_float_type&  k2  = _get_k2( _vertex );

        e1 = best_pce->e1;
        k1 = best_pce->k1;
        k2 = best_pce->k2;
        
    }
}



// step 7
SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
_write_min_max_results()
{
    vec3& e1            = _get_e1( _vertex );
    sp_float_type&  k1  = _get_k1( _vertex );
    sp_float_type&  k2  = _get_k2( _vertex );
    
    const curve_type& c1    = _curves[ _k_max_index ];
    const curve_type& c2    = _curves[ _k_min_index ];
    
    e1  = c1.k;
    k1  = c1.kn;
    k2  = c2.kn;
}




SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
setup_attributes( op_base_type& op, stream_structure& vs )
{
    op.read( vs, "position",    _get_position );
    op.read( vs, "normal",      _get_normal );
    op.read( vs, "neighbors",   _get_neighbors );

    op.write( vs, "e1",         _get_e1 );
    op.write( vs, "k1",         _get_k1 );
    op.write( vs, "k2",         _get_k2 );

    _fit_to_paraboloid.setup_attributes( op, vs );
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
setup_accessors()
{
    _get_position.setup();
    _get_normal.setup();
    _get_neighbors.setup();
    
    _get_e1.setup();
    _get_k1.setup();
    _get_k2.setup();
    
    _fit_to_paraboloid.setup_accessors();
}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
initialize( size_t k_ )
{
    if ( _k == k_ )
        return;
        
    _k = k_;

    assert( _k <= 16 ); // otherwise we need larger than 8bit ints for tuples

    // -> agam+tang 2005, p. 576, #A(v)
    _number_of_curves = ( _k * ( _k - 1 ) ) / 2;
    _curves.resize( _number_of_curves );
    
    _setup_curve_tuples();

}



SP_TEMPLATE_STRING
void
SP_CLASS_NAME::
_setup_curve_tuples()
{
    _curve_tuples.clear();

    const size_t size = _curves.size();

    for( size_t index = 0; index < size; ++index )
    {
        for( size_t j = index + 1; j < size; ++j )
        {
            for( size_t k = j + 1; k < size; ++k )
            {
                _curve_tuples.push_back( vec3ub( index, j, k ) );
            }
        }
    }

    #if 0
    typename std::vector< vec3s >::const_iterator
        it      = _curve_tuples.begin(),
        it_end  = _curve_tuples.end();
    for( ; it != it_end; ++it )
    {
        std::cout << *it << std::endl;
    }
    #endif
}

#undef SP_CLASS_NAME
#undef SP_TEMPLATE_STRING

} // namespace stream_process

#endif

