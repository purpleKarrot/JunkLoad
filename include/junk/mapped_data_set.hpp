#ifndef __STREAM_PROCESS__MAPPED_DATA_SET__HPP__
#define __STREAM_PROCESS__MAPPED_DATA_SET__HPP__

#include <junk/types.hpp>
#include <junk/mapped_data_element.hpp>
#include <junk/stream_data_iterators.hpp>
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

	typedef stream_data_iterator iterator;
	typedef stream_data_const_iterator const_iterator;

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

	void compute_aabb();

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
	template<typename T>
	void _compute_aabb();

	header _header;
	mapped_data_element* _vertex_map;
	mapped_data_element* _face_map;

	std::string _filename;

};

template<typename T>
void mapped_data_set::_compute_aabb()
{
	typedef vmml::vector<3, T> vector_type;

	const element& vs = _header.vertex();

	const attribute& position = get_attribute(vs, "position");
	attribute_accessor<vector_type> get_position(position.offset);

	vector_type aabb_min(std::numeric_limits<T>::max());
	vector_type aabb_max(-std::numeric_limits<T>::max());

	for (iterator it = vbegin(), it_end = vend(); it != it_end; ++it)
	{
		const vector_type& v = get_position(*it);
		for (size_t index = 0; index < 3; ++index)
		{
			const T& v_ = v[index];
			T& min_ = aabb_min[index];
			T& max_ = aabb_max[index];

			if (v_ < min_)
				min_ = v_;
			if (v_ > max_)
				max_ = v_;
		}
	}

	_header.min = aabb_min;
	_header.max = aabb_max;
}

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
