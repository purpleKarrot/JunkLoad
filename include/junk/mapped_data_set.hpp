#ifndef __STREAM_PROCESS__MAPPED_DATA_SET__HPP__
#define __STREAM_PROCESS__MAPPED_DATA_SET__HPP__

#include <junk/types.hpp>
#include <junk/mapped_data_element.hpp>
#include <junk/stream_iterator.hpp>
#include <junk/attribute_accessor.hpp>

#include <boost/iostreams/device/mapped_file.hpp>

#include <string>

namespace junk
{

class mapped_data_set
{
public:
	typedef boost::iostreams::mapped_file mapped_file;
	typedef boost::iostreams::mapped_file_params mapped_file_params;

	typedef stream_iterator iterator;
	typedef const_stream_iterator const_iterator;

	mapped_data_set(const std::string& filename, bool new_file = false);

	~mapped_data_set();

	inline header& get_header();
	inline const header& get_header() const;

	size_t get_vertex_size_in_bytes() const;
	size_t get_face_size_in_bytes() const;

	// note: using iterators for sequential access will faster than
	// using get_x() all the time.
	inline char* get_vertex(size_t index);
	inline const char* get_vertex(size_t index) const;

	inline iterator vbegin();
	inline iterator vend();

	inline const_iterator vbegin() const;
	inline const_iterator vend() const;

	inline char* get_face(size_t index);
	inline const char* get_face(size_t index) const;

	inline iterator fbegin();
	inline iterator fend();

	inline const_iterator fbegin() const;
	inline const_iterator fend() const;

	element& get_vertex_element();
	const element& get_vertex_element() const;
	const element& get_vertex_structure() const;

	element& get_face_element();
	const element& get_face_element() const;
	const element& get_face_structure() const;

	mapped_data_element& get_vertex_map();
	const mapped_data_element& get_vertex_map() const;
	mapped_data_element& get_face_map();
	const mapped_data_element& get_face_map() const;

	const std::string& get_filename() const
	{
		return _filename;
	}

	void _setup(bool new_file);

protected:
	header _header;
	mapped_data_element* _vertex_map;
	mapped_data_element* _face_map;

	std::string _filename;

};

inline char* mapped_data_set::get_vertex(size_t index)
{
	return _vertex_map->operator[](index);
}

inline const char* mapped_data_set::get_vertex(size_t index) const
{
	return _vertex_map->operator[](index);
}

inline char* mapped_data_set::get_face(size_t index)
{
	return _face_map->operator[](index);
}

inline const char* mapped_data_set::get_face(size_t index) const
{
	return _face_map->operator[](index);
}

inline header& mapped_data_set::get_header()
{
	return _header;
}

inline const header& mapped_data_set::get_header() const
{
	return _header;
}

inline mapped_data_set::iterator mapped_data_set::vbegin()
{
	return _vertex_map->begin();
}

inline mapped_data_set::iterator mapped_data_set::vend()
{
	return _vertex_map->end();
}

inline mapped_data_set::const_iterator mapped_data_set::vbegin() const
{
	const mapped_data_element* vm = _vertex_map;
	return vm->begin();
}

inline mapped_data_set::const_iterator mapped_data_set::vend() const
{
	const mapped_data_element* vm = _vertex_map;
	return vm->end();
}

inline mapped_data_set::iterator mapped_data_set::fbegin()
{
	assert(_face_map && _header.face().size !=0);
	return _face_map->begin();
}

inline mapped_data_set::iterator mapped_data_set::fend()
{
	assert(_face_map && _header.face().size !=0);
	return _face_map->end();
}

inline mapped_data_set::const_iterator mapped_data_set::fbegin() const
{
	assert(_face_map && _header.face().size !=0);
	const mapped_data_element* fm = _face_map;
	return fm->begin();
}

inline mapped_data_set::const_iterator mapped_data_set::fend() const
{
	assert(_face_map && _header.face().size !=0);
	const mapped_data_element* fm = _face_map;
	return fm->end();
}

} // namespace junk

#endif
