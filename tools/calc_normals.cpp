//

#include <junk/accessor.hpp>
#include <junk/data_set.hpp>

#include <boost/qvm/all.hpp>
using namespace boost::qvm;

typedef vec<float, 3> vec3;
typedef vec<unsigned int, 3> triangle;

void calc_normals(junk::data_set& junk)
{
	junk::accessor<vec3> get_position = junk.get_accessor<vec3>("vertex", "position");
	junk::accessor<vec3> get_normal = junk.get_accessor<vec3>("vertex", "normal");
	junk::accessor<triangle> get_triangle = junk.get_accessor<triangle>("face", "indices");

	junk::stream_range vertices = junk.stream_range(0);
	junk::stream_range triangles = junk.stream_range(1);

	for (junk::stream_iterator i = vertices.begin(); i != vertices.end(); ++i)
		set_zero(get_normal(*i));

	// iterate over all triangles and add their normals to adjacent vertices
	for (junk::stream_iterator f = triangles.begin(); f != triangles.end(); ++f)
	{
		triangle& tri = get_triangle(*f);

		vec3& p1 = get_position(vertices[tri % X]);
		vec3& p2 = get_position(vertices[tri % Y]);
		vec3& p3 = get_position(vertices[tri % Z]);

		vec3 normal = cross(p2 - p1, p3 - p1);

		get_normal(vertices[tri % X]) += normal;
		get_normal(vertices[tri % Y]) += normal;
		get_normal(vertices[tri % Z]) += normal;
	}

	// normalize all the normals
	for (junk::stream_iterator i = vertices.begin(); i != vertices.end(); ++i)
	{
		vec3& normal = get_normal(*i);
		if (normal % X != 0 || normal % Y != 0 || normal % Z != 0)
			normalize(normal);
	}
}
