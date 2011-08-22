/*
 * main.cpp
 *
 *  Created on: Aug 22, 2011
 *      Author: daniel
 */

#include <junk/data_set.hpp>

#include <boost/qvm/all.hpp>
using namespace boost::qvm;

typedef float position[3];
typedef unsigned int triangle[3];


template<typename T>
T minimum(T a, T b, T c)
{
	return (std::min)(a, (std::min)(b,c));
}

template<typename T>
T minimum(T a, T b, T c, T d)
{
	return minimum(a,b, (std::min)(c,d));
}

position minimum(position const& p1, position const& p2, position const& p3, position const& p4)
{
	position p;
	p[0] = minimum(p[0], p1[0],p2[0],p3[0],p4[0]);
	p[1] = minimum(p[1], p1[1],p2[1],p3[1],p4[1]);
	p[2] = minimum(p[2], p1[2],p2[2],p3[2],p4[2]);
	return p;
}

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cerr << "usage: box <data_set>" << std::endl;
		return 0;
	}

	junk::data_set data_set(argv[1]);
	data_set.add_element("box", "boxes");
	data_set.add_attribute<unsigned int>("box", "min_vertex");
	data_set.add_attribute<unsigned int>("box", "max_vertex");
	data_set.add_attribute<unsigned int>("box", "min_face");
	data_set.add_attribute<unsigned int>("box", "max_face");
	data_set.add_attribute<position>("box", "min_bbox");
	data_set.add_attribute<position>("box", "max_bbox");

	junk::accessor<position> get_position = data_set.get_accessor<position>("vertex", "position");
	junk::accessor<triangle> get_triangle = data_set.get_accessor<triangle>("face", "indices");
	junk::accessor<unsigned int> min_vertex = data_set.get_accessor<unsigned int>("box", "min_vertex");
	junk::accessor<unsigned int> max_vertex = data_set.get_accessor<unsigned int>("box", "max_vertex");
	junk::accessor<unsigned int> min_face = data_set.get_accessor<unsigned int>("box", "min_face");
	junk::accessor<unsigned int> max_face = data_set.get_accessor<unsigned int>("box", "max_face");
	junk::accessor<position> min_bbox = data_set.get_accessor<position>("box", "min_bbox");
	junk::accessor<position> max_bbox = data_set.get_accessor<position>("box", "max_bbox");

	junk::stream_range pos_range = data_set.stream_range(0);
	junk::stream_range tri_range = data_set.stream_range(1);
	junk::stream_range box_range = data_set.stream_range(2);


	for (std::size_t i=0; i < tri_range.size(); ++i)
	{
		triangle& tri = get_triangle(tri_range[i]);

		unsigned int small_idx = (std::min)(tri%X, (std::min)(tri%Y,tri%Z));
		position& smallest_pos = get_position(pos_range[small_idx]);

		int z = z_index(smallest_pos);

		char* box = box_range[z];

		minimize(min_vertex(box), tri%X, tri%Y, tri%Z);
		maximize(max_vertex(box), tri%X, tri%Y, tri%Z);

		minimize(min_face(box), i);
		maximize(max_face(box), i);

		const position& p1 = get_position(pos_range[tri % X]);
		const position& p2 = get_position(pos_range[tri % Y]);
		const position& p3 = get_position(pos_range[tri % Z]);

		minimize(min_bbox(box), p1, p2, p3)
		maximize(max_bbox(box), p1, p2, p3);
	}

	return 0;
}
