#include <stream_process/curve_debug_viewer.hpp>

#include <gloo/draw_simple_geometry.hpp>
#include <boost/lexical_cast.hpp>

namespace stream_process
{

curve_debug_viewer::curve_debug_viewer( gloo::view* _gloo_view, size_t file_index )
    : default_scene( _gloo_view )
    , _point_scale( 0.1 )
    , _current_curve( 0 )
    , _draw_normals_toggle( 0 )
    , _draw_curve_toggle( true )
    , _draw_pce_toggle( 1 )
    , _current_pce( 0 )
{
    _load_from_file( file_index );
}


curve_debug_viewer::~curve_debug_viewer()
{}



void
curve_debug_viewer::display()
{
    default_scene::display();

    glDisable( GL_LIGHTING );
    
    _draw_points();
    
    _draw_normals();
    
    if ( _draw_curve_toggle )
        _draw_curve( _current_curve );

    if ( _draw_pce_toggle )
    {
        _draw_pce( _current_pce );
        _draw_paraboloid( _current_pce );
    }

    glEnable( GL_LIGHTING );
    
}


void
curve_debug_viewer::_draw_points()
{
    vec4f plane;

    point& p = _points[ 0 ];

    _draw_point( p, vec4f( 1.0 ) );

    for( size_t index = 1; index < 9; ++index )
    {
        point& p = _points[ index ];
        _draw_point( p,  _get_point_color( index ) );
    }

}


void 
curve_debug_viewer::_draw_point( const point& p, const vec4f& color ) const
{
    vec4f plane;
    plane.set( p.normal.x(), p.normal.y(), p.normal.z(), -dot( p.position, p.normal ) );
    gloo::draw_plane( plane, p.position, p.radius * _point_scale, color );

}



void
curve_debug_viewer::_draw_normals()
{
    if ( ! _draw_normals_toggle ) 
        return;

    size_t max = _points.size();

    if ( _draw_normals_toggle == 1 )
        max = 1;

    for( size_t index = 0; index < max; ++index )
    {
        point& p = _points[ index ];
        gloo::draw_point_normal( p.normal, p.position, p.radius, _get_point_color( index ) );
        //gloo::draw_point_normal( -p.normal, p.position, p.radius );
    }
}


void
curve_debug_viewer::_draw_pce( size_t pce_index )
{
    const pce& pce_     = _pces[ pce_index ];
    
    const vec3s& cis    = pce_.curve_indices;

    _draw_curve( cis.x(), vec3f( 0.0, 0.0, 1.0 ) );
    _draw_curve( cis.y(), vec3f( 0.0, 0.3, 1.0 ) );
    _draw_curve( cis.z(), vec3f( 0.0, 0.5, 1.0 ) );
    
    const point& p      = _points[ 0 ];
    
    gloo::draw_point_normal( pce_.e1, p.position, p.radius, vec3f( 0.0, 1.0, 1.0, 1.0 ) );
    gloo::draw_point_normal( pce_.e2, p.position, p.radius, vec3f( 0.0, 1.0, 0.5, 1.0 ) );
    gloo::draw_point_normal( -pce_.e1, p.position, p.radius, vec3f( 0.0, 1.0, 1.0, 1.0 ) );
    gloo::draw_point_normal( -pce_.e2, p.position, p.radius, vec3f( 0.0, 1.0, 0.5, 1.0 ) );
    
}



vec4f
curve_debug_viewer::_get_point_color( size_t index )
{
    return vec4f( 0.5 + 0.05 * index );
}



void
curve_debug_viewer::_draw_curve( size_t curve_index, const vec3f& color_ )
{
    assert( curve_index < _curves.size() );

    const curve& c      = _curves[ curve_index ];

    const point& p      = _points[ 0 ];

    assert( c.nb_0 + 1 < _points.size() && c.nb_1 + 1 < _points.size() );

    const point& nb0    = _points[ c.nb_0 + 1 ];
    const point& nb1    = _points[ c.nb_1 + 1 ];
    
    // draw 'curve'
    glBegin( GL_LINES );
    
    glColor3fv( color_.array );
    glVertex3fv( p.position.array );
    glVertex3fv( nb0.position.array );

    glVertex3fv( p.position.array );
    glVertex3fv( nb1.position.array );

    glEnd();

    #if 0
    // draw tangent
    gloo::draw_point_normal( c.tangent, p.position, 1.0, 
        vec4f( 0.0, 1.0, 0.0, 0.0 ) );

    // draw k
    gloo::draw_point_normal( c.k, p.position, p.radius, 
        vec4f( 0.0, 0.0, 1.0, 0.0 ) );
    #endif 

}



void
curve_debug_viewer::key_update( const gloo::keyboard_state& state )
{

    if ( state.is_key_down( '+' ) )
    {
        _point_scale *= 1.1;
    }
    if ( state.is_key_down( '-' ) )
    {
        _point_scale *= 0.89;
    
    }
    
    if ( state.is_key_down( 'o' ) )
    {
        _camera.zoom( 1.1 );
    }

    if ( state.is_key_down( 'p' ) )
    {
        _camera.zoom( 0.9 );
    }

    if ( state.is_key_up( 'n' ) )
    {
        ++_draw_normals_toggle;
        if ( _draw_normals_toggle == 3 )
            _draw_normals_toggle = 0;
    }

    if ( state.is_key_up( 'c' ) )
    {
        _draw_curve_toggle = ! _draw_curve_toggle;
    }
    if ( state.is_key_down( 'x' ) )
    {
        ++_current_curve;
        if ( _current_curve >= _curves.size() )
            _current_curve = 0;
    }
    if ( state.is_key_down( 'v' ) )
    {
        if ( _current_curve == 0 )
            _current_curve = _curves.size() - 1;
        else
            --_current_curve;
    }


    if ( state.is_key_up( 'q' ) )
    {
        if ( _current_pce == 0 )
            _current_pce = _pces.size() - 1;
        else
            --_current_pce;
    }
    if ( state.is_key_up( 'w' ) )
    {
        _draw_pce_toggle = ! _draw_pce_toggle;
        if ( _draw_pce_toggle && _draw_curve_toggle )
            _draw_curve_toggle = 0;
    }
    if ( state.is_key_up( 'e' ) )
    {
        ++_current_pce;
        if ( _current_pce >= _pces.size() )
            _current_pce = 0;
    }


    super::key_update( state );

}



void
curve_debug_viewer::_load_from_file( size_t file_index )
{

    size_t points;
    size_t curves;

    _points.clear();
    _curves.clear();

    std::string filename = "curve_debug.binary." 
        + boost::lexical_cast< std::string >( file_index );
    
    std::ifstream src( filename.c_str() );
    
    if( ! src.is_open() )
    {
        std::cout << "failed to open file '" << filename << "'. " << std::endl;
        exit( 0 );
    }
    else
        std::cout << "opened file '" << filename << "'. " << std::endl;
    

    header  h;
    point   p;
    curve   c;
    pce     pce_; 
    
    
    src.read( (char*)&h, sizeof( header ) );
    
    for( size_t index = 0; index < h.points; ++index )
    {
        src.read( (char*)&p, sizeof( p ) );
        _points.push_back( p );
        std::cout << p << std::endl;
    }
    
    for( size_t index = 0; index < h.curves; ++index )
    {
        src.read( (char*)&c, sizeof( c ) );
        _curves.push_back( c );
        std::cout << c << std::endl;
    }

    for( size_t index = 0; index < h.pces; ++index )
    {
        src.read( (char*)&pce_, sizeof( pce ) );
        _pces.push_back( pce_ );
        std::cout << pce_ << std::endl;
    }

    std::cout
        << "read " << h.points << " vertices, "
        << h.curves << " curves and " 
        << h.pces << " principal curvature estimates." 
        << std::endl;
    
    src.close();
    
    // FIXME 
    for ( size_t index = 1; index < 9; ++index )
    {
        _points[ index ].position -= _points[ 0 ].position;
    }
    _points[0].position = 0.0;
    
}



void
curve_debug_viewer::_draw_paraboloid( size_t pce_index )
{
    const pce& pce_     = _pces[ pce_index ];
    const vec3s& cis    = pce_.curve_indices;
    
    mat4f transform( mat4f::IDENTITY );
    transform.set_row( 0, normalize( pce_.e1 ) );
    transform.set_row( 1, normalize( pce_.e2 ) );
    transform.set_row( 2, _points[ 0 ].normal );
    transform.set_translation( _points[ 0 ].position );
    
    _a2 = fabs( 4.0 / pce_.k1 );
    _b2 = fabs( 4.0 / pce_.k2 );

    _same_sign = false;
    
    if ( pce_.k1 < 0 && pce_.k2 < 0 )
        _same_sign = true;
    else if ( ! (pce_.k1 < 0) && ! (pce_.k2 < 0) )
        _same_sign = true;

    std::cout << "k1 " << pce_.k1 << ", k2 " << pce_.k2 
        << ", a2 " << _a2 << ", b2 " << _b2 << std::endl;

    glPushMatrix();
    glMultMatrixf( transform.array );

    glBegin( GL_POINTS );
    
    glColor3f( 1.0, 0.0, 1.0 );

    //std::cout << "point " << _points[0].position << std::endl;
    //std::cout << "pn " << _points[1].position << std::endl;
    _draw_paraboloid_points( 0.001, 100 );
    _draw_paraboloid_points( 0.002, 100 );
    _draw_paraboloid_points( 0.003, 100 );
    _draw_paraboloid_points( 0.004, 100 );
    _draw_paraboloid_points( 0.005, 100 );
    _draw_paraboloid_points( 0.01, 100 );

    glEnd();

    glPopMatrix();
}

void
curve_debug_viewer::_draw_paraboloid_points( float radius, size_t steps )  
{
    struct pb_gen
    {
        float a2;
        float b2;
        float diff;
        size_t steps;

        void init( float a2_, float b2_, size_t steps_ )
        {
            a2 = a2_;
            b2 = b2_;
            steps = steps_;
            diff = 2.0 * M_PI / steps;
            steps = steps_ + 1;
        }
        
        void gen( float radius, bool same_sign )
        {
            vec3f p( 0.0, 0.0, 0.0 );
            
            float v = 0.0;

            for( size_t index = 0; index < steps; ++index )
            {
                p.x() = cos( v ) * radius;
                p.y() = sin( v ) * radius;

                p.z() = ( p.x() * p.x() / a2  );
                if ( same_sign )
                    p.z() += p.y() * p.y() / b2;
                else
                    p.z() -= p.y() * p.y() / b2;

                glVertex3fv( p.array );
                
                v += diff;
            }
            glVertex3fv( p.array );
            
        }
    };


    pb_gen pb_gen_;
    pb_gen_.init( _a2, _b2, steps );

    float scale = 1.0;

    for( size_t index = 0; index < 20; ++index )
    {
        pb_gen_.gen( radius * scale, _same_sign );
        scale *= 1.1;
    }


}


} // namespace stream_process

