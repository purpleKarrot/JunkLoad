#include "mapped_data_set.hpp"

#include "file_suffix_helper.hpp"

#include <iostream>

namespace stream_process
{

// ctor that loads an existing data set
mapped_data_set::mapped_data_set(const std::string& filename, bool new_file) :
	_filename(filename),
	_header(),
	_vertices(_header.get_vertex_element()),
	_faces(_header.get_face_element()),
	_vertex_map(0),
	_face_map(0)
{
	if (!new_file)
	{
		_header.read_from_file(filename);
		_setup(false);
	}
}

//// ctor that prepares a new data set according to the header
//mapped_data_set::mapped_data_set(const data_set_header& header_,
//		const std::string& filename) :
//	_header(header_),
//	_vertices(_header.get_vertex_element()),
//	_faces(_header.get_face_element()),
//	_vertex_map(0),
//	_face_map(0),
//	_filename(filename)
//{
//	_setup(true);
//}

mapped_data_set::~mapped_data_set()
{
	if (_vertex_map)
	{
		_vertex_map->close();
		delete _vertex_map;
	}

	if (_face_map)
	{
		_face_map->close();
		delete _face_map;
	}

}

void mapped_data_set::_setup(bool new_file)
{
	try
	{
		_vertex_map = new mapped_data_element(_vertices);
		_vertex_map->open(_filename, new_file);

		if (!_vertex_map->is_open())
		{
			throw std::runtime_error("opening vertex file failed.");
		}

		if (new_file || _header.has_faces())
		{
			_face_map = new mapped_data_element(_faces, _filename, new_file);
			if (!_vertex_map->is_open())
			{
				throw std::runtime_error("opening face file failed.");
			}
		}

	} catch (std::exception& e)
	{
		std::string msg("Opening point data set ");
		msg += _filename;
		msg += " failed.";
		throw std::runtime_error(msg);
	}

#if 0
	_vertex_map = new mapped_data_element( _vertices, _filename, new_file );
	assert( _vertex_map->is_open() );

	if ( new_file || _header.has_faces() )
	{
		_face_map = new mapped_data_element( _faces, _filename, new_file );
		assert( _face_map->is_open() );
	}
#endif
}

void mapped_data_set::compute_aabb()
{
	stream_structure& vs = _header.get_vertex_structure();
	const attribute& position = vs.get_attribute("position");

	switch (position.get_data_type_id())
	{
	case SP_FLOAT_32:
		_compute_aabb<float> ();
		break;
	case SP_FLOAT_64:
		_compute_aabb<double> ();
		break;
	default:
		throw std::runtime_error("invalid type for computing aabb.");
	}
}

data_element&
mapped_data_set::get_vertex_element()
{
	return _vertices;
}

const data_element&
mapped_data_set::get_vertex_element() const
{
	return _vertices;
}

const stream_structure&
mapped_data_set::get_vertex_structure() const
{
	return _vertices; //.get_structure();
}

data_element&
mapped_data_set::get_face_element()
{
	return _faces;
}

const data_element&
mapped_data_set::get_face_element() const
{
	return _faces;
}

const stream_structure&
mapped_data_set::get_face_structure() const
{
	return _faces; //.get_structure();
}

mapped_data_element&
mapped_data_set::get_vertex_map()
{
	assert(_vertex_map);
	return *_vertex_map;
}

const mapped_data_element&
mapped_data_set::get_vertex_map() const
{
	assert(_vertex_map);
	return *_vertex_map;
}

mapped_data_element&
mapped_data_set::get_face_map()
{
	assert(_face_map);
	return *_face_map;
}

const mapped_data_element&
mapped_data_set::get_face_map() const
{
	assert(_face_map);
	return *_face_map;
}

size_t mapped_data_set::get_vertex_size_in_bytes() const
{
	return _vertices.get_size_in_bytes();
}

size_t mapped_data_set::get_face_size_in_bytes() const
{
	return _faces.get_size_in_bytes();
}

std::string mapped_data_set::build_info_string() const
{

	size_t index = _filename.find_last_of('/');
	if (index)
		++index;
	std::string fn_no_path = _filename.substr(index, std::string::npos);

	std::stringstream ss;

	ss << "'" << fn_no_path << "'\n\n"

	<< "Vertex Data\n" << "  - " << (float) _vertices.get_file_size_in_bytes()
			/ 1048576.0 << " MBytes\n" << "  - " << (float) _vertices.size()
			* 1e-6 << " Mio Vertices\n" << "  - "
			<< _vertices.get_size_in_bytes() << " Bytes/Vertex\n" << "\n"
			<< "Vertex Attributes\n";

	const stream_structure& vs = _vertices; //.get_structure();
	stream_structure::super::const_iterator vit = vs.attributes.begin(), vit_end = vs.attributes.end();
	for (; vit != vit_end; ++vit)
	{
		ss << "  - " << (vit)->get_name() << "\n";
	}
	ss << "\n\n";

	if (_faces.size())
	{
		ss << "Face Data\n" << "  - "
				<< (float) _faces.get_file_size_in_bytes() / 1048576.0
				<< " MBytes\n" << "  - " << (float) _faces.size() * 1e-6
				<< " Mio Faces\n" << "  - " << _vertices.get_size_in_bytes()
				<< " Bytes/Face\n" << "\n" << "Face Attributes\n";

		const stream_structure& fs = _faces; //.get_structure();
		stream_structure::super::const_iterator fit = fs.attributes.begin(), fit_end = fs.attributes.end();
		for (; fit != fit_end; ++fit)
		{
			ss << "  - " << (fit)->get_name() << "\n";
		}
		ss << "\n\n";
	}

	return ss.str();

}

} // namespace stream_process

