#include "apss_op.hpp"

#include "string_utils.h"

namespace stream_process
{

apss_op::apss_op()
	: _apss_fit_type(           "apss_fit_type" )
    , _apss_fit_data(           "apss_fit_data" )
    , _nb_weights(				"nb_weights" )
	, _nb_derivative_weights(	"nb_derivative_weights" )
	, _position(				"position" )
	, _point_index(				"point_index" )
	, _neighbors(				"neighbors" )
	, _nb_count(				"nb_count" )
	, _normal(					"normal" )
	, _apss_normal(				"apss_normal" )
    , _min_ref_index(           "min_ref_index" )
    , _max_ref_index(           "max_ref_index" )
    , _proj_position(           "proj_position" )
    , _color(                   "color" )
    , _debug_out_filename_counter( 0 )
    , _debug_out_fail_counter( 0 )
{
    set_name( "sphere fit" );
}



apss_op::~apss_op()
{}



void 
apss_op::pull_push()
{
	stream_point* point;

	while( point = prev->front() )
	{
		prev->pop_front();
        // enter waiting queue
        _in_buffer.push_back( point );
    }
    
    // check queue elements
    while ( ! _in_buffer.empty() ) 
    {
        point = _in_buffer.front();
        
        // only process element if all neighbor normals are present
        const uint32_t max_ref_index = point->get( _max_ref_index );
        
        if ( max_ref_index < prev->smallest_element() )
        {
            _in_buffer.pop_front();
            
            _compute_nb_weights( point );
            _fit_sphere( point );

            _out_buffer.push_back( point );
        } 
        else
            break;
    }

}



bool
apss_op::_fit_sphere( stream_point* point )
{
    #if 1
    return _fit_sphere_without_normals( point );
    #else
    return _fit_sphere_with_normal_constraints( point );
    #endif
/*
    neighbor* neighbors = point->get_ptr( _neighbors );
    double* weights		= point->get_ptr( _nb_weights ); 

    bool ok;
    algebraic_sphere< double >* as;

    #if 1
    as = &_eigen_sphere_fitter; 
    #else
    as = &_normal_constrained_sphere_fitter;
    #endif

    ok = _eigen_sphere_fitter.fit( neighbors, weights );
    vec4f esf_test( _eigen_sphere_fitter.get_plane_or_sphere() );

    ok = _normal_constrained_sphere_fitter.fit( neighbors, weights );
    vec4f ncsf_test( _normal_constrained_sphere_fitter.get_plane_or_sphere() );

    _orient_sphere( point, as );

   
    #if 0

    vec3f point_to_sphere_center;
    std::cout << "---" << std::endl;
    std::cout << "esf " << esf_test << std::endl;

    point_to_sphere_center = vec3f( esf_test.x, esf_test.y, esf_test.z );
    point_to_sphere_center -= point->get( _position );
    std::cout << "esf dist = " << point_to_sphere_center.length() << std::endl;

    std::cout << "ncsf " << ncsf_test << std::endl;
    
    point_to_sphere_center = vec3f( ncsf_test.x, ncsf_test.y, ncsf_test.z );
    point_to_sphere_center -= point->get( _position );
    std::cout << "ncsf dist = " << point_to_sphere_center.length() << std::endl;

    #endif
    
    #if 0
    if ( ! ok )
    {
        throw exception( "could not find a fit for current point", 
            SPROCESS_HERE );
    }
    #endif
            
    vec4f& apss_fit_data    = point->get( _apss_fit_data );
    apss_fit_data = as->get_plane_or_sphere();
        
    uint8_t& apss_fit_type  = point->get( _apss_fit_type );
    apss_fit_type = as->get_as_state();

    vec3f& proj_position  = point->get( _proj_position );

    proj_position = as->project(
        point->get( _position ) 
            );

    vec3f& pos  = point->get( _position );
    
    const uint32_t& index = point->get( _point_index );
        
    #if 0
    std::cout << "euclidean distance " << 
        _eigen_sphere_fitter.get_euclidean_distance( pos ) 
        << std::endl;

    std::cout << "euclidean distance n " << 
        _normal_constrained_sphere_fitter.get_euclidean_distance( pos ) 
        << std::endl;
    #endif
    
    //_print_debug_data( point );
    //_write_out_debug_data( point );
      
    point->get( _normal ) = _DEBUG_compare_normals( point, &_eigen_sphere_fitter );
    //point->get( _normal ) = _DEBUG_compare_normals( point, &_normal_constrained_sphere_fitter );
    return ok;
*/    
}


void
apss_op::_write_debug_data( stream_point* point, algebraic_sphere< double >* as, 
    bool ok)
{
    vec4ub& color       = point->get( _color );
    
    vec4ub color_ok( 128u, 128u, 128u, 255u );
    vec4ub color_failed( 255u, 0u, 0u, 255u );
    
    // FIXME DEBUG
    if ( ok )
    {
        vec3f& position     = point->get( _position );
        vec3f& pposition    = point->get( _proj_position );
        vec3f& normal       = point->get( _normal );

        vec4f sop           = as->get_plane_or_sphere();

        pposition            = sop.projectPointOntoSphere( position );
        // sop.getSphereCenter() + normal * sop.radius; 
        color               = color_ok;//.set( 255u, 0u, 0u, 255u );
        
        vec4f& sphere       = point->get( _apss_fit_data );
        sphere              = as->get_plane_or_sphere();
        
        uint8_t& ftype      = point->get( _apss_fit_type );
        ftype               = as->get_as_state();
        
    }
    else
    {
        ++_debug_out_fail_counter;
        vec3f& position         = point->get( _position );
        vec3f& proj_position    = point->get( _proj_position );
        proj_position          = position;
        color = color_failed;//.set( 255u, 255u, 255u, 255u );

        uint8_t& ftype      = point->get( _apss_fit_type );
        ftype               = 0u;

    }
}



void
apss_op::_estimate_normals( stream_point* point )
{
    const vec3f& position   = point->get( _position );
    vec3f& normal           = point->get( _apss_normal );
    
    normal = _eigen_sphere_fitter.get_gradient( vec3d( position ) );
}




bool
apss_op::_fit_sphere_without_normals( stream_point* point )
{
    neighbor* neighbors = point->get_ptr( _neighbors );
    double* weights		= point->get_ptr( _nb_weights ); 

    bool ok = _eigen_sphere_fitter.fit( neighbors, weights );
    
    _estimate_normals( point );

    _write_debug_data( point, &_eigen_sphere_fitter, ok );

    return ok;
}



bool
apss_op::_fit_sphere_with_normal_constraints( stream_point* point )
{
    neighbor* neighbors = point->get_ptr( _neighbors );
    double* weights		= point->get_ptr( _nb_weights ); 

    bool ok = _normal_constrained_sphere_fitter.fit( neighbors, weights );

    _orient_sphere( point, &_normal_constrained_sphere_fitter );
    
    _write_debug_data( point, &_normal_constrained_sphere_fitter, ok );

    return ok;
}


void
apss_op::_orient_sphere( stream_point* point, algebraic_sphere< double >* as )
{
    stream_point* nb = point->get( _neighbors ).get_point();
    const vec3f& normal = nb->get( _normal );
    if ( normal.dot( as->get_gradient( nb->get( _position ) ) ) < 0.0 )
    {
        for( size_t index = 0; index < 5; ++index )
        {
            as->_coefficients[index] = -as->_coefficients[index];
        } 
    }
}



stream_point*
apss_op::front()
{
	return _out_buffer.empty() ? 0 : _out_buffer.front();
}


void
apss_op::pop_front()
{
	_out_buffer.pop_front();
}



void
apss_op::clear_stage()
{
    stream_point* point;
    // check queue elements
    while ( ! _in_buffer.empty() ) 
    {
        point = _in_buffer.front();
        _in_buffer.pop_front();
        
        _compute_nb_weights( point );
        _fit_sphere( point );

        _out_buffer.push_back( point );
    }
}



size_t
apss_op::size() const
{
	return _out_buffer.size();
}



void
apss_op::setup_stage_0()
{
#if 0
	// we want at least 16 nbs
    var_map::iterator it = _config->find( "nb-count" );
    size_t max_neighbors = 0;
    if( it != _config->end() )
        max_neighbors = (*it).second.as< size_t >();
    else
        throw exception( "nb-count config value not found.", SPROCESS_HERE );
    if ( max_neighbors < 16 ) 
        (*it).second.as< size_t >() = 16;    
#endif

}



void
apss_op::setup_stage_1()
{
    var_map::iterator it = _config->find( "nb-count" );
    size_t nb_count = 0;
    if( it != _config->end() )
        nb_count = (*it).second.as< size_t >();
    else
        throw exception( "nb-count config value not found.", SPROCESS_HERE );
	

    _reserve( _apss_fit_type, IO_WRITE_TO_OUTPUT );
	_reserve_array( _apss_fit_data, SP_FLOAT_32, 4, IO_WRITE_TO_OUTPUT );
    
	// reserve weights member in rt_struct
	_reserve_array( _nb_weights, SP_FLOAT_64, nb_count );
	_reserve_array( _nb_derivative_weights, SP_FLOAT_64, nb_count );

    _reserve_array( _normal, SP_FLOAT_32, 3, IO_WRITE_TO_OUTPUT );
    _reserve_array( _apss_normal, SP_FLOAT_32, 3, IO_WRITE_TO_OUTPUT );

    _reserve_array( _proj_position, SP_FLOAT_32, 3, IO_WRITE_TO_OUTPUT );

	_eigen_sphere_fitter.set_nb_count( nb_count );
	_normal_constrained_sphere_fitter.set_nb_count( nb_count );

}



void
apss_op::setup_stage_2()
{}



void
apss_op::prepare_processing()
{}



void
apss_op::prepare_shutdown()
{
    std::cout << "failed : " << _debug_out_fail_counter << std::endl;
}



void
apss_op::_compute_nb_weights( stream_point* point )
{
	/*
	*	weight function
	*
	*	w_i(x) = op( distance( point, p_i ) / h_i( point ) )
	*	op =  ( 1 - xSquared ) to the power of 4 if ( x < 1 )
	*		  0 otherwise
	*
	*/
	
	uint32_t nb_count			= point->get( _nb_count );
	neighbor* neighbors			= point->get_ptr( _neighbors );
	double* weights             = point->get_ptr( _nb_weights );
	double*	derivative_weights	= point->get_ptr( _nb_derivative_weights );
	
	// FIXME???
	double max_dist = ( neighbors + nb_count - 1 )->get_distance();
	
	double s = 1.0 / max_dist;
	
	neighbor* current_nb		= neighbors;
	neighbor* nb_end			= neighbors + nb_count;
	double*   cur_weight		= weights;
	double*   cur_der_weight	= derivative_weights;
	
	for( ; current_nb != nb_end; ++current_nb, ++cur_weight, ++cur_der_weight )
	{
		double& weight		= *cur_weight;
		double& der_weight	= *cur_der_weight;
		
		weight = s * current_nb->get_distance();

		// compute derivative weight 
		if ( weight <= 0.0 || weight >= 1.0 )
            der_weight = 0.0;
		else
		{
			der_weight = 1.0 - weight;
			der_weight = -4.0 * der_weight * der_weight * der_weight;

 			der_weight *= s;
		}
        
		// compute weight
		weight = 1.0 - weight;
        
		if ( weight < 0.0 ) 
			weight = 0.0;
		else if ( weight > 1.0 )
			weight = 1.0;
		weight *= weight;
		weight *= weight;

        #if 0
        std::cout 
            << "distance " << current_nb->get_distance() << "\n"
            << "weight " << weight << "\n"
            << "dw " << der_weight << "\n"
            << std::endl;
        #endif
	}
}



void
apss_op::_print_debug_data( stream_point* point )
{
	std::cout 
		<< "query point "       << point->get( _position ) << "\n"
		<< "plane_or_sphere "   << (size_t) point->get( _apss_fit_type ) 
		<< " " << point->get( _apss_fit_data )<< "\n"
		<< "neighbors: \n";

    #if 0
	uint32_t nb_count	= point->get( _nb_count );
	neighbor* nb		= point->get_ptr( _neighbors );
	neighbor* nb_end	= nb + nb_count; 

	for ( ; nb != nb_end; ++nb )
	{
		std::cout << "    pos " << nb->get_point()->get( _position ) << "\n";
	}
    #endif
	std::cout << std::endl; 
}



void
apss_op::_write_out_debug_data( stream_point* point )
{
    std::string filename = "sphere_fit_test_out.";
    filename += string_utils::to_string( _debug_out_filename_counter );
    if ( ++_debug_out_filename_counter > 20 )
        _debug_out_filename_counter = 0;
    filename += ".txt";
    
	std::ofstream out;   
	out.open( filename.c_str() );

	const vec3f& pos = point->get( _position );
	const vec4f& sph = _eigen_sphere_fitter.get_plane_or_sphere();

	out << pos.x << " " << pos.y << " " << pos.z << "\n" 
		<< _eigen_sphere_fitter.get_as_state() << "\n"
		<< sph.x << " " << sph.y << " " << sph.z << " " << sph.w << "\n";
		
		
	uint32_t nb_count	= point->get( _nb_count );
	neighbor* nb		= point->get_ptr( _neighbors );
	neighbor* nb_end	= nb + nb_count; 

	for ( ; nb != nb_end; ++nb )
	{
		const vec3f& nb_pos = nb->get_point()->get( _position );
		out << nb_pos.x << " " << nb_pos.y << " " << nb_pos.z << " " 
			<< nb->get_distance() << "\n";
	}
	out << std::endl; 

	out.close();
}



vec3f&
apss_op::_DEBUG_compare_normals( stream_point* point, 
    algebraic_sphere< double >* as )
{
    const vec3f& position = point->get( _position );
    vec4f sphere = as->get_plane_or_sphere();
    vec3f& normal = reinterpret_cast< vec3f& >( sphere.x );
    float& radius = sphere.w;
    
    float d = normal.length();
    normal /= d;
    // d = as->get_sphere_radius() - d;
    if ( as->_u4 < 0.0 )
        normal = -normal;
        
    vec3f& sp_normal = point->get( _normal );
    #if 0
    std::cout 
        << "n   " << sp_normal << "\n"
        << "nx  " << normal << "\n"
        << "dot " << normal.dot( sp_normal ) << "\n"
        << std::endl;
    #endif
    return normal;
}



} // namespace stream_process

