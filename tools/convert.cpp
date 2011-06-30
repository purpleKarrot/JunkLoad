#include <string>
#include <vector>

#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>

#include <junk/attribute_accessor.hpp>
#include <junk/mapped_data_set.hpp>

#include "ply/config.hpp"
#include "ply/ply.hpp"
#include "ply/byte_order.hpp"
#include "ply/io_operators.hpp"
#include "ply/ply_parser.hpp"

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

void _setup_header(PlyFile *plyfile, junk::header& header)
{
	int _vertex_count;
	int _vertex_property_count;
	PlyProperty** _vertex_properties = ply_get_element_description(plyfile,
			"vertex", &_vertex_count, &_vertex_property_count);

	header.vertex().size = _vertex_count;

	int _face_count;
	int _face_property_count;
	PlyProperty** _face_properties = ply_get_element_description(plyfile,
			"face", &_face_count, &_face_property_count);

	if (_face_property_count == 0)
		return;

	header.face().size = _face_count;
}

void _read_vertex_data(PlyFile* g_ply, junk::mapped_data_set& _data_set, bool normal, bool color)
{
	const junk::element& vs = _data_set.get_header().vertex();

	const junk::attribute& attr = get_attribute(vs, "position");
	PlyProperty pos_properties[] = {
		{ "x", PLY_FLOAT, PLY_FLOAT, attr.offset + 0 * sizeof(float), 0, 0, 0, 0 },
		{ "y", PLY_FLOAT, PLY_FLOAT, attr.offset + 1 * sizeof(float), 0, 0, 0, 0 },
		{ "z", PLY_FLOAT, PLY_FLOAT, attr.offset + 2 * sizeof(float), 0, 0, 0, 0 },
	};

	for (int i = 0; i < 3; ++i)
		ply_get_property(g_ply, "vertex", &pos_properties[i]);

	if (normal)
	{
		const junk::attribute& attr = get_attribute(vs, "normal");
		PlyProperty pos_properties[] = {
			{ "nx", PLY_FLOAT, PLY_FLOAT, attr.offset + 0 * sizeof(float), 0, 0, 0, 0 },
			{ "ny", PLY_FLOAT, PLY_FLOAT, attr.offset + 1 * sizeof(float), 0, 0, 0, 0 },
			{ "nz", PLY_FLOAT, PLY_FLOAT, attr.offset + 2 * sizeof(float), 0, 0, 0, 0 },
		};

		for (int i = 0; i < 3; ++i)
			ply_get_property(g_ply, "vertex", &pos_properties[i]);
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
			ply_get_property(g_ply, "vertex", &pos_properties[i]);
	}

	junk::mapped_data_set::iterator vit = _data_set.vbegin();
	junk::mapped_data_set::iterator vit_end = _data_set.vend();
	for (; vit != vit_end; ++vit)
		ply_get_element(g_ply, *vit);
}

void _read_face_data(PlyFile* g_ply, junk::mapped_data_set& _data_set)
{
	const junk::element& fs = _data_set.get_header().face();

	struct tmp_face
	{
		int* vertices;
		uint8_t number_of_vertices;
	} tmp_face_;

	PlyProperty face_property = {
		"vertex_indices", PLY_INT, PLY_INT, offsetof(tmp_face, vertices),
		1, PLY_UCHAR, PLY_UCHAR, offsetof(tmp_face, number_of_vertices)
		};

	ply_get_property(g_ply, "face", &face_property);

	const junk::attribute& attr = get_attribute(fs, "indices");

	junk::attribute_accessor<junk::vec3ui> get_indices(attr.offset);

	junk::mapped_data_set::iterator fit = _data_set.fbegin(), fit_end =
			_data_set.fend();
	for (; fit != fit_end; ++fit)
	{
		// read object data into memory map
		ply_get_element(g_ply, &tmp_face_);
		if (tmp_face_.number_of_vertices == 3)
		{
			junk::vec3ui& indices = get_indices(*fit);
			for (size_t index = 0; index < 3; ++index)
			{
				//assert(tmp_face_.vertices[index] < _vertex_count);
				indices[index] = tmp_face_.vertices[index];
			}
		}
		else
		{
			throw std::runtime_error("found non-triangle face in ply.");
		}
	}
}

void convert(const std::string& ply_file, junk::mapped_data_set& junk, bool normal, bool color)
{
    int nelems;
	char** elem_names;
	int file_type;
	float version;

	// open file
	boost::shared_ptr<PlyFile> g_ply(ply_open_for_reading((char*) ply_file.c_str(),
			&nelems, &elem_names, &file_type, &version), ply_close);
	if (!g_ply)
	{
		std::cerr << "opening file " << ply_file << " failed." << std::endl;
		exit(-1);
	}

	junk::header& header = junk.get_header();

	_setup_header(g_ply.get(), header);
	junk._setup(true);

	_read_vertex_data(g_ply.get(), junk, normal, color);
	_read_face_data(g_ply.get(), junk);
}
