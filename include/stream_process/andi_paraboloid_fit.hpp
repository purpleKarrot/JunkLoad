#ifndef __STREAM_PROCESS__ANDI_PARABOLOID_FIT__HPP__
#define __STREAM_PROCESS__ANDI_PARABOLOID_FIT__HPP__

#include <vector>
#include <cstdlib>

#include <stream_process/VMMLibIncludes.h>

namespace stream_process
{

template< typename T >
T get_random()
{
    double r = rand();
    double rmax = RAND_MAX;
    return static_cast< T >( r / rmax );
}


template< typename T >
class andi_paraboloid_fit
{
public:
    typedef vmml::vector< 3, T >        vec3t;
    typedef vmml::vector< 4, T >        vec4t;
    typedef vmml::matrix< 3, 3, T >     mat3t;

    andi_paraboloid_fit()
        : _rot( mat3t::IDENTITY )
        , _A( mat3t::ZERO )
        , _g( 0, 0, -1 )
    {
        
    }

    struct params
    {
        vec3t   position;
        vec3t   normal;
        vec3t   e1; // estimate
        vec3t   e2; // estimate
    };
    
    struct annealing_params
    {
        size_t  k_max;
        T       axis_range;     // range for randomizing axis
        T       angle_range;    // range for randomizing range
        T       temperature;
        T       temperature_step;
        
    };

    struct estimate
    {
        static const bool randomize_angle = false;
    
        T   a;      // init(1)
        T   b;      // init(2)
        T   angle;  // init(3) // to retrieve axis
        T   error;
        
        void randomize( const annealing_params& ap )
        {
            randomize_with_range( a, ap.axis_range );
            randomize_with_range( b, ap.axis_range );
            if (  randomize_angle )
                randomize_with_range( angle, ap.angle_range );
        }
        
        void randomize_with_range( T& value_, const T& range_ )
        {
            T r = get_random< T >();
            r  *= 2.0;
            r  -= 1.0;
            r  *= range_;
            value_ *= 1.0 + r;
        }
        
        friend std::ostream& operator<<( std::ostream& os, 
            const estimate& e )
        {
            os << "a " << e.a << ", b " << e.b << ", angle " << e.angle
                << ", error " << e.error << "\n";
            return os;
        }
        
    };

    params& get_params()
    {
        return _params;
    }
    
    void _compute_error( estimate& e )
    {
        _setup_pb_distance_matrices( e );

        T& error_   = e.error;
        error_      = 0.0;
        typename std::vector< vec3t >::const_iterator
            it      = _neighbors.begin(),
            it_end  = _neighbors.end();
        for( ; it != it_end; ++it )
        {
            error_ += dist_para( *it );
        }
    }
    
    void minimize( annealing_params& ap_, estimate& init_result )
    {
        estimate guess( init_result );
        
        std::cout << "guess " << guess << std::endl;

        _compute_error( guess );

        estimate sbest( guess );
        estimate snew;
        
        T& temp             = ap_.temperature;
        const size_t k_max  = ap_.k_max;
        
        while( temp > 1.0 )
        {
            size_t k = 0;
            while( k < k_max && guess.error > ( temp * 0.1 ) )
            {
                snew = guess;
                snew.randomize( ap_ );
                
                _compute_error( snew );

                if ( snew.error < sbest.error )
                {
                    sbest = snew;
                }
                T rf = get_random< T >();
                T do_overwrite = ( exp( guess.error - snew.error ) / temp );

                //std::cout << "overwrite " << do_overwrite << " <? " << "rf " << rf << std::endl;

                if ( do_overwrite > rf )
                {
                    guess = snew;
                }

                k += 1;
            }
            temp *= ap_.temperature_step;
        }
        
        init_result = sbest;
    }

    T dist_para( const vec3t& nb_position )
    {
        #if 0
        // NOTE: requires _setup_pb_distance_matrices to be called before.
        vec3t v = _rot * nb_position;
        v = _A * v;
        v = _rotT * v;
        T d = dot( v, nb_position );
        
        v = _rot * nb_position;
        d += dot( _g, v );
        
        return d * d;
        #else

        vec3t v = _A * nb_position;
        T d = dot( v, nb_position );
        
        d += dot( _g, nb_position );
        return d * d;

        #endif
    }
    
    std::vector< vec3t >& get_neighbors() { return _neighbors; }

protected:
    void _setup_pb_distance_matrices( estimate& e )
    {
        const T sin_theta   = sin( e.angle );
        const T cos_theta   = cos( e.angle );
    
        // setup rotation matrix
        _rot( 0, 0 ) = cos_theta;
        _rot( 0, 1 ) = -sin_theta;
        _rot( 0, 2 ) = 0.0;

        _rot( 1, 0 ) = sin_theta;
        _rot( 1, 1 ) = cos_theta;
        _rot( 1, 2 ) = 0.0;

        #if 1
        _rot( 2, 0 ) = 0;
        _rot( 2, 1 ) = 0;
        _rot( 2, 2 ) = 1;
        #endif
        
        _rot.transpose_to( _rotT );
        
        const T a =  1.0 / ( e.a * e.a );
        const T b = -1.0 / ( e.b * e.b );
        
        _A = mat3t::ZERO;
        _A( 0, 0 )  = a;
        _A( 1, 1 )  = b;
        
        // g is initialized in the ctor
        _g.set( 0.0, 0.0, -1.0 );
        
    }

    
public: // FIXME //protected:

    params                  _params; // input parameters (e1, e2, normal, pos )
    std::vector< vec3t >    _neighbors;

    mat3t                   _rot;
    mat3t                   _rotT;
    mat3t                   _A;
    vec3t                   _g;
    

}; // class andi_paraboloid_fit

} // namespace stream_process

#endif

