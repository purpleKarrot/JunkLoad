#include "normal_compare_op_algorithm.hpp"

namespace stream_process
{

normal_compare_op_algorithm::normal_compare_op_algorithm()
    : _normal_0( "normal" )
    , _normal_1( "normal_new" )
    , _radius_0( "radius" )
    , _radius_1( "radius_new" )
{
    set_name( "normal compare" );
}


void
normal_compare_op_algorithm::_compute( stream_point* point )
{
    const vec3f& n0 = point->get( _normal_0 );
    const vec3f& n1 = point->get( _normal_1 );
    
    if ( n0 != n1 )
    {
        throw exception( "normals don't match.", SPROCESS_HERE );
    }
    if ( point->get( _radius_0 ) != point->get( _radius_1 ) )
    {
        throw exception( "normals don't match.", SPROCESS_HERE );
    }
}



} // namespace stream_process

