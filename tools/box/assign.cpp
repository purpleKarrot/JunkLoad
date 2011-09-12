#include "assign.hpp"
#include "min_max.hpp"

#include <cmath>
#include <limits>

#include <boost/qvm/all.hpp>
using namespace boost::qvm;

//#define VAL(X) "\n" #X ": " << X

Assign::Assign(int fanout, int height, junk::data_set& data) :
		fanout(fanout), height(height), data(data)
{
	leaves = std::pow(fanout, height);
	branches = (leaves - 1) / (fanout - 1);

	face_max = data.get_size("face") - 1;

	get_position = data.get_accessor<position>("vertex", "position");
	get_triangle = data.get_accessor<triangle>("face", "indices");
}

const Box& Assign::process(const uint32_t index)
{
	junk::stream_range box_range = data.stream_range(2);
	Box& box = *reinterpret_cast<Box*>(box_range[index]);

	box.min_vertex = box.min_face = (std::numeric_limits<uint32_t>::max)();
	box.max_vertex = box.max_face = (std::numeric_limits<uint32_t>::min)();

	box.min_bbox % X = box.min_bbox % Y = box.min_bbox % Z =
			(std::numeric_limits<float>::max)();
	box.min_bbox % X = box.min_bbox % Y = box.max_bbox % Z =
			(std::numeric_limits<float>::min)();

	if (is_leaf(index))
	{
		const uint32_t leaf = index - branches;
		box.min_face = (face_max * (0 + leaf)) / leaves;
		box.max_face = (face_max * (1 + leaf)) / leaves;

		junk::stream_range tri_range = data.stream_range(1);
		for (uint32_t i = box.min_face; i <= box.max_face; ++i)
		{
			const triangle& tri = get_triangle(tri_range[i]);
			minimize(box.min_vertex, tri % X, tri % Y, tri % Z);
			maximize(box.max_vertex, tri % X, tri % Y, tri % Z);
		}

		junk::stream_range pos_range = data.stream_range(0);
		for (uint32_t i = box.min_vertex; i <= box.max_vertex; ++i)
		{
			const position& pos = get_position(pos_range[i]);
			minimize(box.min_bbox, pos);
			maximize(box.max_bbox, pos);
		}
	}
	else
	{
		for (int i = 1; i <= fanout; ++i)
		{
			const Box& child_box = process(index * fanout + i);

			minimize(box.min_vertex, child_box.min_vertex);
			maximize(box.max_vertex, child_box.max_vertex);

			minimize(box.min_face, child_box.min_face);
			maximize(box.max_face, child_box.max_face);

			minimize(box.min_bbox, child_box.min_bbox);
			maximize(box.max_bbox, child_box.max_bbox);
		}
	}

//	std::cout
//		<< VAL(index)
//		<< VAL(box.min_vertex)
//		<< VAL(box.max_vertex)
//		<< VAL(box.min_face)
//		<< VAL(box.max_face)
//		<< std::endl
//		;

	return box;
}
