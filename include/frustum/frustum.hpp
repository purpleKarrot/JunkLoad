#ifndef FRUSTUM_FRUSTUM_HPP
#define FRUSTUM_FRUSTUM_HPP

#include <frustum/matrix.hpp>
#include <frustum/aligned_box.hpp>
#include <boost/qvm/all.hpp>

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

		this->planes[LEF] = row<3>(view) + row<0>(view);
		this->planes[RIG] = row<3>(view) - row<0>(view);
		this->planes[BOT] = row<3>(view) + row<1>(view);
		this->planes[TOP] = row<3>(view) - row<1>(view);
		this->planes[NEA] = row<3>(view) + row<2>(view);
		this->planes[FAR] = row<3>(view) - row<3>(view);
	}

	Visibility test(const AlignedBox& box) const
	{
		Visibility visibility = full;

		for (int i = 0; i < 6; ++i)
		{
			if (distance(this->planes[i], box.p_vertex(this->planes[i])) < 0)
				return none;
			if (distance(this->planes[i], box.n_vertex(this->planes[i])) < 0)
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
