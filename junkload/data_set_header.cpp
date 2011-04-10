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

stream_structure&
data_set_header::get_vertex_structure()
{
	return _vertices; //.get_structure();
}

const stream_structure&
data_set_header::get_vertex_structure() const
{
	return _vertices; //.get_structure();
}

stream_structure&
data_set_header::get_face_structure()
{
	return _faces; //.get_structure();
}

const stream_structure&
data_set_header::get_face_structure() const
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

void data_set_header::update_float_precision(data_type_id sp, data_type_id hp)
{
	data_set_header::iterator eit = begin(), eit_end = end();
	for (; eit != eit_end; ++eit)
	{
		stream_structure& s = *(*eit); //->get_structure();

		stream_structure::super::iterator it = s.attributes.begin(), it_end = s.attributes.end();
		for (; it != it_end; ++it)
		{
			attribute& attr = *it;
			data_type_id in_type = attr.get_data_type_id();
			bool is_hp = attr.is_high_precision();

			if (in_type == SP_FLOAT_32 || in_type == SP_FLOAT_64)
			{
				if (is_hp)
					attr.set_data_type_id(hp);
				else
					attr.set_data_type_id(sp);
			}
		}
	}
}

void data_set_header::finalize_structures()
{
	data_set_header::iterator eit = begin(), eit_end = end();
	for (; eit != eit_end; ++eit)
	{
		// we sort the attributes according to
		// - if they are outputs (so the whole output data is one block)
		// - their offset (so inputs stay in order)

		stream_structure& s = *(*eit); //->get_structure();

//		s.sort(attribute_ptr_outputs_first());

		size_t offset = 0;

		std::cout << "  data_set_header: finalizing " << s.get_name()
				<< " structure." << std::endl;

		stream_structure::super::iterator it = s.attributes.begin(), it_end = s.attributes.end();
		for (; it != it_end; ++it)
		{
			attribute& attr = *it;
			attr.set_offset(offset);
			offset += attr.get_size_in_bytes();
		}

	}
}

} // namespace stream_process

