#ifndef __STREAM_PROCESS__MAPPED_DATA_SET__HPP__
#define __STREAM_PROCESS__MAPPED_DATA_SET__HPP__

#include "data_set_header.hpp"
#include "mapped_data_element.hpp"
#include "stream_data.hpp"
#include "stream_data_iterators.hpp"
#include "attribute_accessor.hpp"

#include <boost/iostreams/device/mapped_file.hpp>

#include <string>

namespace stream_process
{

class mapped_data_set
{
public:
	typedef boost::iostreams::mapped_file mapped_file;
	typedef boost::iostreams::mapped_file_params mapped_file_params;

	typedef stream_data_iterator iterator;
	typedef stream_data_const_iterator const_iterator;

	// ctor that loads an existing data set
	mapped_data_set(const std::string& filename);

	// ctor that prepares a new data set according to the header
	mapped_data_set(const data_set_header& header_,
					const std::string& filename);

	~mapped_data_set();

	inline data_set_header& get_header();
	inline const data_set_header& get_header() const;

	size_t get_vertex_size_in_bytes() const;
	size_t get_face_size_in_bytes() const;

	// note: using iterators for sequential access will faster than
	// using get_x() all the time.
	inline stream_data* get_vertex(size_t index);
	inline const stream_data* get_vertex(size_t index) const;

	inline iterator vbegin();
	inline iterator vend();

	inline const_iterator vbegin() const;
	inline const_iterator vend() const;

	inline stream_data* get_face(size_t index);
	inline const stream_data* get_face(size_t index) const;

	inline iterator fbegin();
	inline iterator fend();

	inline const_iterator fbegin() const;
	inline const_iterator fend() const;

	void compute_aabb();

	data_element& get_vertex_element();
	const data_element& get_vertex_element() const;

	const stream_structure& get_vertex_structure() const;

	data_element& get_face_element();
	const data_element& get_face_element() const;

	const stream_structure& get_face_structure() const;

	mapped_data_element& get_vertex_map();
	const mapped_data_element& get_vertex_map() const;
	mapped_data_element& get_face_map();
	const mapped_data_element& get_face_map() const;

	const std::string& get_filename() const
	{
		return _filename;
	}

	std::string build_info_string() const;

protected:
	void _setup(bool new_file);

	template<typename T>
	void _compute_aabb();

	data_set_header _header;
	data_element& _vertices;
	data_element& _faces;
	mapped_data_element* _vertex_map;
	mapped_data_element* _face_map;

	std::string _filename;

}; // class mapped_data_set


template<typename T>
void mapped_data_set::_compute_aabb()
{
	typedef vmml::vector<3, T> vector_type;

	const stream_structure& vs = _header.get_vertex_structure();

	const attribute& position = vs.get_attribute("position");
	attribute_accessor<vector_type> get_position;

	get_position.set_offset(position.get_offset());

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

	_header.set_aabb_min(aabb_min);
	_header.set_aabb_max(aabb_max);

}

inline stream_data*
mapped_data_set::get_vertex(size_t index)
{
	return _vertex_map->operator[](index);
}

inline const stream_data*
mapped_data_set::get_vertex(size_t index) const
{
	return _vertex_map->operator[](index);
}

inline stream_data*
mapped_data_set::get_face(size_t index)
{
	return _face_map->operator[](index);
}

inline const stream_data*
mapped_data_set::get_face(size_t index) const
{
	return _face_map->operator[](index);
}

inline data_set_header&
mapped_data_set::get_header()
{
	return _header;
}

inline const data_set_header&
mapped_data_set::get_header() const
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
	assert(_face_map && _header.has_faces());
	return _face_map->begin();
}

inline mapped_data_set::iterator mapped_data_set::fend()
{
	assert(_face_map && _header.has_faces());
	return _face_map->end();
}

inline mapped_data_set::const_iterator mapped_data_set::fbegin() const
{
	assert(_face_map && _header.has_faces());
	const mapped_data_element* fm = _face_map;
	return fm->begin();
}

inline mapped_data_set::const_iterator mapped_data_set::fend() const
{
	assert(_face_map && _header.has_faces());
	const mapped_data_element* fm = _face_map;
	return fm->end();
}

} // namespace stream_process

#endif

