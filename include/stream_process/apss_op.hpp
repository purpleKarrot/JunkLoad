#ifndef __STREAM_PROCESS__APSS_OP__HPP__
#define __STREAM_PROCESS__APSS_OP__HPP__

#include "stream_op.h"
#include "stream_point.h"
#include "neighbor.h"

#include "eigen_sphere_fitter.hpp"
#include "normal_constrained_sphere_fitter.hpp"

namespace stream_process
{

class apss_op : public stream_op
{
public:
	apss_op();
	virtual ~apss_op();

    virtual void pull_push();
    virtual stream_point* front();

    virtual void pop_front();
    virtual void clear_stage();
    
    // smallest unprocessed element in this or previous stages
    inline virtual size_t smallest_element();	
    // smallest reference of any unprocessed element in this or previous stages
    inline virtual size_t smallest_reference();	

    virtual size_t size() const;

    virtual void setup_stage_0();
    virtual void setup_stage_1();     
    virtual void setup_stage_2();

    virtual void prepare_processing();
    virtual void prepare_shutdown();

protected:
	void _compute_nb_weights( stream_point* point );
    bool _fit_sphere( stream_point* point );

    void _estimate_normals( stream_point* point );

    bool _fit_sphere_without_normals( stream_point* point );
    bool _fit_sphere_with_normal_constraints( stream_point* point );

    void _orient_sphere( stream_point* point, algebraic_sphere< double >* as );

    vec3f& _DEBUG_compare_normals( stream_point* point, 
        algebraic_sphere< double >* as );
	void _print_debug_data( stream_point* point );
	void _write_out_debug_data( stream_point* point );

    void _write_debug_data( stream_point* point, algebraic_sphere< double >* as, 
        bool ok );
          	
    IndexQueue                          _z_queue;
    std::deque< stream_point* >         _in_buffer;
	std::deque< stream_point* >			_out_buffer;

	rt_struct_member< uint8_t >			_apss_fit_type;
    rt_struct_member< vec4f >           _apss_fit_data;
	rt_struct_member< double >			_nb_weights;
	rt_struct_member< double >			_nb_derivative_weights;
    rt_struct_member< vec3f >           _position;
    rt_struct_member< uint32_t >        _point_index;
    rt_struct_member< neighbor >        _neighbors;
    rt_struct_member< uint32_t >        _nb_count;
    rt_struct_member< vec3f >           _normal;
    rt_struct_member< vec3f >           _apss_normal;
    rt_struct_member< uint32_t >        _min_ref_index;
    rt_struct_member< uint32_t >        _max_ref_index;
    rt_struct_member< vec3f >           _proj_position;
    rt_struct_member< vec4ub >          _color;
	
	eigen_sphere_fitter< double >		_eigen_sphere_fitter;
	normal_constrained_sphere_fitter< double > _normal_constrained_sphere_fitter;
	
    size_t                              _debug_out_filename_counter;
    
    size_t                              _debug_out_fail_counter;
    
}; // class apss_op


// inline and template functions

inline size_t
apss_op::smallest_element()
{
    return ( prev ) ? prev->smallest_element() : UINT_MAX;
}



inline size_t
apss_op::smallest_reference()
{
    return ( prev ) ? prev->smallest_reference() : UINT_MAX;
}

} // namespace stream_process

#endif

