#ifndef FRUSTUM_FRUSTUM_HPP
#define FRUSTUM_FRUSTUM_HPP

#include <frustum/matrix.hpp>
#include <frustum/aligned_box.hpp>

namespace frustum
{

class Frustum
{
public:
	enum
	{
		LEF = 0, RIG, BOT, TOP, NEA, FAR
	};

	enum Visibility
	{
		full, partial, none
	};

	Frustum(const Matrix& view)
	{
		using namespace boost::qvm;

		this->plane[LEF] = row<0>(view) + row<3>(view);
		this->plane[RIG] = row<0>(view) - row<3>(view);
		this->plane[BOT] = row<1>(view) + row<3>(view);
		this->plane[TOP] = row<1>(view) - row<3>(view);
		this->plane[NEA] = row<2>(view) + row<3>(view);
		this->plane[FAR] = row<2>(view) - row<3>(view);
	}

	Visibility test(const aligned_box& box)
	{
		Visibility visibility = full;

		for (int i = 0; i < 6; ++i)
		{
			if (distance(this->plane[i], box.p_vertex(this->plane[i])) < 0)
				return none;
			if (distance(this->plane[i], box.n_vertex(this->plane[i])) < 0)
				visibility = partial;
		}

		return visibility;
	}

private:
	static float distance(const Plane& pln, const Vertex& vtx)
	{
		return pln.a * vtx.x + pln.b * vtx.y + pln.c * vtx.z + pln.d;
	}

	Plane planes[6];
};

} // namespace vmml

#endif /* FRUSTUM_FRUSTUM_HPP */
