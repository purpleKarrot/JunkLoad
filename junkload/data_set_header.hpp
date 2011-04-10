#ifndef __STREAM_PROCESS__DATA_SET_HEADER__HPP__
#define __STREAM_PROCESS__DATA_SET_HEADER__HPP__

#include "VMMLibIncludes.h"
#include "stream_structure.hpp"
#include "element.hpp"

#include <cstddef>

namespace stream_process
{

class data_set_header: private std::vector<data_element*>, private boost::noncopyable
{
public:
	typedef std::vector<data_element*> super;

	using super::iterator;
	using super::const_iterator;
	using super::begin;
	using super::end;

	data_set_header();

	~data_set_header();

	void read_from_file(const std::string& filename_base);

	void write_to_file(const std::string& filename_base) const;

	using super::operator[];

	inline data_element& get_element(const std::string& name);

	inline data_element& get_vertex_element();

	inline data_element& get_face_element();

	inline const data_element& get_element(const std::string& name) const;

	inline const data_element& get_vertex_element() const;

	inline const data_element& get_face_element() const;

	// legacy_begin
	stream_structure& get_vertex_structure();

	const stream_structure& get_vertex_structure() const;

	stream_structure& get_face_structure();

	const stream_structure& get_face_structure() const;

	bool has_faces() const;

	size_t get_number_of_vertices() const;

	void set_number_of_vertices(size_t num);

	size_t get_number_of_faces() const;

	void set_number_of_faces(size_t num);
	// legacy_end

	template<typename T>
	void get_aabb(vmml::vector<3, T>& min_, vmml::vector<3, T>& max) const;

	template<typename T>
	vmml::vector<3, T> get_aabb_min() const;

	template<typename T>
	void set_aabb_min(const vmml::vector<3, T>& min_);

	template<typename T>
	vmml::vector<3, T> get_aabb_max() const;

	template<typename T>
	void set_aabb_max(const vmml::vector<3, T>& max_);

	template<typename T>
	vmml::matrix<4, 4, T> get_transform() const;

	template<typename T>
	void set_transform(const vmml::matrix<4, 4, T>& transform_);

	void set_data_is_big_endian(bool is_big_endian);

	bool get_data_is_big_endian() const;

	void update();

	void update_float_precision(data_type_id sp, data_type_id hp);

	void finalize_structures();

protected:
	data_element _vertices;
	data_element _faces;

	vec3d _aabb_min;
	vec3d _aabb_max;

	mat4d _transform;

	bool _data_is_big_endian;
};

inline data_element& data_set_header::get_element(const std::string& name)
{
	if (name == "vertex")
		return _vertices;

	if (name == "face")
		return _faces;

	throw std::runtime_error("element not found.");
}

inline const data_element& data_set_header::get_element(const std::string& name) const
{
	if (name == "vertex")
		return _vertices;

	if (name == "face")
		return _faces;

	throw std::runtime_error("element not found.");
}

inline data_element& data_set_header::get_vertex_element()
{
	return _vertices;
}

inline const data_element& data_set_header::get_vertex_element() const
{
	return _vertices;
}

inline data_element& data_set_header::get_face_element()
{
	return _faces;
}

inline const data_element& data_set_header::get_face_element() const
{
	return _faces;
}

template<typename T>
void data_set_header::get_aabb(vmml::vector<3, T>& min_,
		vmml::vector<3, T>& max_) const
{
	for (size_t index = 0; index < 3; ++index)
	{
		min_[index] = static_cast<T> (_aabb_min[index]);
	}

	for (size_t index = 0; index < 3; ++index)
	{
		max_[index] = static_cast<T> (_aabb_max[index]);
	}
}

template<typename T>
vmml::vector<3, T> data_set_header::get_aabb_min() const
{
	vmml::vector<3, T> min_;

	for (size_t index = 0; index < 3; ++index)
	{
		min_[index] = static_cast<T> (_aabb_min[index]);
	}

	return min_;
}

template<typename T>
void data_set_header::set_aabb_min(const vmml::vector<3, T>& min_)
{
	for (size_t index = 0; index < 3; ++index)
	{
		_aabb_min[index] = static_cast<double> (min_[index]);
	}
}

template<typename T>
vmml::vector<3, T> data_set_header::get_aabb_max() const
{
	vmml::vector<3, T> max_;

	for (size_t index = 0; index < 3; ++index)
	{
		max_[index] = static_cast<T> (_aabb_max[index]);
	}

	return max_;
}

template<typename T>
void data_set_header::set_aabb_max(const vmml::vector<3, T>& max_)
{
	for (size_t index = 0; index < 3; ++index)
	{
		_aabb_max[index] = static_cast<double> (max_[index]);
	}
}

template<typename T>
vmml::matrix<4, 4, T> data_set_header::get_transform() const
{
	vmml::matrix<4, 4, T> transform_;
	for (size_t row = 0; row < 4; ++row)
	{
		for (size_t col = 0; col < 4; ++col)
		{
			transform_(row, col) = static_cast<T> (_transform(row, col));
		}
	}
	return transform_;
}

template<typename T>
void data_set_header::set_transform(const vmml::matrix<4, 4, T>& transform_)
{
	for (size_t row = 0; row < 4; ++row)
	{
		for (size_t col = 0; col < 4; ++col)
		{
			_transform(row, col) = static_cast<T> (transform_(row, col));
		}
	}
}

} // namespace stream_process

#endif
