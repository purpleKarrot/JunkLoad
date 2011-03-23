#include <stream_process/ply_converter.hpp>

#include <stream_process/exception.hpp>
#include <stream_process/attribute_accessor.hpp>

#include <boost/algorithm/string.hpp>

namespace stream_process
{

ply_converter::ply_converter(const params& params_) :
	_params(params_), _data_set(0)
{
	_init_ply_sp_type_map();

	if (boost::algorithm::iends_with(_params.source_file, ".ply"))
		_from_ply();
	else if (boost::algorithm::iends_with(_params.target_file, ".ply"))
		throw exception("ply-export not implemented yet.", SPROCESS_HERE);
	else
		throw exception("either source or target file must be '.ply'",
				SPROCESS_HERE);
}

void ply_converter::_from_ply()
{
	const std::string ply_file = _params.source_file;

	// open file
	_ply_file = ply_open_for_reading(const_cast<char *> (ply_file.c_str()),
			&_element_count, &_element_names, &_file_type, &_version);

	if (!_ply_file)
	{
		std::string msg = "opening file ";
		msg += ply_file;
		msg += " failed.";
		throw exception(msg.c_str(), SPROCESS_HERE);
	}

	// check for vertex and face element ( MUST HAVE )
	bool has_vertex_element = false;
	bool has_face_element = false;
	for (ssize_t index = 0; index < _element_count; ++index)
	{
		if (std::string(_element_names[index]) == "vertex")
			has_vertex_element = true;
		if (std::string(_element_names[index]) == "face")
			has_face_element = true;
	}

	if (!has_vertex_element)
	{
		throw exception("no vertices in ply file.", SPROCESS_HERE);
	}
	if (!has_face_element)
	{
		throw exception("no faces in ply file.", SPROCESS_HERE);
	}

	// read header
	{
		_read_meta_data();

		data_set_header header;

		_setup_header_from_vertex_properties(header);
		_setup_header_from_face_properties(header);

		_data_set = new data_set(header, _params.target_file);
	}

	// read data
	_read_vertex_data();
	_read_face_data();

	_data_set->compute_aabb();

	_data_set->get_header().write_to_file(_params.target_file);

	delete _data_set;
	_data_set = 0;
}

void ply_converter::_read_meta_data()
{
	// get vertex information
	_vertex_properties = ply_get_element_description(_ply_file, "vertex",
			&_vertex_count, &_vertex_property_count);

	// get face information
	_face_properties = ply_get_element_description(_ply_file, "face",
			&_face_count, &_face_property_count);

}

void ply_converter::_setup_header_from_vertex_properties(
		data_set_header& header)
{
	header.set_number_of_vertices(_vertex_count);

	size_t position_comps = 0;
	size_t normal_comps = 0;

	bool has_color = false;
	_color_names.resize(3);

	stream_data_structure& vs = header.get_vertex_structure();

	for (int index = 0; index < _vertex_property_count; ++index)
	{
		PlyProperty* prop = _vertex_properties[index];
		std::string name = prop->name;

		boost::algorithm::to_lower(name);

		if (name == "x" || name == "y" || name == "z")
		{
			++position_comps;
			if (position_comps == 3 && !vs.has_attribute("position"))
			{
				vs.create_attribute("position", SP_FLOAT_32, 3);
			}
		}
		else if (name == "nx" || name == "ny" || name == "nz")
		{
			++normal_comps;
			if (normal_comps == 3 && !vs.has_attribute("normal"))
			{
				vs.create_attribute("normal", SP_FLOAT_32, 3);
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
			_other_properties.push_back(std::pair<std::string, size_t>(name,
					index));
	}

	if (has_color)
	{
		vs.create_attribute("color", SP_UINT_8, 4);
	}

	std::vector<std::pair<std::string, size_t> >::iterator opit =
			_other_properties.begin(), opit_end = _other_properties.end();
	for (; opit != opit_end; ++opit)
	{
		std::pair < std::string, size_t > &prop_ref = *opit;
		PlyProperty* prop = _vertex_properties[prop_ref.second];
		vs.create_attribute(prop_ref.first, // name
				_ply_sp_type_map[prop->external_type], // type
				1 // array size
		);
	}

	vs.compute_offsets();

	std::cout << vs.to_header_string() << std::endl;

}

void ply_converter::_setup_header_from_face_properties(data_set_header& header)
{
	if (_face_property_count == 0)
		return;

	header.set_number_of_faces(_face_count);

	// atm only triangle support is implemented.

	stream_data_structure& fs = header.get_face_structure();

	std::string index_name = "vertex_indices";

	for (int index = 0; index < _face_property_count; ++index)
	{
		PlyProperty* prop = _face_properties[index];

		if (index_name == prop->name)
		{
			fs.create_attribute("vertex_indices", SP_UINT_32, // TODO make configable, or uint64_t?
					3 // triangles only
			);
		}
	}

	fs.compute_offsets();

	std::cout << fs.to_header_string() << std::endl;
}

void ply_converter::_read_vertex_data()
{
	const stream_data_structure& vs =
			_data_set->get_header().get_vertex_structure();

	const data_type_helper& dth = data_type_helper::get_singleton();

	// we have to build the required PlyProperties that allow
	// data extraction from the ply
	std::vector<PlyProperty> ply_props;

	std::vector<const char*> names;

	if (vs.has_attribute("position"))
	{
		const attribute& attr = vs.get_attribute("position");

		size_t offset = attr.get_offset();

		ply_props.push_back(_create_ply_property("x", PLY_FLOAT, offset));
		offset += sizeof(float);
		ply_props.push_back(_create_ply_property("y", PLY_FLOAT, offset));
		offset += sizeof(float);
		ply_props.push_back(_create_ply_property("z", PLY_FLOAT, offset));
	}

	if (vs.has_attribute("normal"))
	{
		const attribute& attr = vs.get_attribute("normal");

		size_t offset = attr.get_offset();

		ply_props.push_back(_create_ply_property("nx", PLY_FLOAT, offset));
		offset += sizeof(float);
		ply_props.push_back(_create_ply_property("ny", PLY_FLOAT, offset));
		offset += sizeof(float);
		ply_props.push_back(_create_ply_property("nz", PLY_FLOAT, offset));
	}

	if (vs.has_attribute("color"))
	{
		const attribute& attr = vs.get_attribute("color");

		size_t offset = attr.get_offset();

		ply_props.push_back(_create_ply_property(_color_names[0], PLY_UCHAR,
				offset));
		offset += sizeof(uint8_t);
		ply_props.push_back(_create_ply_property(_color_names[1], PLY_UCHAR,
				offset));
		offset += sizeof(uint8_t);
		ply_props.push_back(_create_ply_property(_color_names[2], PLY_UCHAR,
				offset));
		if (_color_names.size() == 4)
		{
			offset += sizeof(uint8_t);
			ply_props.push_back(_create_ply_property(_color_names[3],
					PLY_UCHAR, offset));
		}
	}

	std::vector<std::pair<std::string, size_t> >::iterator opit =
			_other_properties.begin(), opit_end = _other_properties.end();
	for (; opit != opit_end; ++opit)
	{
		std::pair < std::string, size_t > &prop_ref = *opit;
		const attribute& attr = vs.get_attribute(opit->first);
		ply_props.push_back(_create_ply_property(attr.get_name(),
				_vertex_properties[opit->second]->external_type,
				attr.get_offset()));
	}

	std::vector<PlyProperty>::iterator it = ply_props.begin(), it_end =
			ply_props.end();
	for (; it != it_end; ++it)
	{
		// register the properties that interest us with the ply reader
		ply_get_property(_ply_file, "vertex", &(*it));
	}

	data_set::iterator vit = _data_set->vbegin(), vit_end = _data_set->vend();
	for (; vit != vit_end; ++vit)
	{
		// read object data into memory map
		ply_get_element(_ply_file, *vit);
	}

	// clean up (delete c-string char arrays)
	for (it = ply_props.begin(); it != it_end; ++it)
	{
		delete[] (*it).name;
	}
}

void ply_converter::_read_face_data()
{
	// we have to build the required PlyProperties that allow
	// data extraction from the ply

	const data_set_header& header = _data_set->get_header();
	const stream_data_structure& fs = header.get_face_structure();

	struct tmp_face
	{
		int* vertices;
		uint8_t number_of_vertices;
	} tmp_face_;

	PlyProperty face_property =
	{ "vertex_indices", PLY_INT, PLY_INT, offsetof(tmp_face, vertices), 1,
			PLY_UCHAR, PLY_UCHAR, offsetof(tmp_face, number_of_vertices) };

	ply_get_property(_ply_file, "face", &face_property);

	const attribute& vertex_indices = fs.get_attribute("vertex_indices");

	attribute_accessor<vec3ui> get_indices;
	get_indices.set_offset(vertex_indices.get_offset());

	data_set::iterator fit = _data_set->fbegin(), fit_end = _data_set->fend();
	for (; fit != fit_end; ++fit)
	{
		// read object data into memory map
		ply_get_element(_ply_file, &tmp_face_);
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
			throw exception("found non-triangle face in ply.", SPROCESS_HERE);
		}
	}
}

PlyProperty ply_converter::_create_ply_property(const std::string& name,
		int internal_type, int offset, int count_internal, int count_offset)
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

void ply_converter::_init_ply_sp_type_map()
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
	_ply_sp_type_map.resize(PLY_END_TYPE + 1, 0);

	_ply_sp_type_map[PLY_CHAR] = SP_INT_8;
	_ply_sp_type_map[PLY_SHORT] = SP_INT_16;
	_ply_sp_type_map[PLY_INT] = SP_INT_32;

	_ply_sp_type_map[PLY_UCHAR] = SP_UINT_8;
	_ply_sp_type_map[PLY_USHORT] = SP_UINT_16;
	_ply_sp_type_map[PLY_UINT] = SP_UINT_32;

	_ply_sp_type_map[PLY_FLOAT] = SP_FLOAT_32;
	_ply_sp_type_map[PLY_DOUBLE] = SP_FLOAT_64;

	_ply_sp_type_map[PLY_FLOAT32] = SP_FLOAT_32;
	_ply_sp_type_map[PLY_UINT8] = SP_UINT_8;
	_ply_sp_type_map[PLY_INT32] = SP_INT_32;
}

} // namespace trip
