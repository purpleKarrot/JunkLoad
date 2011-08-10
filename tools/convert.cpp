#include <string>
#include <vector>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/range/algorithm/for_each.hpp>

#include <junk/stream_iterator.hpp>
#include <junk/attribute_accessor.hpp>
#include <junk/data_set.hpp>

#include "ply/config.hpp"
#include "ply/ply.hpp"
#include "ply/byte_order.hpp"
#include "ply/io_operators.hpp"
#include "ply/ply_parser.hpp"

struct vec3_t
{
	float x, y, z;
};

struct color_t
{
	float r, g, b;
};

typedef unsigned int triangle_t[3];

junk::attribute_accessor<vec3_t> get_position(0);
junk::attribute_accessor<vec3_t> get_normal(12);
junk::attribute_accessor<color_t> get_color(24);
junk::attribute_accessor<triangle_t> get_indices(0);

class acc_ply_size
{
public:
	acc_ply_size(std::size_t& vertices, std::size_t& faces) :
			vertices(vertices), faces(faces)
	{
		ply_parser.end_header_callback(end_header);
		ply_parser.element_definition_callback(boost::bind(&acc_ply_size::element_definition, this, _1, _2));
	}

	void operator()(const std::string& filename)
	{
		ply_parser.parse(filename);
	}

private:
	ply::ply_parser::element_callbacks_type element_definition(const std::string& element, std::size_t size)
	{
		if (element == "vertex")
			vertices += size;

		if (element == "face")
			faces += size;

		return ply::ply_parser::element_callbacks_type();
	}

	static bool end_header()
	{
		return false;
	}

private:
	ply::ply_parser ply_parser;

	std::size_t& vertices;
	std::size_t& faces;
};

class read_ply_data
{
public:
	read_ply_data(junk::data_set& junk, bool normal, bool color);

	void operator()(const std::string& filename)
	{
		ply_parser.parse(filename);
	}

private:
	ply::ply_parser::element_callbacks_type element_definition(const std::string& element, std::size_t count);

	template<typename ScalarType>
	std::tr1::function<void(ScalarType)>
	scalar_property_definition(const std::string& element, const std::string& property);

	template<typename SizeType, typename ScalarType>
	std::tr1::tuple<std::tr1::function<void(SizeType)>, std::tr1::function<void(ScalarType)>, std::tr1::function<void()> >
	list_property_definition(const std::string& element, const std::string& property);

	void vertex_begin()
	{
	}

	void vertex_x(ply::float32 val)
	{
		get_position(*vrtx_it).x = val;
	}

	void vertex_y(ply::float32 val)
	{
		get_position(*vrtx_it).y = val;
	}

	void vertex_z(ply::float32 val)
	{
		get_position(*vrtx_it).z = val;
	}

	void vertex_nx(ply::float32 val)
	{
		get_normal(*vrtx_it).x = val;
	}

	void vertex_ny(ply::float32 val)
	{
		get_normal(*vrtx_it).y = val;
	}

	void vertex_nz(ply::float32 val)
	{
		get_normal(*vrtx_it).z = val;
	}

	void vertex_r(ply::uint8 val)
	{
		get_color(*vrtx_it).r = val;
	}

	void vertex_g(ply::uint8 val)
	{
		get_color(*vrtx_it).g = val;
	}

	void vertex_b(ply::uint8 val)
	{
		get_color(*vrtx_it).b = val;
	}

	void vertex_end()
	{
		++vrtx_it;
	}

	void face_begin()
	{
	}

	void face_vertex_indices_begin(ply::uint8 size)
	{
		assert(size == 3 && "non triangle face!");
		p = 0;
	}

	void face_vertex_indices_element(ply::int32 vertex_index)
	{
		get_indices(*face_it)[p++] = vertex_index + vertex_offset;
	}

	void face_vertex_indices_end()
	{
	}

	void face_end()
	{
		++face_it;
	}

private:
	ply::ply_parser ply_parser;

	bool normal;
	bool color;

	int p;

	std::size_t num_vertices;
	std::size_t vertex_offset;

	junk::stream_iterator vrtx_it;
	junk::stream_iterator face_it;
};

ply::ply_parser::element_callbacks_type read_ply_data::element_definition(const std::string& element_name, std::size_t count)
{
	if (element_name == "vertex")
	{
		vertex_offset += num_vertices;
		num_vertices = count;
		return ply::ply_parser::element_callbacks_type(boost::bind(&read_ply_data::vertex_begin, this), boost::bind(&read_ply_data::vertex_end, this));
	}

	if (element_name == "face")
		return ply::ply_parser::element_callbacks_type(boost::bind(&read_ply_data::face_begin, this), boost::bind(&read_ply_data::face_end, this));

	return ply::ply_parser::element_callbacks_type(0, 0);
}

template<>
std::tr1::function<void(ply::float32)> read_ply_data::scalar_property_definition(const std::string& element, const std::string& property)
{
	if (element != "vertex")
		return 0;

	if (property == "x")
		return boost::bind(&read_ply_data::vertex_x, this, _1);

	if (property == "y")
		return boost::bind(&read_ply_data::vertex_y, this, _1);

	if (property == "z")
		return boost::bind(&read_ply_data::vertex_z, this, _1);

	if (normal)
	{
		if (property == "nx")
			return boost::bind(&read_ply_data::vertex_nx, this, _1);

		if (property == "ny")
			return boost::bind(&read_ply_data::vertex_ny, this, _1);

		if (property == "nz")
			return boost::bind(&read_ply_data::vertex_nz, this, _1);
	}

	return 0;
}

template<>
std::tr1::function<void(ply::uint8)> read_ply_data::scalar_property_definition(const std::string& element, const std::string& property)
{
	if (element != "vertex")
		return 0;

	if (color)
	{
		if (property == "r" || property == "red")
			return boost::bind(&read_ply_data::vertex_r, this, _1);

		if (property == "g" || property == "green")
			return boost::bind(&read_ply_data::vertex_g, this, _1);

		if (property == "b" || property == "blue")
			return boost::bind(&read_ply_data::vertex_b, this, _1);
	}

	return 0;
}

template<>
std::tr1::tuple<std::tr1::function<void(ply::uint8)>, std::tr1::function<void(ply::int32)>, std::tr1::function<void()> > read_ply_data::list_property_definition(const std::string& element, const std::string& property)
{
	typedef std::tr1::tuple<std::tr1::function<void(ply::uint8)>, std::tr1::function<void(ply::int32)>, std::tr1::function<void()> > return_type;

	if (element == "face" && property == "vertex_indices")
		return return_type(boost::bind(&read_ply_data::face_vertex_indices_begin, this, _1), boost::bind(&read_ply_data::face_vertex_indices_element, this, _1), boost::bind(&read_ply_data::face_vertex_indices_end, this));

	return return_type(0, 0, 0);
}

read_ply_data::read_ply_data(junk::data_set& junk, bool normal, bool color) :
		normal(normal), color(color), vrtx_it(junk.stream_range(0).begin()), face_it(junk.stream_range(1).begin()), num_vertices(0), vertex_offset(0)
{
	ply_parser.element_definition_callback(boost::bind(&read_ply_data::element_definition, this, _1, _2));

	ply::ply_parser::scalar_property_definition_callbacks_type scalar_property_definition_callbacks;
	ply::at<ply::float32>(scalar_property_definition_callbacks) = boost::bind(&read_ply_data::scalar_property_definition<ply::float32>, this, _1, _2);
	ply_parser.scalar_property_definition_callbacks(scalar_property_definition_callbacks);

	ply::ply_parser::list_property_definition_callbacks_type list_property_definition_callbacks;
	ply::at<ply::uint8, ply::int32>(list_property_definition_callbacks) = boost::bind(&read_ply_data::list_property_definition<ply::uint8, ply::int32>, this, _1, _2);
	ply_parser.list_property_definition_callbacks(list_property_definition_callbacks);
}

void setup_header(junk::data_set& data_set, bool normal, bool color)
{
	data_set.add_element("vertex", "vertices");
	data_set.add_attribute("vertex", "position", junk::float_32, 3);

	if (normal)
		data_set.add_attribute("vertex", "normal", junk::float_32, 3);

	if (color)
		data_set.add_attribute("vertex", "color", junk::u_int_08, 3);

	data_set.add_element("face");
	data_set.add_attribute("face", "indices", junk::u_int_32, 3);
}

void convert(const std::vector<std::string>& input, junk::data_set& junk, bool normal, bool color)
{
	std::size_t vertices = 0;
	std::size_t faces = 0;

	boost::range::for_each(input, acc_ply_size (vertices, faces));

	junk.set_size("vertex", vertices);
	junk.set_size("face", faces);

	junk.load(true);

	boost::range::for_each(input, read_ply_data(junk, normal, color));
}
