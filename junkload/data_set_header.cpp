#include "data_set_header.hpp"
#include "data_set_header_io.hpp"

#include <limits>

namespace stream_process
{

data_set_header::data_set_header() :
	_vertices("vertex"),
	_faces("face"),
	_aabb_min(-std::numeric_limits<double>::max()),
	_aabb_max(std::numeric_limits<double>::max()),
	_transform(mat4d::IDENTITY),
#ifdef __BIG_ENDIAN__
	_data_is_big_endian(true)
#else
	_data_is_big_endian(false)
#endif
{
	super::push_back(&_vertices);
	super::push_back(&_faces);
	update();
}

data_set_header::~data_set_header()
{
	if (super::size() > 2)
	{
		for (iterator it = begin() + 2, it_end = end(); it != it_end; ++it)
		{
			delete *it;
		}
	}
}

bool data_set_header::has_faces() const
{
	return !_faces.empty();
}

element& data_set_header::get_vertex_structure()
{
	return _vertices; //.get_structure();
}

const element& data_set_header::get_vertex_structure() const
{
	return _vertices; //.get_structure();
}

element& data_set_header::get_face_structure()
{
	return _faces; //.get_structure();
}

const element& data_set_header::get_face_structure() const
{
	return _faces; //.get_structure();
}

size_t data_set_header::get_number_of_vertices() const
{
	return _vertices.size();
}

void data_set_header::set_number_of_vertices(size_t num)
{
	_vertices.size(num);
}

size_t data_set_header::get_number_of_faces() const
{
	return _faces.size();
}

void data_set_header::set_number_of_faces(size_t num)
{
	_faces.size(num);
}

void data_set_header::set_data_is_big_endian(bool is_big_endian)
{
	_data_is_big_endian = is_big_endian;
}

bool data_set_header::get_data_is_big_endian() const
{
	return _data_is_big_endian;
}

void data_set_header::read_from_file(const std::string& filename_base)
{
	data_set_header_io io;
	io.read_from_file(filename_base, *this);

	for (iterator it = begin(), it_end = end(); it != it_end; ++it)
	{
		(*it)->update();
	}
}

void data_set_header::write_to_file(const std::string& filename_base) const
{
	data_set_header_io io;
	io.write_to_file(filename_base, *this);
}

void data_set_header::update()
{
	for (iterator it = begin(), it_end = end(); it != it_end; ++it)
	{
		(*it)->update();
	}
}

} // namespace stream_process
