#include "convert.hpp"

#include "../preprocess_types.hpp"
#include "../mapped_data_set.hpp"

#include "ply_io.h"

#include <vector>

#include "../attribute_accessor.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/shared_ptr.hpp>

#pragma GCC diagnostic ignored "-Wwrite-strings"

namespace stream_process
{

static PlyFile* g_ply;

static struct
{
	mapped_data_set* _data_set;

	std::vector<std::string> _color_names;

	// ply-reader related members
	std::vector<std::pair<std::string, size_t> > _other_properties;
	std::vector<typid> _ply_sp_type_map;

	PlyProperty _create_ply_property(const std::string& name,
			int internal_type, int offset, int count_internal = 0,
			int count_offset = 0);

	PlyProperty** _vertex_properties;
	int _vertex_count;
	int _vertex_property_count;

	PlyProperty** _face_properties;
	int _face_count;
	int _face_property_count;

//	int _element_count;
//	char** _element_names;
	int _file_type; // file type, either ascii or binary
	float _version;
} _static;

static PlyProperty _create_ply_property(const std::string& name,
		int internal_type, int offset, int count_internal = 0,
		int count_offset = 0)
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

//void _setup_header_from_vertex_properties(data_set_header& h);
//void _setup_header_from_face_properties(data_set_header& h);
//
//void _read_meta_data();
//
//void _read_vertex_data();
//void _read_face_data();

static void _init_ply_sp_type_map()
{
	/*
	 #define PLY_START_TYPE 0
	 #define PLY_CHAR       1
	 #define PLY_SHORT      2
	 #define PLY_INT        3
	 #define PLY_UCHAR      4
	 #define PLY_USHORT     5
	 #define PLY_UINT       6
	 #define PLY_FLOAT      7
	 #define PLY_DOUBLE     8
	 #define PLY_FLOAT32    9
	 #define PLY_UINT8      10
	 #define PLY_INT32      11
	 #define PLY_END_TYPE   12
	 */
	_static._ply_sp_type_map.resize(EndType + 1, SP_INT_8);

	_static._ply_sp_type_map[Int8] = SP_INT_8;
	_static._ply_sp_type_map[Int16] = SP_INT_16;
	_static._ply_sp_type_map[Int32] = SP_INT_32;

	_static._ply_sp_type_map[Uint8] = SP_UINT_8;
	_static._ply_sp_type_map[Uint16] = SP_UINT_16;
	_static._ply_sp_type_map[Uint32] = SP_UINT_32;

	_static._ply_sp_type_map[Float32] = SP_FLOAT_32;
	_static._ply_sp_type_map[Float64] = SP_FLOAT_64;
}

static void _read_meta_data()
{
	// get vertex information
	_static._vertex_properties = get_element_description_ply(g_ply,
			"vertex", &_static._vertex_count, &_static._vertex_property_count);

	// get face information
	_static._face_properties = get_element_description_ply(g_ply,
			"face", &_static._face_count, &_static._face_property_count);
}

static void _setup_header_from_vertex_properties(header& header)
{
	header.vertex().size = _static._vertex_count;

	size_t position_comps = 0;
	size_t normal_comps = 0;

	bool has_color = false;
	_static._color_names.resize(3);

	element& vs = header.vertex();

	for (int index = 0; index < _static._vertex_property_count; ++index)
	{
		PlyProperty* prop = _static._vertex_properties[index];
		std::string name = prop->name;

		boost::algorithm::to_lower(name);

		if (name == "x" || name == "y" || name == "z")
		{
			++position_comps;
			if (position_comps == 3 && !has_attribute(vs, "position"))
			{
				create_attribute(vs, "position", SP_FLOAT_32, 3);
			}
		}
		else if (name == "nx" || name == "ny" || name == "nz")
		{
			++normal_comps;
			if (normal_comps == 3 && !has_attribute(vs, "normal"))
			{
				create_attribute(vs, "normal", SP_FLOAT_32, 3);
			}
		}
		else if (name == "r" || name == "red")
		{
			has_color = true;
			_static._color_names[0] = name;
		}
		else if (name == "g" || name == "green")
		{
			has_color = true;
			_static._color_names[1] = name;
		}
		else if (name == "b" || name == "blue")
		{
			has_color = true;
			_static._color_names[2] = name;
		}
		else if (name == "a" || name == "alpha")
		{
			has_color = true;
			_static._color_names.resize(4);
			_static._color_names[3] = name;
		}
		else
			_static._other_properties.push_back(
					std::pair<std::string, size_t>(name, index));
	}

	if (has_color)
	{
		create_attribute(vs, "color", SP_UINT_8, 4);
	}

	std::vector<std::pair<std::string, size_t> >::iterator opit =
			_static._other_properties.begin(), opit_end =
			_static._other_properties.end();
	for (; opit != opit_end; ++opit)
	{
		std::pair<std::string, size_t> &prop_ref = *opit;
		PlyProperty* prop = _static._vertex_properties[prop_ref.second];
		create_attribute(vs, prop_ref.first, // name
				_static._ply_sp_type_map[prop->external_type], // type
				1 // array size
		);
	}

	junk::compute_offsets(vs);
}

static void _setup_header_from_face_properties(header& header)
{
	if (_static._face_property_count == 0)
		return;

	header.face().size = _static._face_count;

	// atm only triangle support is implemented.

	element& fs = header.face();

	std::string index_name = "vertex_indices";

	for (int index = 0; index < _static._face_property_count; ++index)
	{
		PlyProperty* prop = _static._face_properties[index];

		if (index_name == prop->name)
		{
			create_attribute(fs, "vertex_indices", SP_UINT_32, // TODO make configable, or uint64_t?
					3 // triangles only
			);
		}
	}

	junk::compute_offsets(fs);
}

static void _read_vertex_data()
{
	const element& vs = _static._data_set->get_header().vertex();

	// we have to build the required PlyProperties that allow
	// data extraction from the ply
	std::vector<PlyProperty> ply_props;

	std::vector<const char*> names;

	if (has_attribute(vs, "position"))
	{
		const attribute& attr = get_attribute(vs, "position");

		size_t offset = attr.offset;

		ply_props.push_back(_create_ply_property("x", Float32, offset));
		offset += sizeof(float);
		ply_props.push_back(_create_ply_property("y", Float32, offset));
		offset += sizeof(float);
		ply_props.push_back(_create_ply_property("z", Float32, offset));
	}

	if (has_attribute(vs, "normal"))
	{
		const attribute& attr = get_attribute(vs, "normal");

		size_t offset = attr.offset;

		ply_props.push_back(_create_ply_property("nx", Float32, offset));
		offset += sizeof(float);
		ply_props.push_back(_create_ply_property("ny", Float32, offset));
		offset += sizeof(float);
		ply_props.push_back(_create_ply_property("nz", Float32, offset));
	}

	if (has_attribute(vs, "color"))
	{
		const attribute& attr = get_attribute(vs, "color");

		size_t offset = attr.offset;

		ply_props.push_back(
				_create_ply_property(_static._color_names[0], Uint8, offset));
		offset += sizeof(uint8_t);
		ply_props.push_back(
				_create_ply_property(_static._color_names[1], Uint8, offset));
		offset += sizeof(uint8_t);
		ply_props.push_back(
				_create_ply_property(_static._color_names[2], Uint8, offset));
		if (_static._color_names.size() == 4)
		{
			offset += sizeof(uint8_t);
			ply_props.push_back(
					_create_ply_property(_static._color_names[3], Uint8, offset));
		}
	}

	std::vector<std::pair<std::string, size_t> >::iterator opit =
			_static._other_properties.begin(), opit_end =
			_static._other_properties.end();
	for (; opit != opit_end; ++opit)
	{
		std::pair<std::string, size_t> &prop_ref = *opit;
		const attribute& attr = get_attribute(vs, opit->first);
		ply_props.push_back(
				_create_ply_property(
						attr.name,
						_static._vertex_properties[opit->second]->external_type,
						attr.offset));
	}

	std::vector<PlyProperty>::iterator it = ply_props.begin(), it_end =
			ply_props.end();
	for (; it != it_end; ++it)
	{
		// register the properties that interest us with the ply reader
		ply_get_property(g_ply, "vertex", &(*it));
	}

	mapped_data_set::iterator vit = _static._data_set->vbegin();
	mapped_data_set::iterator vit_end = _static._data_set->vend();
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

static void _read_face_data()
{
	// we have to build the required PlyProperties that allow
	// data extraction from the ply

	const header& header = _static._data_set->get_header();
	const element& fs = header.face();

	struct tmp_face
	{
		int* vertices;
		uint8_t number_of_vertices;
	} tmp_face_;

	PlyProperty face_property =
	{ "vertex_indices", Int32, Int32, offsetof(tmp_face, vertices), 1, Uint8,
			Uint8, offsetof(tmp_face, number_of_vertices) };

	ply_get_property(g_ply, "face", &face_property);

	const attribute& vertex_indices = get_attribute(fs, "vertex_indices");

	attribute_accessor<vec3ui> get_indices(vertex_indices.offset);

	mapped_data_set::iterator fit = _static._data_set->fbegin(), fit_end =
			_static._data_set->fend();
	for (; fit != fit_end; ++fit)
	{
		// read object data into memory map
		ply_get_element(g_ply, &tmp_face_);
		if (tmp_face_.number_of_vertices == 3)
		{
			vec3ui& indices = get_indices(*fit);
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

void ply_convert(const char* source_file, const std::string& target_file)
{
	_init_ply_sp_type_map();

	// open file
	boost::shared_ptr<FILE> file(fopen(source_file, "r"), fclose);
	if (!file)
	{
		std::string msg = "opening file ";
		msg += source_file;
		msg += " failed.";
		throw std::runtime_error(msg.c_str());
	}

//	_static._ply_file = ply_open_for_reading(
//			const_cast<char *> (source_file.c_str()), &_static._element_count,
//			&_static._element_names, &_static._file_type, &_static._version);

	g_ply = read_ply(file.get());
	if (!g_ply)
	{
		std::string msg = "opening file ";
		msg += source_file;
		msg += " failed.";
		throw std::runtime_error(msg.c_str());
	}

	// check for vertex and face element ( MUST HAVE )
	bool has_vertex_element = false;
	bool has_face_element = false;
	for (int index = 0; index < g_ply->num_elem_types; ++index)
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

	// read header
	_read_meta_data();

	_static._data_set = new mapped_data_set(target_file, true);

	header& header = _static._data_set->get_header();

	_setup_header_from_vertex_properties(header);
	_setup_header_from_face_properties(header);

	_static._data_set->_setup(true);

	//	_static._data_set = new mapped_data_set(header, target_file);

	// read data
	_read_vertex_data();
	_read_face_data();

	_static._data_set->compute_aabb();

	junk::save_header(target_file, _static._data_set->get_header());

	delete _static._data_set;
	_static._data_set = 0;
}

} // namespace trip
