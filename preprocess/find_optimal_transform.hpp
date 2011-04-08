#ifndef __STREAM_PROCESS___OPTIMAL_TRANSFORM__HPP__
#define __STREAM_PROCESS___OPTIMAL_TRANSFORM__HPP__

#include "VMMLibIncludes.h"

namespace stream_process
{
template<typename vertex_t, typename vertex_accessor_t, typename container_t>
class optimal_transform
{
public:
	typedef vertex_t vertex_type;
	typedef typename vertex_t::value_type value_type;
	typedef vertex_accessor_t accessor_type;
	typedef container_t container_type;

	typedef vmml::vector<3, value_type> vec3;
	typedef vmml::matrix<3, 3, value_type> mat3;
	typedef vmml::matrix<4, 4, value_type> mat4;

	typedef vmml::vector<3, double> vec3d;
	typedef vmml::matrix<3, 3, double> mat3d;
	typedef vmml::matrix<4, 4, double> mat4d;

	bool analyze(const container_t& mesh_); // compute the optimal transform
	void apply(container_t& mesh_);

	const mat4& get_transformation_matrix();
	void set_transformation_matrix(const mat4& transform_);

	// if an initialized or custom configured accessor is needed
	void set_accessor(accessor_type& accessor_)
	{
		_accessor = accessor_;
	}

protected:
	void _compute_covar(const container_t& mesh_);

	mat3d _covariance;
	mat4d _transform;
	vec3d _center;

	mat4 _transform_user_type;

	accessor_type _accessor;

}; // class optimal_transform

template<typename vertex_t, typename vertex_accessor_t, typename container_t>
bool optimal_transform<vertex_t, vertex_accessor_t, container_t>::analyze(
		const container_t& mesh_)
{
	std::cout << "optimal_transform: computing transformation matrix."
			<< std::endl;

	identity(_transform);

	_compute_covar(mesh_);

	mat3d eigenvectors;
	vec3d eigenvalues;
	size_t rotation_count;

	vmml::solve_jacobi_3x3<double>(_covariance, eigenvalues, eigenvectors,
			rotation_count);

	size_t largest_eigenvalue_index = eigenvalues.find_max_index();

	mat3d rotation;

	double r0[] =
	{ 0, 0, -1, 0, 1, 0, 1, 0, 0 };
	double r1[] =
	{ 1, 0, 0, 0, 0, -1, 0, 1, 0 };

	switch (largest_eigenvalue_index)
	{
	case 0:
		rotation.set(r0, r0 + 9);
		break;
	case 1:
		rotation.set(r1, r1 + 9);
		break;
	default:
		identity(rotation);
		//rotation *= -1.0;
		break;
	}

	_transform.set_sub_matrix(rotation * eigenvectors);
	_transform.set_translation(_center);

#if 0
	std::cout << "cov: " << _covariance << std::endl;
	std::cout << "eigenvalues: " << eigenvalues << std::endl;
	std::cout << "eigenvectors: " << eigenvectors << std::endl;

	std::cout << "optimal transformation: " << _transform << std::endl;
#endif

	return true;

}

template<typename vertex_t, typename vertex_accessor_t, typename container_t>
void optimal_transform<vertex_t, vertex_accessor_t, container_t>::apply(
		container_t& mesh_)
{
	std::cout << "optimal_transform: transforming model..." << std::endl;

	if (_transform == _transform.IDENTITY)
		return;

	mat4 t = get_transformation_matrix();

	typename container_t::iterator it = mesh_.begin(), it_end = mesh_.end();
	vec3d vd;
	it = mesh_.begin(), it_end = mesh_.end();
	for (; it != it_end; ++it)
	{
		vec3& v = _accessor(*it);

		v = t * v;

		assert(!std::isnan(v.x()));
		assert(!std::isnan(v.y()));
		assert(!std::isnan(v.z()));
	}

}

template<typename vertex_t, typename vertex_accessor_t, typename container_t>
void optimal_transform<vertex_t, vertex_accessor_t, container_t>::_compute_covar(
		const container_t& mesh_)
{
	vec3d sum_points = 0.0f;
	vec3d sum_square = 0.0f;
	vec3d sum_cross = 0.0f;

	vec3d v, product, cov_cross_product;

	typename container_t::const_iterator it = mesh_.begin(), it_end =
			mesh_.end();
	for (; it != it_end; ++it)
	{
		const vec3& vf = _accessor(*it);
		v.set(vf.x(), vf.y(), vf.z());
		product = v;
		product *= v;

		//cov_cross_product.set( v.x() * v.y(), v.y() * v.z(), v.x() * v.z() );
		cov_cross_product.cross(v, v);
		sum_points += v;
		sum_square += product;
		sum_cross += cov_cross_product;
	}

	const size_t vsize = std::distance(mesh_.begin(), mesh_.end());
	assert(vsize != 0);

	_center = sum_points / vsize;

	vec3d avg_point = sum_points / vsize;
	vec3d avg_squares = sum_square / vsize;
	vec3d avg_cross = sum_cross / vsize;
	vec3d avg_product = avg_point * avg_point;

	//vec3 avg_point_cross_avg_point = vec3( avg_point.x() * avg_point.y(), avg_point.y() * avg_point.z(), avg_point.z() * avg_point.x() );
	vec3d avg_point_cross_avg_point;
	avg_point_cross_avg_point.cross(avg_point, avg_point);

	const vec3d covar_values_product = avg_squares - avg_product;
	const vec3d covar_values_crosses = avg_cross - avg_point_cross_avg_point;

#if 1
	_covariance(0, 0) = covar_values_product.x();
	_covariance(0, 1) = covar_values_crosses.x();
	_covariance(0, 2) = covar_values_crosses.z();
	_covariance(1, 0) = covar_values_crosses.x();
	_covariance(1, 1) = covar_values_product.y();
	_covariance(1, 2) = covar_values_crosses.y();
	_covariance(2, 0) = covar_values_crosses.z();
	_covariance(2, 1) = covar_values_crosses.y();
	_covariance(2, 2) = covar_values_product.z();

#else

	_covariance.set(
			covar_values_product.x(), covar_values_crosses.x(), covar_values_crosses.z(),
			covar_values_crosses.x(), covar_values_product.y(), covar_values_crosses.y(),
			covar_values_crosses.z(), covar_values_crosses.y(), covar_values_product.z()
	);
#endif
}

template<typename vertex_t, typename vertex_accessor_t, typename container_t>
void optimal_transform<vertex_t, vertex_accessor_t, container_t>::set_transformation_matrix(
		const mat4& transform_)
{
	_transform = transform_;
}

template<typename vertex_t, typename vertex_accessor_t, typename container_t>
const typename optimal_transform<vertex_t, vertex_accessor_t, container_t>::mat4&
optimal_transform<vertex_t, vertex_accessor_t, container_t>::get_transformation_matrix()
{
	for (size_t i = 0; i < 4; ++i)
	{
		for (size_t j = 0; j < 4; ++j)
		{
			_transform_user_type(i, j) = static_cast<value_type> (_transform(i,
					j));
		}
	}
	return _transform_user_type;
}

} // namespace stream_process

#endif
