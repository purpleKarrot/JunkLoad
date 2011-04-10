#ifndef __STREAM_PROCESS__DATA_SET_HEADER__HPP__
#define __STREAM_PROCESS__DATA_SET_HEADER__HPP__

#include "VMMLibIncludes.h"
#include "element.hpp"

#include <cstddef>

namespace stream_process
{

class header
{
public:
	header():
		transform(mat4d::IDENTITY),
		min(-std::numeric_limits<double>::max()),
		max(std::numeric_limits<double>::max()),
	#ifdef __BIG_ENDIAN__
		big_endian(true)
	#else
		big_endian(false)
	#endif
	{
		elements.push_back(element("vertex"));
		elements.push_back(element("face"));
		update();
	}

	header(const header& other) :
		transform(other.transform), min(other.min), max(other.max),
				big_endian(other.big_endian), elements(other.elements)
	{
	}

	~header()
	{
	}

	void read_from_file(const std::string& filename_base);

	void write_to_file(const std::string& filename_base) const;

	inline element& get_element(const std::string& name);

	inline element& get_vertex_element();

	inline element& get_face_element();

	inline const element& get_element(const std::string& name) const;

	inline const element& get_vertex_element() const;

	inline const element& get_face_element() const;

	// legacy_begin
	element& get_vertex_structure();

	const element& get_vertex_structure() const;

	element& get_face_structure();

	const element& get_face_structure() const;

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

public:
	mat4d transform;
	vec3d min;
	vec3d max;
	bool big_endian;

	typedef std::vector<element> super;
	super elements;
};

inline element& header::get_element(const std::string& name)
{
	if (name == "vertex")
		return elements[0];

	if (name == "face")
		return elements[1];

	throw std::runtime_error("element not found.");
}

inline const element& header::get_element(const std::string& name) const
{
	if (name == "vertex")
		return elements[0];

	if (name == "face")
		return elements[1];

	throw std::runtime_error("element not found.");
}

inline element& header::get_vertex_element()
{
	return elements[0];
}

inline const element& header::get_vertex_element() const
{
	return elements[0];
}

inline element& header::get_face_element()
{
	return elements[1];
}

inline const element& header::get_face_element() const
{
	return elements[1];
}

template<typename T>
void header::get_aabb(vmml::vector<3, T>& min_,
		vmml::vector<3, T>& max_) const
{
	for (size_t index = 0; index < 3; ++index)
	{
		min_[index] = static_cast<T> (min[index]);
	}

	for (size_t index = 0; index < 3; ++index)
	{
		max_[index] = static_cast<T> (max[index]);
	}
}

template<typename T>
vmml::vector<3, T> header::get_aabb_min() const
{
	vmml::vector<3, T> min_;

	for (size_t index = 0; index < 3; ++index)
	{
		min_[index] = static_cast<T> (min[index]);
	}

	return min_;
}

template<typename T>
void header::set_aabb_min(const vmml::vector<3, T>& min_)
{
	for (size_t index = 0; index < 3; ++index)
	{
		min[index] = static_cast<double> (min_[index]);
	}
}

template<typename T>
vmml::vector<3, T> header::get_aabb_max() const
{
	vmml::vector<3, T> max_;

	for (size_t index = 0; index < 3; ++index)
	{
		max_[index] = static_cast<T> (max[index]);
	}

	return max_;
}

template<typename T>
void header::set_aabb_max(const vmml::vector<3, T>& max_)
{
	for (size_t index = 0; index < 3; ++index)
	{
		max[index] = static_cast<double> (max_[index]);
	}
}

template<typename T>
vmml::matrix<4, 4, T> header::get_transform() const
{
	vmml::matrix<4, 4, T> transform_;
	for (size_t row = 0; row < 4; ++row)
	{
		for (size_t col = 0; col < 4; ++col)
		{
			transform_(row, col) = static_cast<T> (transform(row, col));
		}
	}
	return transform_;
}

template<typename T>
void header::set_transform(const vmml::matrix<4, 4, T>& transform_)
{
	for (size_t row = 0; row < 4; ++row)
	{
		for (size_t col = 0; col < 4; ++col)
		{
			transform(row, col) = static_cast<T> (transform_(row, col));
		}
	}
}

} // namespace stream_process

#endif
