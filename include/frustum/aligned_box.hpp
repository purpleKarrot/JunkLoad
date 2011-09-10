#ifndef FRUSTUM_ALIGNED_BOX_HPP
#define FRUSTUM_ALIGNED_BOX_HPP

#include <frustum/plane.hpp>
#include <frustum/vertex.hpp>
#include <boost/qvm/v_access.hpp>

namespace frustum
{

struct AlignedBox
{
	AlignedBox()
	{
	}

	AlignedBox(const Vertex& a, const Vertex& b)
	{
		set(a, b);
	}

	void set(const Vertex& a, const Vertex& b)
	{
		this->min.x = (std::min)(a.x, b.x);
		this->min.y = (std::min)(a.y, b.y);
		this->min.z = (std::min)(a.z, b.z);

		this->max.x = (std::max)(a.x, b.x);
		this->max.y = (std::max)(a.y, b.y);
		this->max.z = (std::max)(a.z, b.z);
	}

	// for use in frustum computations
	Vertex p_vertex(const Plane& plane)
	{
		return Vertex(
			plane.a < 0 ? this->min.x : this->max.x,
			plane.b < 0 ? this->min.y : this->max.y,
			plane.c < 0 ? this->min.z : this->max.z);
	}

	Vertex n_vertex(const Plane& plane)
	{
		return Vertex(
			plane.a > 0 ? this->min.x : this->max.x,
			plane.b > 0 ? this->min.y : this->max.y,
			plane.c > 0 ? this->min.z : this->max.z);
	}

	Vertex min, max;
};

} // namespace frustum

#endif /* FRUSTUM_ALIGNED_BOX_HPP */
