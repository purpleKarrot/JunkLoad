
#include <stream_process/andi_paraboloid_fit.hpp>
#include <stream_process/estimate_paraboloid_params_sa.hpp>

using stream_process::vec3f;
using stream_process::mat3f;

struct para_params
{
    para_params()
        : a( 4 )
        , b( 2 )
        , alpha( M_PI / 4.0 )
        , count( 2 )
    {}
    
    float   a;
    float   b;
    float   alpha;
    size_t  count;
};



void
produce_test_data( std::vector< vec3f >& nbs )
{
    nbs.push_back( vec3f( -2.8284, -0.0000,    -0.7500 ) );
    nbs.push_back( vec3f( -1.4142, 1.4142,     0.2500 ) );
    nbs.push_back( vec3f( 0,       2.8284,    -0.75 ) );
    nbs.push_back( vec3f( -1.4142, 0,          -0.1875 ) );
    nbs.push_back( vec3f( 0,       1.4142,     -0.1875 ) );
    nbs.push_back( vec3f( -1.4142, -1.4142,    -1 ) );
    nbs.push_back( vec3f( 0,        0,          0 ) );
    nbs.push_back( vec3f( 1.4142,   1.4142,     -1 ) );
    nbs.push_back( vec3f( 0.0,      -1.4142,    -0.1875 ) );
    nbs.push_back( vec3f( 1.4142,   0,          -0.1875 ) );
}

void
produce_para( std::vector< vec3f >& nbs, const para_params& pp_ )
{
    const int count = pp_.count;

    mat3f rot_z( mat3f::IDENTITY );
    rot_z( 0, 0 ) = cos( pp_.alpha );
    rot_z( 0, 1 ) = -sin( pp_.alpha );

    rot_z( 1, 0 ) = sin( pp_.alpha );
    rot_z( 1, 1 ) = cos( pp_.alpha );
    
    float a2 = pp_.a * pp_.a;
    float b2 = pp_.b * pp_.b;
    
    for( int i = -count; i <= count; ++i )
    {
        for( int j = -count; j <= count; ++j )
        {
            nbs.push_back( vec3f( i, j, i*i/a2 - j*j/b2 ) );
        }
    }
    
    std::vector< vec3f >::iterator
        it      = nbs.begin(),
        it_end  = nbs.end();
    for( size_t index = 0; it != it_end; ++it, ++index )
    {
        (*it) = rot_z * (*it);
        std::cout << "nb " << index << ": " << *it << std::endl;
    }
   
    std::cout << " generated " << nbs.size() << " neighbors " << std::endl;
}



int
main( int argc, const char* argv[] )
{
    #define NEW_IMPL
    
    #ifdef NEW_IMPL
    typedef stream_process::estimate_paraboloid_params_sa< float >    apfit;
    #else
    typedef stream_process::andi_paraboloid_fit< float >    apfit;
    #endif

    srand( time( 0 ) );

    apfit fitter;

    para_params p;

    p.a     = 4;
    p.b     = 2;
    p.alpha = 0.0; //M_PI / 4;
    p.count = 2;

    produce_para( fitter.get_neighbors(), p );

    #ifdef NEW_IMPL
    apfit::sa_params ap;

    ap.max_k                = 1000;
    ap.random_range         = 0.2;
    #else

    apfit::annealing_params ap;
    ap.k_max            = 1000;
    ap.axis_range       = 0.2;

    #endif
    
    ap.temperature      = 1000;
    ap.temperature_step = 0.5;
    
    apfit::estimate e;
    e.a     = 1.0;
    e.b     = 1.0;
    
    
    #ifdef NEW_IMPL
    e.error = fitter.compute_error( e );
    #else
    fitter._compute_error( e );
    #endif
    std::cout << "guess error " << e.error << std::endl;
    
    fitter.minimize( ap, e );
    
    std::cout << e << std::endl;


}

