#ifndef __STREAM_PROCESS__ESTIMATE_PARABOLOID_PARAMS_SA__HPP__
#define __STREAM_PROCESS__ESTIMATE_PARABOLOID_PARAMS_SA__HPP__

#include <stream_process/VMMLibIncludes.h>

namespace stream_process
{

template< typename T >
class estimate_paraboloid_params_sa
{
public:
    typedef vmml::vector< 3, T >        vec3t;
    typedef vmml::matrix< 3, 3, T >     mat3t;

    struct estimate
    {
        T a;
        T b;
        T error;

        friend std::ostream& operator<<( std::ostream& os, 
            const estimate& e )
        {
            os << "a " << e.a << ", b " << e.b << ", error " << e.error << "\n";
            return os;
        }

    };
    
    // simulated annealing params
    struct sa_params
    {
        T       temperature;
        T       temperature_step;
        T       random_range;
        size_t  max_k;
    };
    
    
    estimate_paraboloid_params_sa();
    
    void minimize( const sa_params& sap, estimate& e );
    void operator()( const sa_params& sap, estimate& e )
        { minimize( sap, e ); }

    std::vector< vec3t >& get_neighbors();

    // returns a random in range [0,1];
    T get_random();
    // returns a random in range [-range_, range];
    T get_random_in_range( const T& range_ );

    T       compute_error( const estimate& e );
    
protected:
    void    _init_error_matrices( const estimate& e );
    T       _compute_error_for_nb( const vec3t& nb_position );

    std::vector< vec3t >    _nbs;
    
    vec3t       _g;
    mat3t       _A;

    estimate    _best;
    estimate    _new;
    estimate    _guess;

}; // class estimate_paraboloid_params_sa


template< typename T >
estimate_paraboloid_params_sa< T >::estimate_paraboloid_params_sa()
    : _g( 0, 0, -1.0 )
    , _A( mat3t::ZERO )
{}


template< typename T >
void
estimate_paraboloid_params_sa< T >::minimize( const sa_params& sap, 
    estimate& e )
{
    _guess = e;
    _guess.error = compute_error( _guess );

    _best = _guess;
    
    T temp              = sap.temperature;
    const T t_step      = sap.temperature_step;
    const size_t k_max  = sap.max_k;
    
    while( temp > 1.0 )
    {
        size_t k = 0;
        while( k < k_max && _guess.error > ( temp * 0.1 ) )
        {
            _new = _guess;
            _new.a *= 1.0 + get_random_in_range( sap.random_range );
            _new.b *= 1.0 + get_random_in_range( sap.random_range );
            
            _new.error = compute_error( _new );

            if ( _new.error < _best.error )
            {
                _best = _new;
            }

            T rf = get_random();
            T do_overwrite = ( exp( _guess.error - _new.error ) / temp );
            if ( do_overwrite > rf )
            {
                _guess = _new;
            }

            k += 1;
        }
        temp *= t_step;
    }
    
    e = _best;
}



template< typename T >
std::vector< typename estimate_paraboloid_params_sa< T >::vec3t >&
estimate_paraboloid_params_sa< T >::get_neighbors()
{
    return _nbs;
}



template< typename T >
T
estimate_paraboloid_params_sa< T >::get_random()
{
    static const double rmax = RAND_MAX;
    double r = rand() / rmax;
    return static_cast< T >( r );
}



template< typename T >
T
estimate_paraboloid_params_sa< T >::get_random_in_range( const T& range_ )
{
    static const double rmax = RAND_MAX;
    
    double r = rand();
    
    r *= 2.0;
    r -= 1.0;
    
    r *= range_;
    return static_cast< T >( r );
}



template< typename T >
void
estimate_paraboloid_params_sa< T >::_init_error_matrices( const estimate& e )
{
    // note: _g and all other elements of _A are initialized in the ctor
    // _A = mat3t::ZERO;
    // _g.set( 0.0, 0.0, -1.0 )

    _A( 0, 0 )  = 1.0 / ( e.a * e.a );
    _A( 1, 1 )  = -1.0 / ( e.b * e.b );       
}



template< typename T >
T
estimate_paraboloid_params_sa< T >::compute_error( const estimate& e )
{
    _init_error_matrices( e );

    T error_ = 0.0;

    typename std::vector< vec3t >::const_iterator
        it      = _nbs.begin(),
        it_end  = _nbs.end();
    for( ; it != it_end; ++it )
    {
        error_ += _compute_error_for_nb( *it );
    }
    return error_;
}

template< typename T >
T
estimate_paraboloid_params_sa< T >::
_compute_error_for_nb( const vec3t& nb_position )
{
    T d = vmml::dot( _A * nb_position, nb_position );
    d += vmml::dot( _g, nb_position );
    return d * d;
}


} // namespace stream_process

#endif

