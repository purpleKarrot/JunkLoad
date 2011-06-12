#include <string>
#include <vector>

#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>

#include <junk/attribute_accessor.hpp>
#include <junk/mapped_data_set.hpp>

#include "ply.h"

#pragma GCC diagnostic ignored "-Wwrite-strings"

PlyProperty** _vertex_properties;
int _vertex_count;
int _vertex_property_count;

PlyProperty** _face_properties;
int _face_count;
int _face_property_count;

junk::typid ply2junk_type(int input)
{
	switch (input)
	{
	case PLY_CHAR:
		return junk::SP_INT_8;
	case PLY_SHORT:
		return junk::SP_INT_16;
	case PLY_INT:
	case PLY_INT32:
		return junk::SP_INT_32;
	case PLY_UCHAR:
	case PLY_UINT8:
		return junk::SP_UINT_8;
	case PLY_USHORT:
		return junk::SP_UINT_16;
	case PLY_UINT:
		return junk::SP_UINT_32;
	case PLY_FLOAT:
		return junk::SP_FLOAT_32;
	case PLY_DOUBLE:
		return junk::SP_FLOAT_64;
	case PLY_FLOAT32:
		return junk::SP_FLOAT_32;
	}
	assert(!"ply2junk_type: invalid input");
}

int junk2ply_type(junk::typid input)
{
	static int ply[] =
	{
		PLY_CHAR,
		PLY_SHORT,
		PLY_INT,
		PLY_UCHAR,
		PLY_USHORT,
		PLY_UINT,
		PLY_FLOAT,
		PLY_DOUBLE
	};

	return ply[input];
}

void _setup_header_from_vertex_properties(junk::header& header)
{
	header.vertex().size = _vertex_count;
	junk::element& vs = header.vertex();

	for (int index = 0; index < _vertex_property_count; ++index)
	{
		PlyProperty* prop = _vertex_properties[index];
		create_attribute(vs, prop->name, ply2junk_type(prop->external_type), 1);
	}

	junk::compute_offsets(vs);
}

void _setup_header_from_face_properties(junk::header& header)
{
	if (_face_property_count == 0)
		return;

	header.face().size = _face_count;

	junk::element& fs = header.face();

	std::string index_name = "vertex_indices";

	for (int index = 0; index < _face_property_count; ++index)
	{
		PlyProperty* prop = _face_properties[index];

		if (index_name == prop->name)
			junk::create_attribute(fs, "vertex_indices", junk::SP_UINT_32, 3);
	}

	junk::compute_offsets(fs);
}

void _read_vertex_data(PlyFile* g_ply, junk::mapped_data_set& _data_set)
{
	const junk::element& vs = _data_set.get_header().vertex();

	BOOST_FOREACH(const junk::attribute& attr, vs.attributes)
	{
		PlyProperty ply_property;
		ply_property.name = attr.name.c_str();
		ply_property.internal_type = junk2ply_type(attr.type);
		ply_property.offset = attr.offset;
		ply_property.count_internal = 0;
		ply_property.count_offset = 0;

		ply_get_property(g_ply, "vertex", &ply_property);
	}

	junk::mapped_data_set::iterator vit = _data_set.vbegin();
	junk::mapped_data_set::iterator vit_end = _data_set.vend();
	for (; vit != vit_end; ++vit)
		ply_get_element(g_ply, *vit);
}

void _read_face_data(PlyFile* g_ply, junk::mapped_data_set& _data_set)
{
	// we have to build the required PlyProperties that allow
	// data extraction from the ply

	const junk::header& header = _data_set.get_header();
	const junk::element& fs = header.face();

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

	const junk::attribute& vertex_indices = get_attribute(fs, "vertex_indices");

	junk::attribute_accessor<junk::vec3ui> get_indices(vertex_indices.offset);

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
				assert(tmp_face_.vertices[index] < _vertex_count);
				indices[index] = tmp_face_.vertices[index];
			}
		}
		else
		{
			throw std::runtime_error("found non-triangle face in ply.");
		}
	}
}

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		std::cerr << "usage: ply_convert <plyfile> <junkfile>" << std::endl;
		return 0;
	}

	const char* source_file = argv[1];
	const char* target_file = argv[2];

    int nelems;
	char** elem_names;
	int file_type;
	float version;

	// open file
	boost::shared_ptr<PlyFile> g_ply(ply_open_for_reading((char*) source_file,
			&nelems, &elem_names, &file_type, &version), ply_close);
	if (!g_ply)
	{
		std::cerr << "opening file " << source_file << " failed." << std::endl;
		return -1;
	}

	// check for vertex and face element ( MUST HAVE )
	bool has_vertex_element = false;
	bool has_face_element = false;
	for (int index = 0; index < nelems; ++index)
	{
		if (std::string(g_ply->elems[index]->name) == "vertex")
			has_vertex_element = true;
		if (std::string(g_ply->elems[index]->name) == "face")
			has_face_element = true;
	}

	if (!has_vertex_element)
	{
		throw std::runtime_error("no vertices in ply file.");
	}

	if (!has_face_element)
	{
		throw std::runtime_error("no faces in ply file.");
	}

	// get vertex information
	_vertex_properties = ply_get_element_description(g_ply.get(), "vertex",
			&_vertex_count, &_vertex_property_count);

	// get face information
	_face_properties = ply_get_element_description(g_ply.get(), "face", &_face_count,
			&_face_property_count);

	junk::mapped_data_set _data_set(target_file, true);

	junk::header& header = _data_set.get_header();

	_setup_header_from_vertex_properties(header);
	_setup_header_from_face_properties(header);

	_data_set._setup(true);

	_read_vertex_data(g_ply.get(), _data_set);
	_read_face_data(g_ply.get(), _data_set);

	junk::save_header(target_file, header);
}
