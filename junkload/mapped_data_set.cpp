#include <junk/mapped_data_set.hpp>

#include <iostream>

namespace junk
{

mapped_data_set::mapped_data_set(const std::string& filename, bool new_file) :
		_vertex_map(0), _face_map(0), filename_(filename)
{
	if (!new_file)
	{
		junk::load_header(filename, header_);
		_setup(false);
	}
}

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
		_vertex_map = new mapped_data_element(header_.vertex());
		_vertex_map->open(filename_, new_file);

		if (!_vertex_map->is_open())
		{
			throw std::runtime_error("opening vertex file failed.");
		}

		if (new_file || header_.face().size != 0)
		{
			_face_map = new mapped_data_element(header_.face(), filename_, new_file);
			if (!_vertex_map->is_open())
			{
				throw std::runtime_error("opening face file failed.");
			}
		}

	} catch (std::exception& e)
	{
		std::string msg("Opening point data set ");
		msg += filename_;
		msg += " failed.";
		throw std::runtime_error(msg);
	}
}

} // namespace junk
