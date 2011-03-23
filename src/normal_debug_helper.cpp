#include "normal_debug_helper.hpp"

namespace stream_process
{

normal_debug_helper::normal_debug_helper()
{
    _debug_info.open( "normal_debug_info.txt" );
    if ( ! _debug_info.is_open() )
    {
        std::cout << "normal debug helper: opening file failed. Aborting..." << std::endl;
        exit( 2 );
    }
}

normal_debug_helper::~normal_debug_helper()
{
    _debug_info.close();
}


void
normal_debug_helper::setup_old( 
    const rt_struct_member< vec3f >& position, 
    const rt_struct_member< vec3f >& normal, 
    const rt_struct_member< uint32_t >& point_index, 
    //const rt_struct_member< uint32_t >& nb_count, 
    const rt_struct_member< stream_point* >& neighbors,
    const rt_struct_member< float >& nb_distances
    )
{
    _position = position;
    _point_index = point_index;
    _nb_ptrs = neighbors;
    _nb_distances = nb_distances;
    _normal = normal;
    //_nb_count = nb_count;
}
    
void
normal_debug_helper::setup_new( 
    const rt_struct_member< vec3f >& position,
    const rt_struct_member< vec3f >& normal, 
    const rt_struct_member< uint32_t >& point_index, 
    //const rt_struct_member< uint32_t >& nb_count, 
    const rt_struct_member< neighbor >& neighbors
    )
{
    _position = position;
    _point_index = point_index;
    _neighbors = neighbors;
    _normal = normal;
    //_nb_count = nb_count;
}


void
normal_debug_helper::write_debug_info_old( stream_point* point )
{
    const uint32_t& index   = point->get( _point_index );
    const vec3f& pos        = point->get( _position );
    const vec3f& normal     = point->get( _normal );
    
    const uint32_t nb_count = 8u;//point->get( _nb_count );
    stream_point** nb_ptrs  = point->get_ptr( _nb_ptrs );
    float*         nb_dists = point->get_ptr( _nb_distances );


    _debug_info <<      "point          " << index << "\n";
    _debug_info <<      "pos            " << pos << "\n";
    _debug_info <<      "normal         " << normal << "\n";
    for( size_t i = 0; i < nb_count; ++i )
    {
        _debug_info 
            << "nb " << i
            << " index " << nb_ptrs[ i ]->get( _point_index )
            << " dist " << nb_dists[ i ]
            << " pos " << nb_ptrs[ i ]->get( _position ) 
            << "\n";
    }
    _debug_info << "\n" << std::endl;

}


void
normal_debug_helper::write_debug_info_new( stream_point* point )
{
    const uint32_t& index   = point->get( _point_index );
    const vec3f& pos        = point->get( _position );
    const vec3f& normal     = point->get( _normal );
    
    const uint32_t nb_count = 8u;//point->get( _nb_count );
    neighbor* nb             = point->get_ptr( _neighbors );

    _debug_info <<      "point          " << index << "\n";
    _debug_info <<      "pos            " << pos << "\n";
    _debug_info <<      "normal         " << normal << "\n";
    for( size_t i = 0; i < nb_count; ++i )
    {
        #if 0
        _debug_info <<  "nb " << i << "\n";
        _debug_info <<  "nb index       " << nb[ i ].get_point()->get( _point_index ) << "\n";
        _debug_info <<  "nb dist        " << nb[ i ].get_distance() << "\n";
        #else       
        _debug_info 
            << "nb " << i
            << " index " << nb[ i ].get_point()->get( _point_index ) 
            << " dist " << nb[ i ].get_distance()
            << " pos " << nb[ i ].get_point()->get( _position ) 
            << "\n";
        #if 0
        if ( nb[ i ].get_point()->get( _point_index ) == 133 )
        {
            _debug_info << "\n\nXXXXXXXXXXXXXXX" << ( pos - nb[ i ].get_point()->get( _position ) ).length() << "\n\n";
        }
        #endif
        #endif
    }
    _debug_info << "\n" << std::endl;
}

} // namespace stream_process

