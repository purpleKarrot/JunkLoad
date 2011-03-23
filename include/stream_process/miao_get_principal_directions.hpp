#ifndef __STREAM_PROCESS__MIAO_GET_PRINCIPAL_DIRECTIONS__HPP__
#define __STREAM_PROCESS__MIAO_GET_PRINCIPAL_DIRECTIONS__HPP__

#include <stream_process/stream_process_types.hpp>

#include <vmmlib/math.hpp>

namespace stream_process
{

template< typename sp_types_t >
class miao_get_principal_directions : public sp_types_t
{
public:
    STREAM_PROCESS_TYPES

    struct params
    {
        sp_float_type   ka;
        sp_float_type   kb;
        sp_float_type   kc;
        sp_float_type   theta_1;
        sp_float_type   theta_2;

        friend std::ostream& operator<<( std::ostream& os, const params& r )
        {
            os
                << "theta_1 " << r.theta_1
                << ", theta_2 " << r.theta_2
                << ", ka " << r.ka << ", kb " << r.kb << ", kc " << r.kc;
            return os;
        }

    };
    
    struct results
    {
        sp_float_type   k1;
        sp_float_type   k2;
        sp_float_type   theta;
        
        friend std::ostream& operator<<( std::ostream& os, const results& r )
        {
            os << "theta: " << r.theta << ", k1 " << r.k1 << ", k2 " << r.k2;
            return os;
        }
    };

    void operator()( const params& params_, results& results_ )
    {
        const sp_float_type ka = params_.ka;
        const sp_float_type kb = params_.kb;
        const sp_float_type kc = params_.kc;
        const sp_float_type t1 = params_.theta_1;
        const sp_float_type t2 = params_.theta_2;

        sp_float_type&  theta   = results_.theta;
        sp_float_type&  k1      = results_.k1;
        sp_float_type&  k2      = results_.k2;
        
        const sp_float_type t1mt2       = t1 - t2;
        const sp_float_type cos_t1mt2   = cos( t1mt2 );
        const sp_float_type sin_t1      = sin( t1 );
        const sp_float_type sin_t2      = sin( t2 );
        
        // get principal curvature directions
        theta = - t2 * 0.5 - atan
            ( 
                (
                    sin( t1mt2 ) * ( ka - kb ) * sin_t1
                )
                /
                ( 
                    - sin( t2 ) * ka 
                    + sin( t2 ) * kb 
                    + cos_t1mt2 * sin_t1 * ka 
                    - cos_t1mt2 * sin_t1 * kc
                )
            ) * 0.5;

        // get principal curvatures
        const sp_float_type cos_tplust1 = cos( theta + t1 );
        const sp_float_type cos_tpt1_sq = cos_tplust1 * cos_tplust1;

        const sp_float_type sin_tplust1 = sin( theta + t1 );
        const sp_float_type sin_tpt1_sq = sin_tplust1 * sin_tplust1;

        const sp_float_type cos_theta   = cos( theta );
        const sp_float_type cos_theta_sq    = cos_theta * cos_theta;

        const sp_float_type sin_theta   = sin( theta );
        const sp_float_type sin_theta_sq    = sin_theta * sin_theta;
        
        k2 = 
            ( 
                - ka * cos_tpt1_sq
                + cos_theta_sq * kb
            )
            /
            (
                + cos_theta_sq * sin_tpt1_sq
                - sin_theta_sq * cos_tpt1_sq
            );
        
        k1 = - (
                - ka 
                + k2 * sin_theta_sq
            )   
            /
            cos_theta_sq;
        

    }
    
protected:

}; // class miao_get_principal_directions

} // namespace stream_process

#endif

