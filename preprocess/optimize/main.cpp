//

#include "VMMLibIncludes.h"
#include "mapped_data_set.hpp"
#include "jacobi_solver.hpp"

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cerr << "usage: optimize <junkfile>" << std::endl;
		return 0;
	}

	stream_process::mapped_data_set data_set_(argv[1]);
	stream_process::mapped_data_element& mesh_ = data_set_.get_vertex_map();

	stream_process::attribute_accessor<junk::vec3f> get_position(
			get_attribute(data_set_.get_vertex_element(), "position").offset);

	junk::mat3d _covariance;
	junk::mat4d _transform;
	junk::vec3d _center;

	{
		std::cout << "optimal_transform: computing transformation matrix."
				<< std::endl;

		identity( _transform);

		{
			junk::vec3d sum_points = 0.0f;
			junk::vec3d sum_square = 0.0f;
			junk::vec3d sum_cross = 0.0f;

			junk::vec3d v, product, cov_cross_product;

			typename stream_process::mapped_data_element::iterator it =
					mesh_.begin(), it_end = mesh_.end();
			for (; it != it_end; ++it)
			{
				const junk::vec3f& vf = get_position(*it);
				v.set(vf.x(), vf.y(), vf.z());
				product = v;
				product *= v;

				cov_cross_product.cross(v, v);
				sum_points += v;
				sum_square += product;
				sum_cross += cov_cross_product;
			}

			const size_t vsize = std::distance(mesh_.begin(), mesh_.end());
			assert(vsize != 0);

			_center = sum_points / vsize;

			junk::vec3d avg_point = sum_points / vsize;
			junk::vec3d avg_squares = sum_square / vsize;
			junk::vec3d avg_cross = sum_cross / vsize;
			junk::vec3d avg_product = avg_point * avg_point;

			//vec3 avg_point_cross_avg_point = vec3( avg_point.x() * avg_point.y(), avg_point.y() * avg_point.z(), avg_point.z() * avg_point.x() );
			junk::vec3d avg_point_cross_avg_point;
			avg_point_cross_avg_point.cross(avg_point, avg_point);

			const junk::vec3d covar_values_product = avg_squares - avg_product;
			const junk::vec3d covar_values_crosses = avg_cross
					- avg_point_cross_avg_point;

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

		junk::mat3d eigenvectors;
		junk::vec3d eigenvalues;
		size_t rotation_count;

		vmml::solve_jacobi_3x3<double>(_covariance, eigenvalues, eigenvectors,
				rotation_count);

		size_t largest_eigenvalue_index = eigenvalues.find_max_index();

		junk::mat3d rotation;

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

		std::cout << "cov: " << _covariance << std::endl;
		std::cout << "eigenvalues: " << eigenvalues << std::endl;
		std::cout << "eigenvectors: " << eigenvectors << std::endl;

		std::cout << "optimal transformation: " << _transform << std::endl;
	}

	std::cout << "optimal_transform: transforming model..." << std::endl;

	if (_transform != _transform.IDENTITY)
	{
		junk::mat4f t = _transform;

		typedef stream_process::mapped_data_element::iterator it_t;
		it_t it = mesh_.begin(), it_end = mesh_.end();

		for (; it != it_end; ++it)
		{
			junk::vec3f& v = get_position(*it);

			v = t * v;

			assert(!std::isnan(v.x()));
			assert(!std::isnan(v.y()));
			assert(!std::isnan(v.z()));
		}
	}

	data_set_.get_header().transform = _transform;

	data_set_.compute_aabb();
	junk::save_header(argv[1], data_set_.get_header());
}
