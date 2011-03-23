#ifndef __STREAM_PROCESS__CURVE_DEBUG_VIEWER__HPP__
#define __STREAM_PROCESS__CURVE_DEBUG_VIEWER__HPP__

#include <gloo/view.hpp>

#include <gloo/default_scene.hpp>

#include <gloo/camera.hpp>
#include <gloo/mouse_motion.hpp>

#include <stream_process/curve_debug_file.hpp>

namespace stream_process
{

class curve_debug_viewer
    : public gloo::default_scene
{
public:
    typedef stream_process::curve_debug_file::header    header;
    typedef stream_process::curve_debug_file::point     point;
    typedef stream_process::curve_debug_file::curve     curve;
    typedef stream_process::curve_debug_file::pce       pce;
    typedef curve_debug_file::vec3s                     vec3s;

    typedef gloo::default_scene                         super;

    curve_debug_viewer( gloo::view* _gloo_view, size_t file_index = 0 );
    virtual ~curve_debug_viewer();

    virtual void display();

    virtual void key_update( const gloo::keyboard_state& state );
    
protected:
    
    std::vector< point >    _points;
    std::vector< curve >    _curves;
    std::vector< pce >      _pces;
    
    size_t                  _current_curve;
    size_t                  _draw_normals_toggle;
    bool                    _draw_curve_toggle;
    bool                    _draw_pce_toggle;
    size_t                  _current_pce;
    
    float   _point_scale;
    
    bool                    _same_sign;
    float                   _a2;
    float                   _b2;
    
    void    _load_from_file( size_t file_index );
    
    void    _draw_points();
    void    _draw_point( const point& point, const vec4f& color ) const;
    void    _draw_normals();
    
    void    _draw_curve( size_t curve_index, const vec3f& color_ = vec3f( 1.0, 0.0, 0.0 ) );
    void    _draw_pce( size_t pce_index );
    
    void    _draw_paraboloid( size_t pce_index );
    void    _draw_paraboloid_points( float radius, size_t steps );    

    vec4f   _get_point_color( size_t index );
    

}; // class curve_debug_viewer

} // namespace stream_process

#endif

