/*
 * main.cpp
 *
 *  Created on: Aug 22, 2011
 *      Author: daniel
 */

#include <jnk/data_set.hpp>
#include <zix/z_index.hpp>

#include <boost/qvm/all.hpp>
using namespace boost::qvm;

typedef vec<float, 3> position;
typedef vec<uint32_t, 3> triangle;

template<typename T>
inline void minimize(T& a, const T& b)
{
	if (a > b)
		a = b;
}

template<typename T>
inline void maximize(T& a, const T& b)
{
	if (a < b)
		a = b;
}

template<>
inline void minimize<position>(position& a, const position& b)
{
	minimize(a % X, b % X);
	minimize(a % Y, b % Y);
	minimize(a % Z, b % Z);
}

template<>
inline void maximize<position>(position& a, const position& b)
{
	maximize(a % X, b % X);
	maximize(a % Y, b % Y);
	maximize(a % Z, b % Z);
}

template<typename T>
inline void minimize(T& a, const T& b, const T& c)
{
	minimize(a, b);
	minimize(a, c);
}

template<typename T>
inline void maximize(T& a, const T& b, const T& c)
{
	maximize(a, b);
	maximize(a, c);
}

template<typename T>
inline void minimize(T& a, const T& b, const T& c, const T& d)
{
	minimize(a, b);
	minimize(a, c);
	minimize(a, d);
}

template<typename T>
inline void maximize(T& a, const T& b, const T& c, const T& d)
{
	maximize(a, b);
	maximize(a, c);
	maximize(a, d);
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
	data_set.add_attribute<uint32_t>("box", "min_vertex");
	data_set.add_attribute<uint32_t>("box", "max_vertex");
	data_set.add_attribute<uint32_t>("box", "min_face");
	data_set.add_attribute<uint32_t>("box", "max_face");
	data_set.add_attribute<position>("box", "min_bbox");
	data_set.add_attribute<position>("box", "max_bbox");

	junk::accessor<position> get_position = data_set.get_accessor<position>("vertex", "position");
	junk::accessor<triangle> get_triangle = data_set.get_accessor<triangle>("face", "indices");
	junk::accessor<uint32_t> min_vertex = data_set.get_accessor<uint32_t>("box", "min_vertex");
	junk::accessor<uint32_t> max_vertex = data_set.get_accessor<uint32_t>("box", "max_vertex");
	junk::accessor<uint32_t> min_face = data_set.get_accessor<uint32_t>("box", "min_face");
	junk::accessor<uint32_t> max_face = data_set.get_accessor<uint32_t>("box", "max_face");
	junk::accessor<position> min_bbox = data_set.get_accessor<position>("box", "min_bbox");
	junk::accessor<position> max_bbox = data_set.get_accessor<position>("box", "max_bbox");

	junk::stream_range pos_range = data_set.stream_range(0);
	junk::stream_range tri_range = data_set.stream_range(1);
	junk::stream_range box_range = data_set.stream_range(2);

	for (std::size_t i = 0; i < tri_range.size(); ++i)
	{
		triangle& tri = get_triangle(tri_range[i]);

		unsigned int small_idx = (std::min)(tri % X, (std::min)(tri % Y, tri % Z));
		position& small = get_position(pos_range[small_idx]);

		int z = zix::z_index(small % X, small % Y, small % Z);

		char* box = box_range[z];

		minimize(min_vertex(box), tri % X, tri % Y, tri % Z);
		maximize(max_vertex(box), tri % X, tri % Y, tri % Z);

		minimize(min_face(box), i);
		maximize(max_face(box), i);

		const position& p1 = get_position(pos_range[tri % X]);
		const position& p2 = get_position(pos_range[tri % Y]);
		const position& p3 = get_position(pos_range[tri % Z]);

		minimize(min_bbox(box), p1, p2, p3);
		maximize(max_bbox(box), p1, p2, p3);
	}

	return 0;
}
