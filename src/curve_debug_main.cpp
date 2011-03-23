#include <stream_process/curve_debug_viewer.hpp>
#include <gloo/glut_window.hpp>
#include <boost/lexical_cast.hpp>

int
main( int argc, char const** argv )
{
    gloo::glut_window glut_window_( "curve debug", 800, 450 );
    
    size_t idx = 0;
    
    if ( argc > 1 )
    {
        idx = boost::lexical_cast< size_t >( argv[1] );
    }
    stream_process::curve_debug_viewer cdv( &glut_window_, idx  );
    glut_window_.start();
}

