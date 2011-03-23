#ifndef __STREAM_PROCESS__COMPUTE_ACCURATE_CURVATURE__HPP__
#define __STREAM_PROCESS__COMPUTE_ACCURATE_CURVATURE__HPP__

#include <stream_process/stream_process_types.hpp>

#include <vmmlib/svd.hpp>

#include <stream_process/miao_get_principal_directions.hpp>
#include <stream_process/curve_debug_file.hpp>
#include <stream_process/paraboloid.hpp>
#include <stream_process/paraboloid_test.hpp>

namespace stream_process
{

template< typename sp_types_t >
class compute_accurate_curvature : public sp_types_t
{
public:
    STREAM_PROCESS_TYPES 
    
    typedef miao_get_principal_directions< sp_types_t > miao_dirs;
    
    typedef vmml::vector< 3, size_t >       vec3s;
    typedef paraboloid_test< sp_types_t >   paraboloid_test_type;
    
    struct curve
    {
        const neighbor_type*    vi;
        const neighbor_type*    vj;

        sp_float_type           t;
        vec3                    tangent;
        vec3                    p_doubleprime;
        vec3                    k;
        sp_float_type           kn;
        
        sp_float_type           angle;
        
        vec2                    proj_tangent;

        sp_float_type           distance;
        
    };
    
    struct curvature_set
    {
        size_t                  tuple_index;
        vec3                    e1;
        vec3                    e2;
        sp_float_type           k1;
        sp_float_type           k2;
    };

    struct angle_compare
    {
        inline bool operator()( curve const* a, curve const* b ) const
        {
            return a->angle < b->angle;
        }
    };
    

    compute_accurate_curvature()
        : _use_stream_normal( true )
        , _get_principal_dirs()
    {
        set_k( 8 ); // default to 8 neighbors
    }

    compute_accurate_curvature( const compute_accurate_curvature& orig )
        : _get_position( orig._get_position )
        , _get_normal( orig._get_normal )
        , _get_neighbors( orig._get_neighbors )
        , _get_radius( orig._get_radius )
        , _get_k_min( orig._get_k_min )
        , _get_k_min_dir( orig._get_k_min_dir )
        , _get_k_max( orig._get_k_max )
        , _get_k_max_dir( orig._get_k_max_dir )
        , _get_axis( orig._get_axis )
        , _get_axis_ratio( orig._get_axis_ratio )
        , _get_axis_length( orig._get_axis_length )
        , _use_stream_normal( orig._use_stream_normal )
        , _get_point_index( orig._get_point_index )
        , _k( orig._k )
        , _get_principal_dirs()
    {
        set_k( _k ); 
    }

    inline void operator()( stream_data* vertex )
    {
        assert( vertex );
        _vertex = vertex;
    
        setup_curves();
        
        compute_tangents_and_curves();
        
        project_onto_tangent_plane_and_compute_angles();
        
        // integrated in compute_tangents_and_curves: compute_curvature( _get_normal( _vertex ) );
        
        sp_float_type k_min = std::numeric_limits< sp_float_type >::max();
        sp_float_type k_max = -std::numeric_limits< sp_float_type >::max();
        
        const curve* min_curve = 0;
        const curve* max_curve = 0;
        
        typename std::list< curve* >::const_iterator it = _used_curves.begin(),
            it_end = _used_curves.end();

        for( ; it != it_end; ++it )
        {
            const curve& p = **it;
            
            float k = p.kn;
            
            if ( k < k_min )
            {
                k_min       = k;
                min_curve   = &p;
            }

            if ( k > k_max )
            {
                k_max       = k;
                max_curve   = &p;
            }
        }
        
        assert( min_curve );
        assert( max_curve );
        assert( min_curve != max_curve );

        const vec3& normal      = _get_normal( _vertex );
        const vec3& position    = _get_position( _vertex );

        // project curve-tangents onto tangent-plane of the current vertex
        _get_k_min( _vertex )       = k_min;

        vec3 v = min_curve->tangent;
        _get_k_min_dir( _vertex )   = v - dot( v, normal ) * normal;
        
        _get_k_max( _vertex )       = k_max;
        
        v = max_curve->tangent;
        _get_k_max_dir( _vertex )   = v - dot( v, normal ) * normal;
        vec3& splat_axis = _get_axis( _vertex );
        
        #if 1
        splat_axis = normalize( _get_k_max_dir( _vertex ) );
        #else
        splat_axis.cross( max_curve->tangent, normal );
        splat_axis.normalize();
        #endif

        _get_axis_ratio( _vertex ) = 0.5;
        _get_axis_length( _vertex ) = 2.0 * _get_radius( _vertex ); // FIXME for comparison with old curv
        

        std::string filename = "curve_debug.binary." 
            + boost::lexical_cast< std::string >( _get_point_index( _vertex ) );
        curve_debug_file curve_debug( filename );

        write_out_curves(  curve_debug );

        curve_debug_file::pce   pce_;

        for( size_t index = 0; index < 10; ++index )
        {
            get_principal_curvature_directions( index, pce_ );
            curve_debug.write_pce( pce_ );
        }
        
    }
    
    void write_out_curves( curve_debug_file& curve_debug )
    {
    
        const nbh_type& nbh = _get_neighbors( _vertex );

        size_t points = 1 + nbh.size();
        size_t curves = _curves.size();
        
        curve_debug.write_header( points, curves );
        
        curve_debug.write_point( 
            _get_position( _vertex ),
            _get_normal( _vertex ),
            _get_radius( _vertex )
            );
        
        typename nbh_type::const_iterator
            nb      = nbh.begin(),
            nb_end  = nbh.end();
        for( ; nb != nb_end; ++nb )
        {
            const stream_data* point = (*nb).get_ptr();
            curve_debug.write_point( 
                _get_position( point ),
                _get_normal( point ),
                _get_radius( point )
                );
        }

        const nbh_type& nbh_ = _get_neighbors( _vertex );

        typename std::vector< curve >::const_iterator
            xit = _curves.begin(),
            xit_end = _curves.end();
        for( size_t index = 0 ; xit != xit_end; ++xit, ++index )
        {
            const curve& c = *xit;
            curve_debug.write_curve(
                nbh_.get_neighbor_index( c.vi ),
                nbh_.get_neighbor_index( c.vj ),
                c.tangent,
                c.k,
                c.kn
                );
            
        }
    }
    
    // miao et al: differentials-based segmentation and parametrization
    void get_principal_curvature_directions( const size_t tuple_index, curve_debug_file::pce& pce_ )
    {
        typename miao_dirs::params   params_;
        typename miao_dirs::results  results_;

        curvature_set cs;
        cs.tuple_index = tuple_index;

        _get_dirs( cs, params_, results_ );
        fit_to_ellipsoid( cs );
        
        pce_.curve_indices  = _curve_tuples[ cs.tuple_index ];
        pce_.k1 = cs.k1;
        pce_.k2 = cs.k2;
        
        pce_.e1 = cs.e1;
        pce_.e2 = cs.e2;
    }
    
    void _get_dirs(
        curvature_set& cs,
        typename miao_dirs::params& params_,
        typename miao_dirs::results& results_
        )
    {
        assert( cs.tuple_index < _curve_tuples.size() );
        
        const vec3s indices = _curve_tuples[ cs.tuple_index ];

        const curve& a = _curves[ indices.x() ];
        const curve& b = _curves[ indices.y() ];
        const curve& c = _curves[ indices.z() ];
        
        std::cout
            << "using curves " 
            << indices
            << std::endl;
        
        params_.ka  = a.kn;
        params_.kb  = b.kn;
        params_.kc  = c.kn;
        
        params_.theta_1  = acos( dot( a.proj_tangent, b.proj_tangent ));//b.angle - a.angle;
        params_.theta_2  = acos( dot( a.proj_tangent, c.proj_tangent ));//c.angle - a.angle;

        _get_principal_dirs( params_, results_ );

        cs.k1   = results_.k1;
        cs.k2   = results_.k2;
        
        // FIXME assert( cs.k1 >= cs.k2 );
        
        //const vec2& pt0 = a.proj_tangent;
        std::cout << cs.tuple_index << ": " << results_ << std::endl;

        // determine the principal curvature directions (since we now have theta )
        const vec2& t = a.proj_tangent;
        
        sp_float_type& theta = results_.theta;
        
        // rotate proj tangent by theta to get e1
        const sp_float_type& sin_t = sin( theta );
        const sp_float_type& cos_t = cos( theta );
        
        vec2 e1_2d;
        
        e1_2d.x()  = t.x() * cos_t - t.y() * sin_t;
        e1_2d.y()  = t.x() * sin_t + t.y() * cos_t;
        
        cs.e1   = _tangent_plane_backtransform * e1_2d;
        // e2 is obtained by computing the cross product of e1 and the normal
        cs.e2.cross( cs.e1, _get_normal( _vertex ) );
        
        

    }

    inline void fit_to_ellipsoid( curvature_set& cs )
    {
        _paraboloid_init.position   = _get_position( _vertex );
        _paraboloid_init.normal     = _get_normal( _vertex );
        _paraboloid_init.k1         = cs.k1;
        _paraboloid_init.k2         = cs.k2;
        _paraboloid_init.e1         = cs.e1;
        _paraboloid_init.e2         = cs.e2;
        
        _paraboloid.initialize( _paraboloid_init );
        
        _diffs.clear();
        
        const nbh_type& nbh = _get_neighbors( _vertex );

        typename nbh_type::const_iterator
            nb      = nbh.begin(),
            nb_end  = nbh.end();
        for( ; nb != nb_end; ++nb )
        {
            const vec3& nb_pos = _get_position( nb->get_ptr() );
            _diffs.push_back( _paraboloid.get_z_diff( nb_pos ) );
        }
        
        typename std::vector< sp_float_type >::iterator
            it = _diffs.begin(),
            it_end = _diffs.end();
        for( ; it != it_end; ++it )
        {
            if ( it == _diffs.begin() )
                std::cout << "diffs: ";
            else
                std::cout << ", ";
            std::cout << *it;
        }
        std::cout << std::endl;
        
    }

    inline void setup_curves()
    {
        // we generate the set A(v) as in agam+tang 2005, p.576, formula (14)
        const nbh_type& nbh = _get_neighbors( _vertex );
               
        assert( nbh.size() <= _k );

        typename nbh_type::const_iterator
            nb      = nbh.begin(),
            nb_end  = nbh.begin() + _k;

        size_t cnt = 0;

        typename std::vector< curve >::iterator curve_it = _curves.begin();
        for( ; nb != nb_end; ++nb )
        {
            typename nbh_type::const_iterator tmp = nb;
            for( ++tmp; tmp != nb_end; ++tmp, ++curve_it )
            {
                assert( curve_it != _curves.end() );

                curve& c = *curve_it;
                c.vi        = nb;
                c.vj        = tmp;
                c.distance  = nb->get_distance() + tmp->get_distance();

                ++cnt;
            }
        }
        //std::cout << cnt << " curves " << std::endl;
    }
    
    void display_debug_view()
    {
    
        
    
    
    }
    
    
    inline void compute_tangents_and_curves()
    {
        vec3 tangent;
        vec3 p_dp; // p_doubleprime
        vec3 k; 
        for( size_t index = 0; index < _number_of_curves; ++index )
        {
            curve& curve_ = _curves[ index ];

            const neighbor_type* nb_vi = curve_.vi;
            const neighbor_type* nb_vj = curve_.vj;
            
            // -> agam+tang 2005, p. 576, formula (15)
            sp_float_type t = nb_vi->get_distance() / curve_.distance;
            
            const vec3& v   = _get_position( _vertex );
            const vec3& vi  = _get_position( nb_vi->get_ptr() );
            const vec3& vj  = _get_position( nb_vj->get_ptr() );
            
            
            // -> agam+tang 2005, p. 576, formula (17)
            tangent = vi *  ( ( t - 1.0 ) / t );
            tangent += ( v * ( ( t * -2.0 ) + 1.0 ) /  ( t - t*t ) );
            tangent += ( vj * ( t / ( -t + 1.0 ) ) );
            
            assert( ! std::isnan( tangent.x() ) );
            assert( ! std::isnan( tangent.y() ) );
            assert( ! std::isnan( tangent.z() ) );

            // -> agam+tang 2005, p. 576, formula (20)
            p_dp    = vi * ( 2.0 / t );
            p_dp   += ( v  * ( -2.0 / ( t - t*t ) ) );
            p_dp   += ( vj * ( 2.0 / ( -t + 1.0 ) ) );
            
            // -> agam+tang 2005, p. 576, formulas (19,21)
            k.cross( tangent, p_dp );

            sp_float_type t_len = tangent.length();
            
            k /= t_len * t_len * t_len;
            
            const vec3& normal = _get_normal( _vertex );
            sp_float_type kn = dot( k, normal );

            curve_.t         = t;
            curve_.tangent   = tangent;
            curve_.k         = k;
            curve_.kn        = kn;
            
        }
    }
    
    
    
    inline void project_onto_tangent_plane_and_compute_angles()
    {
        // we use the first curve as T0 for formula (25) 
        const vec3& t0      = _curves[ 0 ].tangent;
        const vec3& normal  = _get_normal( _vertex ); 
        
        sp_float_type t0_length = t0.length();
        sp_float_type t0_len_r  = 1.0 / t0_length;
        
        vec3 t0xn;
        t0xn.cross( t0, normal );

        _tangent_plane_u = normalize( t0 );
        _tangent_plane_v = normalize( t0xn );

        _tangent_plane_transform.set_row( 0, _tangent_plane_u );
        _tangent_plane_transform.set_row( 1, _tangent_plane_v );
        
        _tangent_plane_backtransform.set_column( 0, _tangent_plane_u );
        _tangent_plane_backtransform.set_column( 1, _tangent_plane_v );
        
        // project tangents into 2d plane t0 x ( t0 x normal )
        typename std::vector< curve >::iterator
            it      = _curves.begin(),
            it_end  = _curves.end();
        for( ; it != it_end; ++it )
        {
            curve& c    = *it;
            vec2& tk    = c.proj_tangent;
            tk          = _tangent_plane_transform * c.tangent;
            tk         *= t0_len_r;

            assert( ! std::isnan( tk.x() ) );
            assert( ! std::isnan( tk.y() ) );

            tk.normalize();
            
            assert( ! std::isnan( tk.x() ) );
            assert( ! std::isnan( tk.y() ) );
        }
        
        // compute angles between the projected tangets
        //typename std::vector< curve >::iterator
        it      = _curves.begin();
        it_end  = _curves.end();
            
        const vec2& t_ = _curves[0].proj_tangent;
        _curves[0].angle = 0.0;
        for( ++it; it != it_end; ++it )
        {
            curve& c = *it;
            assert( t_.length() == 1.0f );
            assert( c.proj_tangent.length() - 1.0f < 0.0001 );

            sp_float_type& angle = c.angle;
            angle = acos( dot( t_, c.proj_tangent ) ); // FIXME?
            
            assert( ! std::isnan( c.angle ) );

            _used_curves.push_back( &c );
        }

        //_used_curves.sort( angle_compare() );
        
    }
    
    #if 0
    inline void compute_normal()
    {
        vmml::matrix< 3, K, sp_float_type > _tangents;
        for( size_t index = 0; index < _number_of_curves; ++index )
        {
            curve& curve_ = _curves[ index ];
            _tangents.set_column( index, curve_.tangent );
        }
        
        mat3 TTt = _tangents * transpose( _tangents );
        
        vec3 sigma;
        mat3 Vt;
        
        vmml::svdecompose( TTt, sigma, Vt );
        
        size_t min_eigenvalue_index = sigma.find_min_index();
        
        Vt.get_column( min_eigenvalue_index, _curv_normal );
      
        //_get_normal( _vertex ) = _curv_normal;
        
    }
    #endif
    
    inline void compute_curvature( const vec3& normal )
    {
        typename std::list< curve* >::iterator
            it      = _used_curves.begin(),
            it_end  = _used_curves.end();
        for( ; it != it_end; ++it )
        {
            curve& curve_   = **it;
            curve_.kn      = dot( curve_.k, normal );
        }
    
    #if 0
        for( size_t index = 0; index < _number_of_curves; ++index )
        {
            curve& curve_   = _curves[ index ];
            curve_.kn      = dot( curve_.k, normal );
        }
    #endif
    }
        
    void setup_attributes( op_base_type& op, stream_structure& vs )
    {
        op.read( vs, "position",    _get_position );
        op.read( vs, "normal",      _get_normal );
        op.read( vs, "neighbors",   _get_neighbors );
        op.read( vs, "point_index", _get_point_index );

        op.read( vs, "radius",      _get_radius );

        op.write( vs, "k1",      _get_k_max );
        op.write( vs, "k1_dir",  _get_k_max_dir );
        op.write( vs, "k2",      _get_k_min );
        op.write( vs, "k2_dir",  _get_k_min_dir );

        op.write( vs, "axis",           _get_axis, true );
        op.write( vs, "axis_ratio",		_get_axis_ratio, true );
        op.write( vs, "axis_length",	_get_axis_length, true );
    }

    void setup_accessors()
    {
        _get_position.setup();
        _get_normal.setup();
        _get_neighbors.setup();
        _get_point_index.setup();
        
        _get_radius.setup();
        
        _get_k_min.setup();
        _get_k_min_dir.setup();
        _get_k_max.setup();
        _get_k_max_dir.setup();

        _get_axis.setup();
        _get_axis_length.setup();
        _get_axis_ratio.setup();
    }
    
    void set_use_stream_normal( bool use_stream_normal )
    {
        _use_stream_normal = use_stream_normal;
    }
    
    void set_k( size_t k_ )
    {
        _k = k_;
        // -> agam+tang 2005, p. 576, #A(v)
        _number_of_curves = ( _k * ( _k - 1 ) ) / 2;
        _curves.resize( _number_of_curves );
        setup_tuple_indices();
    }
    
    void setup_tuple_indices()
    {
        _curve_tuples.clear();

        const size_t s = _curves.size();

        for( size_t index = 0; index < s; ++index )
        {
            for( size_t j = index + 1; j < s; ++j )
            {
                for( size_t k = j + 1; k < s; ++k )
                {
                    _curve_tuples.push_back( vec3s( index, j, k ) );
                }
            }
        }

        #if 0
        std::vector< vec3s >::const_iterator it = _curve_tuples.begin(),
            it_end = _curve_tuples.end();
        for( ; it != it_end; ++it )
        {
            std::cout << *it << std::endl;
        }
        #endif
    }
    
protected:
    attribute_accessor< vec3 >              _get_position;
    attribute_accessor< vec3 >              _get_normal;
    attribute_accessor< nbh_type >          _get_neighbors;

	attribute_accessor< sp_float_type >     _get_radius;

	attribute_accessor< sp_float_type >     _get_k_min;
	attribute_accessor< vec3 >              _get_k_min_dir;
	attribute_accessor< sp_float_type >     _get_k_max;
	attribute_accessor< vec3 >              _get_k_max_dir;

	attribute_accessor< vec3 >              _get_axis;
	attribute_accessor< sp_float_type >     _get_axis_length;
	attribute_accessor< sp_float_type >     _get_axis_ratio;
    
    attribute_accessor< size_t >            _get_point_index;

    stream_data*                    _vertex;

    size_t                          _number_of_curves;

    std::vector< curve >            _curves;        // A(v) in agam+tang 2005
    std::list< curve* >             _used_curves;   // B(v) in agam+tang 2005

    std::vector< vec3s >            _curve_tuples;
    std::vector< curvature_set >    _curve_sets;

    vec3                            _curv_normal;
    
    bool                            _use_stream_normal;
    
    size_t                          _k; // max. neighborhood size
    
    miao_dirs                       _get_principal_dirs;
    
    vec3                            _tangent_plane_u;
    vec3                            _tangent_plane_v;

    vmml::matrix< 2, 3, sp_float_type >     _tangent_plane_transform;
    vmml::matrix< 3, 2, sp_float_type >     _tangent_plane_backtransform;
    
    paraboloid_test_type                        _paraboloid;
    typename paraboloid_test_type::init_data    _paraboloid_init;

    std::vector< sp_float_type >    _diffs;
    


}; // class compute_accurate_curvature

} // namespace stream_process

#endif

