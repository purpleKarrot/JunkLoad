#include "data_set_header.hpp"
#include "data_set_header_io.hpp"

#include <limits>

namespace stream_process
{

bool header::has_faces() const
{
	return !elements[1].empty();
}

element& header::get_vertex_structure()
{
	return elements[0]; //.get_structure();
}

const element& header::get_vertex_structure() const
{
	return elements[0]; //.get_structure();
}

element& header::get_face_structure()
{
	return elements[1]; //.get_structure();
}

const element& header::get_face_structure() const
{
	return elements[1]; //.get_structure();
}

size_t header::get_number_of_vertices() const
{
	return elements[0].size();
}

void header::set_number_of_vertices(size_t num)
{
	elements[0].size(num);
}

size_t header::get_number_of_faces() const
{
	return elements[1].size();
}

void header::set_number_of_faces(size_t num)
{
	elements[1].size(num);
}

void header::set_data_is_big_endian(bool is_big_endian)
{
	big_endian = is_big_endian;
}

bool header::get_data_is_big_endian() const
{
	return big_endian;
}

void header::read_from_file(const std::string& filename_base)
{
	data_set_header_io io;
	io.read_from_file(filename_base, *this);

	for (super::iterator it = elements.begin(), it_end = elements.end(); it != it_end; ++it)
	{
		it->update();
	}
}

void header::write_to_file(const std::string& filename_base) const
{
	data_set_header_io io;
	io.write_to_file(filename_base, *this);
}

void header::update()
{
	for (super::iterator it = elements.begin(), it_end = elements.end(); it != it_end; ++it)
	{
		it->update();
	}
}

} // namespace stream_process
