#include "optimal_transform.hpp"

namespace stream_process
{

optimal_transform::optimal_transform( mapped_point_data& mapped_point_data_ )
    : _mapped_point_data( mapped_point_data_ )
#ifdef SPROCESS_OLD_VMMLIB
    , _transform_4x4(   mat4f::ZERO )
    , _transform(       mat3f::ZERO )
    , _translation(     vec3f::ZERO )
{
#else
{
    _transform_4x4.zero();
    _transform      = 0.0f;
    _translation    = 0.0f;
#endif
    _determine_optimal_transform();
}



optimal_transform::optimal_transform( mapped_point_data& mapped_point_data_, 
    const mat4f& transform )
    : _mapped_point_data( mapped_point_data_ )
    , _transform_4x4( transform )
    , _transform()
    , _translation( _transform_4x4.getTranslation() )
{
    #ifdef SPROCESS_OLD_VMMLIB
    _transform_4x4.get3x3SubMatrix( _transform );
    #else
    _transform_4x4.getSubMatrix( _transform, 0, 0 );
    #endif
}



void
optimal_transform::apply_optimal_transform()
{
	point_info& info          = _mapped_point_data.get_point_info();
	const size_t number_of_points   = info.get_point_count();
	const size_t size_in_bytes      = info.get_size_in_bytes();
	char* cur_point                 = _mapped_point_data.get_data_ptr();
	char* points_end                
        = cur_point + ( number_of_points * size_in_bytes );

    // transform the positions 
    rt_struct_member< vec3f > position 
        = info.get_accessor_for_attribute< vec3f >( "position" );

    stream_point* point;
	for( ; cur_point != points_end; cur_point += size_in_bytes )
	{
        point = reinterpret_cast< stream_point*> ( cur_point );
        vec3f& position_ = point->get( position );

        position_ = position_ + _translation;
		position_ = _transform * position_;
	}
    
    // transform the normals using the inverse transpose
    if ( info.has_attribute( "normal" ) )
    {
        bool ok;
        mat4f inverse;
        ok = _transform_4x4.getInverse( inverse );
        if ( ! ok )
        {
            throw exception( "optimal transform matrix is not invertible.",
                SPROCESS_HERE );
        }
        
        mat4f invtrans( inverse.getTransposed() );
        mat3f invtrans_3x3;
        #ifdef SPROCESS_OLD_VMMLIB
        invtrans.get3x3SubMatrix( invtrans_3x3 );
        #else
        invtrans.getSubMatrix( invtrans_3x3, 0, 0 );
        #endif
        

        rt_struct_member< vec3f > normal 
            = info.get_accessor_for_attribute< vec3f >( "normal" );
        
        cur_point = _mapped_point_data.get_data_ptr();
        for( ; cur_point != points_end; cur_point += size_in_bytes )
        {
            point = reinterpret_cast< stream_point*> ( cur_point );
            vec3f& normal_ = point->get( normal );
            normal_ = invtrans_3x3 * normal_;
        }

    }

    _mapped_point_data.recompute_aabb();
}



void
optimal_transform::apply_inverse_optimal_transform()
{
	const point_info& info          = _mapped_point_data.get_point_info();
	const size_t number_of_points   = info.get_point_count();
	const size_t size_in_bytes      = info.get_size_in_bytes();
	char* cur_point                 = _mapped_point_data.get_data_ptr();
	char* points_end                
        = cur_point + ( number_of_points * size_in_bytes );

    rt_struct_member< vec3f > position 
        = info.get_accessor_for_attribute< vec3f >( "position" );

	vec3f translation = _translation * -1.0f;

    bool is_invertible;
	mat3f transformation_matrix;
    is_invertible = _transform.getInverse( transformation_matrix );
    if ( ! is_invertible )
    {
        std::string error_msg 
            = "invalid transformation matrix - must be invertible.";
        throw exception( error_msg.c_str(), SPROCESS_HERE );
    }

    stream_point* point;
	for( ; cur_point != points_end; cur_point += size_in_bytes )
	{
        point = reinterpret_cast< stream_point*> ( cur_point );
        vec3f& position_ = point->get( position );

		position_ = transformation_matrix * position_;
		position_ = position_ + translation;
	}
}



void
optimal_transform::_determine_optimal_transform()
{
	mat3f mat_covariance_of_model( _get_covariance_3x3() );
	const vec3f center_of_model = _center;
	mat3f eigenvectors;
	vec3f eigenvalues;
	size_t rotation_count;
	
	mat3f covar_dummy_for_jacobi = mat_covariance_of_model;

	vmml::solveJacobi3x3( covar_dummy_for_jacobi, eigenvalues, eigenvectors, rotation_count );

	int index_eigenvalue = 0;
	float value_eigenvalue = 0.0;
	for ( int i = 0 ;  i < 3 ; ++i ) 
	{
		if( eigenvalues[i] > value_eigenvalue )
		{
			value_eigenvalue = eigenvalues[i];
			index_eigenvalue = i;
		}
	}

	const mat3f rotation_basis( mat3f::IDENTITY );
	mat3f rotation_y_90;
    rotation_y_90.set(0.f,0.f, -1.f,0.f,1.f,0.f,1.f,0.f,0.f);
	mat3f rotation_x_90;
    rotation_x_90.set(1.,0., 0.,0.,0.,-1.,0.,1.,0.);
	
	mat3f rotation = rotation_basis;
		
	if( index_eigenvalue == 0 ) rotation = rotation_y_90;
	else if ( index_eigenvalue == 1 ) rotation = rotation_x_90;

	const mat3f transformation = rotation * eigenvectors;
	const vec3f translation = -center_of_model;

	_transform      = transformation;
	_translation    = translation;
    
    _transform_4x4 = mat4f::IDENTITY;
    #ifdef SPROCESS_OLD_VMMLIB
    _transform_4x4.set3x3SubMatrix( _transform );
    #else
    _transform_4x4.setSubMatrix( _transform );
    #endif
    _transform_4x4.setTranslation( _translation );

}



mat3f
optimal_transform::_get_covariance_3x3()
{
	const point_info& info              = _mapped_point_data.get_point_info();
	const size_t& number_of_points      = info.get_point_count();
	const size_t& point_size_in_bytes   = info.get_size_in_bytes();
	const char* model_ptr               = _mapped_point_data.get_data_ptr();
	
	vec3f sum_points = 0.0f;
	vec3f sum_square = 0.0f;  
	vec3f sum_cross = 0.0f;
	const size_t n = number_of_points;
	
	for( size_t i = 0 ; i < n ; ++i )
	{
		const vec3f& point     = *(vec3f*)(model_ptr + i * point_size_in_bytes);
		const vec3f product    =  point * point;
        #ifdef SPROCESS_OLD_VMMLIB
		const vec3f cov_cross_product = vec3f( point.x*point.y, point.y*point.z, point.x*point.z );
        #else
		const vec3f cov_cross_product = vec3f( point.x()*point.y(), point.y()*point.z(), point.x()*point.z() );
        #endif

		sum_points += point;
		sum_square += product;
		sum_cross += cov_cross_product;
	}


	_center = sum_points / n;



#if 0
	const vec3f squares_sum_values  = sum_points * sum_points;

//	vec3f cross_point = vec3f(sum_points.y, sum_points.z, sum_points.x);
	const vec3f crosses_sum_values  = vec3f( sum_points.x * sum_points.y, sum_points.y * sum_points.z, sum_points.z*sum_points.x );
	//sum_points * cross_point;
						
	const vec3f divided_sum_squares = squares_sum_values / n;
	const vec3f divided_sum_crosses	= crosses_sum_values / n;

	 vec3f cov_squares = sum_square - divided_sum_squares;
	 vec3f cov_crosses = sum_cross - divided_sum_crosses;
		
	cov_squares /= n;
	cov_crosses /= n;
	const mat3f covar_mat = mat3f(cov_squares.x, cov_crosses.x, cov_crosses.z,
							cov_crosses.x, cov_squares.y, cov_crosses.y,
							cov_crosses.z, cov_crosses.y, cov_squares.z);
#else
	const vec3f& average_point = sum_points/ n;
	const vec3f& average_squares = sum_square/ n;
	const vec3f& average_crosses = sum_cross/ n;
	const vec3f& average_point_products = average_point * average_point;

//	const vec3f& cross_product_helper =  vec3f( average_point.y, average_point.z, average_point.x );
    #if SPROCESS_OLD_VMMLIB
	const vec3f average_cross_products( average_point.x * average_point.y, average_point.y * average_point.z, average_point.z*average_point.x );
	#else
	const vec3f average_cross_products = vec3f( average_point.x() * average_point.y(), average_point.y() * average_point.z(), average_point.z() * average_point.x() );
    #endif

	const vec3f covar_values_product = average_squares - average_point_products;
	const vec3f covar_values_crosses = average_crosses - average_cross_products;


	mat3f covar_mat;
    #if SPROCESS_OLD_VMMLIB
    covar_mat.set( covar_values_product.x, covar_values_crosses.x, covar_values_crosses.z,
							covar_values_crosses.x, covar_values_product.y, covar_values_crosses.y,
							covar_values_crosses.z, covar_values_crosses.y, covar_values_product.z);
    #else
    covar_mat.set( 
        covar_values_product.x(), covar_values_crosses.x(), covar_values_crosses.z(),
        covar_values_crosses.x(), covar_values_product.y(), covar_values_crosses.y(),
        covar_values_crosses.z(), covar_values_crosses.y(), covar_values_product.z() 
        );
    #endif
    

#endif
	return covar_mat;
}



const vec3f&
optimal_transform::get_translation() const
{
    return _translation;
}



const mat3f&
optimal_transform::get_orientation() const
{
    return _transform;
}



const mat4f&
optimal_transform::get_transform() const
{
    return _transform_4x4;
}



} // namespace stream_process

