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
	}

	header(const header& other) :
		transform(other.transform), min(other.min), max(other.max),
				big_endian(other.big_endian), elements(other.elements)
	{
	}

	~header()
	{
	}

public:
	element& get_element(const std::string& name);
	const element& get_element(const std::string& name) const;

	element& vertex()
	{
		return get_element("vertex");
	}

	const element& vertex() const
	{
		return get_element("vertex");
	}

	element& face()
	{
		return get_element("face");
	}

	const element& face() const
	{
		return get_element("face");
	}

public:
	void read_from_file(const std::string& filename_base);

	void write_to_file(const std::string& filename_base) const;

	void update() const;

public:
	mat4d transform;
	vec3d min;
	vec3d max;
	bool big_endian;

	typedef std::vector<element> super;
mutable	super elements;
};

inline element& header::get_element(const std::string& name)
{
	if (elements.empty())
	{
		elements.push_back(element("vertex"));
		elements.push_back(element("face"));
		update();
	}

	if (name == "vertex")
		return elements[0];

	if (name == "face")
		return elements[1];

	throw std::runtime_error("element not found.");
}

inline const element& header::get_element(const std::string& name) const
{
	if (elements.empty())
	{
		elements.push_back(element("vertex"));
		elements.push_back(element("face"));
		update();
	}

	if (name == "vertex")
		return elements[0];

	if (name == "face")
		return elements[1];

	throw std::runtime_error("element not found.");
}

} // namespace stream_process

#endif
