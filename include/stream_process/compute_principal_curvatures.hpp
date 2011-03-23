#ifndef __STREAM_PROCESS__COMPUTE_PRINCIPAL_CURVATURES__HPP__
#define __STREAM_PROCESS__COMPUTE_PRINCIPAL_CURVATURES__HPP__

#include <stream_process/stream_process_types.hpp>
#include <stream_process/curve.hpp>

#include <vmmlib/math.hpp>

// an implementation of the principal curvature estimation method 
// proposed by Miao et. al in their 2007 paper
// Differentials-based Segmentation and Parametrization for Point-Sampled Surfaces

namespace stream_process
{

template< typename sp_types_t >
class compute_principal_curvatures : public sp_types_t
{
public:
    STREAM_PROCESS_TYPES
    
    typedef curve< sp_types_t >                 curve_type;
    
#if 0
    void compute( const std::vector< curve_type >& curves, const vec3ub& indices )
    {
        _compute(   & curves[ indices.x() ], 
                    & curves[ indices.y() ],
                    & curves[ indices.z() ] );
    }
#endif



    bool operator()( 
        const curve_type& in_a_,
        const curve_type& in_b_,
        const curve_type& in_c_, 
        sp_float_type&    out_k1,
        sp_float_type&    out_k2,
        sp_float_type&    out_theta
        )
    {
        a = &in_a_;
        b = &in_b_;
        c = &in_c_;

        if ( _setup() == false )
            return false;
        
        if ( _compute_curvature_and_directions() == false )
            return false;
        
        out_k1      = k1;
        out_k2      = k2;
        out_theta   = theta;
        
        return true;
    }
    
#if 0
    void _compute( const curve_type* a_, const curve_type* b_, const curve_type* c_ )
    {
        assert( a_ != 0 );
        assert( b_ != 0 );
        assert( c_ != 0 );

        a = a_;
        b = b_;
        c = c_;
        
        _setup();
        _compute_curvature_and_directions();
        
    }
#endif    

    void get_results( sp_float_type& k1_, sp_float_type& k2_,
        sp_float_type& theta_ )
    {
        k1_     = k1;
        k2_     = k2;
        theta_  = theta;
    }


    bool _setup()
    {
        assert( a != 0 && b != 0 && c != 0 );
        assert( a != b && b != c );

        ka  = a->kn;
        kb  = b->kn;
        kc  = c->kn;

        //assert( a->proj_tangent != b->proj_tangent );
        //assert( a->proj_tangent != c->proj_tangent );

        // compute theta_1 and theta_2, see Miao paper p.751 
        sp_float_type t1d   = dot( a->proj_tangent, b->proj_tangent );
        sp_float_type t2d   = dot( a->proj_tangent, c->proj_tangent );
        
        // acos is only defined for [-1,1]
        if ( t1d >= 1.0 || t1d <= -1.0 )
            return false;

        if ( t2d >= 1.0 || t2d <= -1.0 )
            return false;

        if ( t1d == t2d )
            return false;
            
        theta_1 = acos( t1d );
        theta_2 = acos( t2d );
        
        // fix for div by zero -> k2_divisor
        // 0.017 ~ 1 degree
        if ( fabs( theta_1 - theta_2 ) < 0.017 ) 
            return false;

        //std::cout << "t1d " << t1d << ", t2d " << t2d << std::endl;
       
        return true;
       
        assert( ! std::isnan( theta_1 ) );
        assert( ! std::isinf( theta_1 ) );
        assert( ! std::isnan( theta_2 ) );
        assert( ! std::isinf( theta_2 ) );

    }

    bool _compute_curvature_and_directions()
    {
        const sp_float_type t1mt2       = theta_1 - theta_2;
        const sp_float_type cos_t1mt2   = cos( t1mt2 );
        const sp_float_type sin_t1      = sin( theta_1 );
        const sp_float_type sin_t2      = sin( theta_2 );
        
        // get principal curvature directions
        theta = - theta_2 * 0.5 - atan
            ( 
                (
                    sin( t1mt2 ) * ( ka - kb ) * sin_t1
                )
                /
                ( 
                    - sin( theta_2 ) * ka 
                    + sin( theta_2 ) * kb 
                    + cos_t1mt2 * sin_t1 * ka 
                    - cos_t1mt2 * sin_t1 * kc
                )
            ) * 0.5;

        // get principal curvatures
        const sp_float_type cos_tplust1     = cos( theta + theta_1 );
        const sp_float_type cos_tpt1_sq     = cos_tplust1 * cos_tplust1;

        const sp_float_type sin_tplust1     = sin( theta + theta_1 );
        const sp_float_type sin_tpt1_sq     = sin_tplust1 * sin_tplust1;

        const sp_float_type cos_theta       = cos( theta );
        const sp_float_type cos_theta_sq    = cos_theta * cos_theta;

        const sp_float_type sin_theta       = sin( theta );
        const sp_float_type sin_theta_sq    = sin_theta * sin_theta;
        
        // fixme
        sp_float_type k2_dividend   = - ka * cos_tpt1_sq + cos_theta_sq * kb;
        sp_float_type k2_divisor    = cos_theta_sq * sin_tpt1_sq 
            - sin_theta_sq * cos_tpt1_sq;

        if ( k2_divisor == 0.0 )
            return false;
        
        k2 = k2_dividend / k2_divisor;
#if 0
            ( 
                - ka * cos_tpt1_sq
                + cos_theta_sq * kb
            )
            /
            (
                k2_divisor
                //+ cos_theta_sq * sin_tpt1_sq
                //- sin_theta_sq * cos_tpt1_sq
            );
#endif        
        k1 = - (
                - ka 
                + k2 * sin_theta_sq
            )   
            /
            cos_theta_sq;
            
        assert( ! std::isnan( k1 ) );
        assert( ! std::isinf( k1 ) );
        assert( ! std::isnan( k2 ) );
        assert( ! std::isinf( k2 ) );
        
        return true;
        
    }

protected:

    // inputs 
    const curve_type*   a;
    const curve_type*   b;
    const curve_type*   c;

    // temps

    sp_float_type   ka;
    sp_float_type   kb;
    sp_float_type   kc;

    sp_float_type   theta_1;
    sp_float_type   theta_2;
    
    // results
    sp_float_type   theta;

    sp_float_type   k1;
    sp_float_type   k2;

}; // class compute_principal_curvatures

} // namespace stream_process

#endif

