#ifndef __STREAM_PROCESS__CURVE_DEBUG_FILE__HPP__
#define __STREAM_PROCESS__CURVE_DEBUG_FILE__HPP__

#include <stream_process/VMMLibIncludes.h>
#include <fstream>

#include <stream_process/exception.hpp>

namespace stream_process
{
/*


format:
- (this) point
- (nb) points
- curves



*/
class curve_debug_file
{
public:
    typedef vmml::vector< 3, size_t >   vec3s;
    
    struct header
    {
        size_t version;
        size_t points;
        size_t curves;
        size_t pces;

        friend std::ostream& operator<<( std::ostream& os, const header& h ) 
        {
            os 
                << h.points << " points, " << h.curves << " curves, "
                << h.pces << " pces." << std::endl;
            return os;
        }
    };

    struct point
    {
        vec3f   position;
        vec3f   normal;
        float   radius;

        friend std::ostream& operator<<( std::ostream& os, const point& p ) 
        {
            os 
                << "point position " << p.position << ", normal " << p.normal 
                << ", radius " << p.radius
                << std::endl;
            return os;
        }
    };
    
    struct curve
    {
        size_t  nb_0;
        size_t  nb_1;
        vec3f   tangent;
        vec3f   k;
        float   kn;

        friend std::ostream& operator<<( std::ostream& os, const curve& c ) 
        {
            os
                << "curve nbs " << c.nb_0 << ", " << c.nb_1
                << ", tangent " << c.tangent << ", k " << c.k 
                << ", kn " << c.kn << std::endl;
            return os;
        }

    };
    
    struct pce
    {
        vec3s   curve_indices;
        float   k1;
        float   k2;
        vec3f   e1;
        vec3f   e2;

        friend std::ostream& operator<<( std::ostream& os, const pce& e ) 
        {
            os
                << "curve indices " << e.curve_indices << ", k1 " << e.k1
                << ", k2 " << e.k2 << ", e1 " << e.e1 << ", e2 " << e.e2 
                << std::endl;
            return os;
        }
    };

    inline curve_debug_file( const std::string& filename );
    inline ~curve_debug_file();
    
    inline void write_header( size_t points, size_t curves );
    inline void write_point( const vec3f& position, const vec3f& normal, float radius );
    inline void write_curve(
        const size_t nb_index_0, 
        const size_t nb_index_1, 
        const vec3f& tangent,
        const vec3f& k, 
        const float kn ); 

    inline void write_pce( const pce& pce_ );


protected:
    size_t          _point_index;
    size_t          _number_of_pces;
    std::fstream    _fstream;
    
    header          _header;

}; // class curve_debug_file


curve_debug_file::curve_debug_file( const std::string& filename )
    : _point_index( std::numeric_limits< size_t >::max() )
    , _fstream()
    , _number_of_pces( 0 )
{
    _fstream.open( filename.c_str(), std::ios_base::out );
    if ( ! _fstream.is_open() )
    {
        throw exception( "curve debug file could not be opened.", SPROCESS_HERE );
    }
    
    _header.version = 1;
}


curve_debug_file::~curve_debug_file()
{
    _header.pces    = _number_of_pces;

    _fstream.seekg( 0 );
    _fstream.write( (char*) &_header, sizeof( header ) );
    _fstream.close();
}



void
curve_debug_file::write_header( size_t points, size_t curves )
{
    _header.points = points;
    _header.curves = curves;

    _fstream.write( (char*) &_header, sizeof( header ) );
}

void
curve_debug_file::write_point( const vec3f& position, const vec3f& normal, const float radius )
{
    assert( _fstream.is_open() );
    
    point p;
    p.position  = position;
    p.normal    = normal;
    p.radius    = radius;
    
    _fstream.write( (char*) &p, sizeof( point ) );
}



void
curve_debug_file::write_curve(
        const size_t nb_index_0, 
        const size_t nb_index_1, 
        const vec3f& tangent,
        const vec3f& k, 
        const float kn )
{
    assert( _fstream.is_open() );
    
    curve c;
    
    c.nb_0      = nb_index_0;
    c.nb_1      = nb_index_1;
    c.tangent   = tangent;
    c.k         = k;
    c.kn        = kn;
    
    _fstream.write( (char*) &c, sizeof( curve ) );
}


inline void
curve_debug_file::write_pce( const pce& pce_ )
{
    _fstream.write( (char*) &pce_, sizeof( pce_ ) );

    ++_number_of_pces;
}

} // namespace stream_process

#endif

