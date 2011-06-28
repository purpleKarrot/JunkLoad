#include <string>
#include <vector>

#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>

#include <junk/attribute_accessor.hpp>
#include <junk/mapped_data_set.hpp>

#include "ply/ply.h"

// hardcoded offset of zero
junk::attribute_accessor<junk::vec3ui> get_indices(0);

struct tmp_face
{
	int* vertices;
	uint8_t number_of_vertices;
};

typedef boost::shared_ptr<PlyFile> shared_ply;
static inline shared_ply open_ply(const std::string& filename)
{
	int nelems;
	char** elem_names;
	int file_type;
	float version;

	return shared_ply(ply_open_for_reading((char*) filename.c_str(), &nelems,
			&elem_names, &file_type, &version), ply_close);
}

static inline int elem_count(const shared_ply& ply, const char* elem)
{
	int size;
	int properties;
	ply_get_element_description(ply.get(), (char*) elem, &size, &properties);
	return size;
}

void setup_header(junk::header& header, bool normal, bool color)
{
	junk::element& vs = header.vertex();

	create_attribute(vs, "position", junk::SP_FLOAT_32, 3);

	if(normal)
		create_attribute(vs, "normal", junk::SP_FLOAT_32, 3);

	if(color)
		create_attribute(vs, "color", junk::SP_UINT_8, 3);

	junk::compute_offsets(vs);

	junk::element& fs = header.face();
	junk::create_attribute(fs, "indices", junk::SP_UINT_32, 3);
	junk::compute_offsets(fs);
}

static void setup_ply(const shared_ply& ply, junk::header& header, bool normal, bool color)
{
	const junk::element& vs = header.vertex();

	const junk::attribute& attr = get_attribute(vs, "position");
	PlyProperty pos_properties[] = {
		{ "x", PLY_FLOAT, PLY_FLOAT, attr.offset + 0 * sizeof(float), 0, 0, 0, 0 },
		{ "y", PLY_FLOAT, PLY_FLOAT, attr.offset + 1 * sizeof(float), 0, 0, 0, 0 },
		{ "z", PLY_FLOAT, PLY_FLOAT, attr.offset + 2 * sizeof(float), 0, 0, 0, 0 },
	};

	for (int i = 0; i < 3; ++i)
		ply_get_property(ply.get(), "vertex", &pos_properties[i]);

	if (normal)
	{
		const junk::attribute& attr = get_attribute(vs, "normal");
		PlyProperty pos_properties[] = {
			{ "nx", PLY_FLOAT, PLY_FLOAT, attr.offset + 0 * sizeof(float), 0, 0, 0, 0 },
			{ "ny", PLY_FLOAT, PLY_FLOAT, attr.offset + 1 * sizeof(float), 0, 0, 0, 0 },
			{ "nz", PLY_FLOAT, PLY_FLOAT, attr.offset + 2 * sizeof(float), 0, 0, 0, 0 },
		};

		for (int i = 0; i < 3; ++i)
			ply_get_property(ply.get(), "vertex", &pos_properties[i]);
	}

	if (color)
	{
		const junk::attribute& attr = get_attribute(vs, "color");
		PlyProperty pos_properties[] = {
			{ "red", PLY_UCHAR, PLY_UCHAR, attr.offset + 0 * sizeof(char), 0, 0, 0, 0 },
			{ "green", PLY_UCHAR, PLY_UCHAR, attr.offset + 1 * sizeof(char), 0, 0, 0, 0 },
			{ "blue", PLY_UCHAR, PLY_UCHAR, attr.offset + 2 * sizeof(char), 0, 0, 0, 0 },
		};

		for (int i = 0; i < 3; ++i)
			ply_get_property(ply.get(), "vertex", &pos_properties[i]);
	}

	PlyProperty face_property = {
		"vertex_indices", PLY_INT, PLY_INT, offsetof(tmp_face, vertices),
		1, PLY_UCHAR, PLY_UCHAR, offsetof(tmp_face, number_of_vertices)
		};

	ply_get_property(ply.get(), "face", &face_property);
}

static inline void _read_vertex_data(const shared_ply& ply,
		std::size_t num_vertices, junk::mapped_data_set::iterator& vit)
{
	while (num_vertices--)
		ply_get_element(ply.get(), *(vit++));
}

static void _read_face_data(const shared_ply& ply, std::size_t num_faces,
		std::size_t vetex_offset, junk::mapped_data_set::iterator& fit)
{
	tmp_face tmp;

	while (num_faces--)
	{
		ply_get_element(ply.get(), &tmp);
		if (tmp.number_of_vertices != 3)
			throw std::runtime_error("found non-triangle face in ply.");

		junk::vec3ui& indices = get_indices(*(fit++));

		for (std::size_t i = 0; i < 3; ++i)
			indices[i] = tmp.vertices[i] + vetex_offset;
	}
}

void convert(const std::string& input, junk::mapped_data_set& junk, bool normal, bool color)
{
	typedef boost::filesystem::recursive_directory_iterator ply_iterator;
	ply_iterator start(input), end;

	junk::header& header = junk.get_header();
	header.vertex().size = 0;
	header.face().size = 0;

	for (ply_iterator it = start; it != end; ++it)
	{
		shared_ply ply_file = open_ply(it->path().string());
		header.vertex().size += elem_count(ply_file, "vertex");
		header.face().size += elem_count(ply_file, "face");
	}

	junk._setup(true);
	std::size_t vetex_offset = 0;

	junk::mapped_data_set::iterator vit = junk.vbegin();
	junk::mapped_data_set::iterator vit_end = junk.vend();

	junk::mapped_data_set::iterator fit = junk.fbegin();
	junk::mapped_data_set::iterator fit_end = junk.fend();

	for (ply_iterator it = start; it != end; ++it)
	{
		shared_ply ply_file = open_ply(it->path().string());
		setup_ply(ply_file, header, normal, color);

		std::size_t num_vertices = elem_count(ply_file, "vertex");
		std::size_t num_faces = elem_count(ply_file, "face");

		_read_vertex_data(ply_file, num_vertices, vit);
		_read_face_data(ply_file, num_faces, vetex_offset, fit);

		vetex_offset += num_vertices;
	}

	assert(++vit == vit_end);
	assert(++fit == fit_end);
}
