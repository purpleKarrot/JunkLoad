#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/shared_ptr.hpp>

#include <junk/attribute_accessor.hpp>
#include <junk/mapped_data_set.hpp>

#include "ply.h"

#pragma GCC diagnostic ignored "-Wwrite-strings"

std::vector<std::string> _color_names;

// ply-reader related members
std::vector<std::pair<std::string, size_t> > _other_properties;
std::vector<junk::typid> _ply_sp_type_map;

PlyProperty** _vertex_properties;
int _vertex_count;
int _vertex_property_count;

PlyProperty** _face_properties;
int _face_count;
int _face_property_count;


PlyProperty _create_ply_property(const std::string& name, int internal_type,
		int offset, int count_internal = 0, int count_offset = 0)
{
	PlyProperty ply_property;
	ply_property.name = new char[name.size()];
	strcpy(ply_property.name, name.c_str());
	ply_property.internal_type = internal_type;
	ply_property.offset = offset;
	ply_property.count_internal = count_internal;
	ply_property.count_offset = count_offset;
	return ply_property;
}

void _init_ply_sp_type_map()
{
	_ply_sp_type_map.resize(EndType + 1, junk::SP_INT_8);

	_ply_sp_type_map[Int8] = junk::SP_INT_8;
	_ply_sp_type_map[Int16] = junk::SP_INT_16;
	_ply_sp_type_map[Int32] = junk::SP_INT_32;

	_ply_sp_type_map[Uint8] = junk::SP_UINT_8;
	_ply_sp_type_map[Uint16] = junk::SP_UINT_16;
	_ply_sp_type_map[Uint32] = junk::SP_UINT_32;

	_ply_sp_type_map[Float32] = junk::SP_FLOAT_32;
	_ply_sp_type_map[Float64] = junk::SP_FLOAT_64;
}

void _setup_header_from_vertex_properties(junk::header& header)
{
	header.vertex().size = _vertex_count;

	size_t position_comps = 0;
	size_t normal_comps = 0;

	bool has_color = false;
	_color_names.resize(3);

	junk::element& vs = header.vertex();

	for (int index = 0; index < _vertex_property_count; ++index)
	{
		PlyProperty* prop = _vertex_properties[index];
		std::string name = prop->name;

		boost::algorithm::to_lower(name);

		if (name == "x" || name == "y" || name == "z")
		{
			++position_comps;
			if (position_comps == 3 && !junk::has_attribute(vs, "position"))
			{
				junk::create_attribute(vs, "position", junk::SP_FLOAT_32, 3);
			}
		}
		else if (name == "nx" || name == "ny" || name == "nz")
		{
			++normal_comps;
			if (normal_comps == 3 && !junk::has_attribute(vs, "normal"))
			{
				junk::create_attribute(vs, "normal", junk::SP_FLOAT_32, 3);
			}
		}
		else if (name == "r" || name == "red")
		{
			has_color = true;
			_color_names[0] = name;
		}
		else if (name == "g" || name == "green")
		{
			has_color = true;
			_color_names[1] = name;
		}
		else if (name == "b" || name == "blue")
		{
			has_color = true;
			_color_names[2] = name;
		}
		else if (name == "a" || name == "alpha")
		{
			has_color = true;
			_color_names.resize(4);
			_color_names[3] = name;
		}
		else
			_other_properties.push_back(
					std::pair<std::string, size_t>(name, index));
	}

	if (has_color)
	{
		junk::create_attribute(vs, "color", junk::SP_UINT_8, 4);
	}

	std::vector<std::pair<std::string, size_t> >::iterator opit =
			_other_properties.begin(), opit_end = _other_properties.end();

	for (; opit != opit_end; ++opit)
	{
		std::pair<std::string, size_t> &prop_ref = *opit;
		PlyProperty* prop = _vertex_properties[prop_ref.second];
		create_attribute(vs, prop_ref.first,
				_ply_sp_type_map[prop->external_type], 1);
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
		{
			junk::create_attribute(fs, "vertex_indices", junk::SP_UINT_32, 3);
		}
	}

	junk::compute_offsets(fs);
}

void _read_vertex_data(PlyFile* g_ply, junk::mapped_data_set& _data_set)
{
	const junk::element& vs = _data_set.get_header().vertex();

	// we have to build the required PlyProperties that allow
	// data extraction from the ply
	std::vector<PlyProperty> ply_props;

	std::vector<const char*> names;

	if (junk::has_attribute(vs, "position"))
	{
		const junk::attribute& attr = get_attribute(vs, "position");

		size_t offset = attr.offset;

		ply_props.push_back(_create_ply_property("x", PLY_FLOAT, offset));
		offset += sizeof(float);
		ply_props.push_back(_create_ply_property("y", PLY_FLOAT, offset));
		offset += sizeof(float);
		ply_props.push_back(_create_ply_property("z", PLY_FLOAT, offset));
	}

	if (has_attribute(vs, "normal"))
	{
		const junk::attribute& attr = get_attribute(vs, "normal");

		size_t offset = attr.offset;

		ply_props.push_back(_create_ply_property("nx", PLY_FLOAT, offset));
		offset += sizeof(float);
		ply_props.push_back(_create_ply_property("ny", PLY_FLOAT, offset));
		offset += sizeof(float);
		ply_props.push_back(_create_ply_property("nz", PLY_FLOAT, offset));
	}

	if (has_attribute(vs, "color"))
	{
		const junk::attribute& attr = get_attribute(vs, "color");

		size_t offset = attr.offset;

		ply_props.push_back(
				_create_ply_property(_color_names[0], Uint8, offset));
		offset += sizeof(uint8_t);
		ply_props.push_back(
				_create_ply_property(_color_names[1], Uint8, offset));
		offset += sizeof(uint8_t);
		ply_props.push_back(
				_create_ply_property(_color_names[2], Uint8, offset));
		if (_color_names.size() == 4)
		{
			offset += sizeof(uint8_t);
			ply_props.push_back(
					_create_ply_property(_color_names[3], Uint8, offset));
		}
	}

	std::vector<std::pair<std::string, size_t> >::iterator opit =
			_other_properties.begin(), opit_end = _other_properties.end();
	for (; opit != opit_end; ++opit)
	{
		std::pair<std::string, size_t> &prop_ref = *opit;
		const junk::attribute& attr = get_attribute(vs, opit->first);
		ply_props.push_back(
				_create_ply_property(attr.name,
						_vertex_properties[opit->second]->external_type,
						attr.offset));
	}

	std::vector<PlyProperty>::iterator it = ply_props.begin(), it_end =
			ply_props.end();
	for (; it != it_end; ++it)
	{
		// register the properties that interest us with the ply reader
		ply_get_property(g_ply, "vertex", &(*it));
	}

	junk::mapped_data_set::iterator vit = _data_set.vbegin();
	junk::mapped_data_set::iterator vit_end = _data_set.vend();
	for (; vit != vit_end; ++vit)
	{
		// read object data into memory map
		ply_get_element(g_ply, *vit);
	}

	// clean up (delete c-string char arrays)
	for (it = ply_props.begin(); it != it_end; ++it)
	{
		delete[] (*it).name;
	}
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

	PlyProperty face_property =
	{ "vertex_indices", Int32, Int32, offsetof(tmp_face, vertices), 1, Uint8,
			Uint8, offsetof(tmp_face, number_of_vertices) };

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

	_init_ply_sp_type_map();

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

	_data_set.compute_aabb();

	junk::save_header(target_file, _data_set.get_header());
}
