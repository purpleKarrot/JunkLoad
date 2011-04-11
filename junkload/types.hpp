/*
 * types.hpp
 *
 *  Created on: Apr 10, 2011
 *      Author: daniel
 */

#ifndef JUNKLOAD_TYPES_HPP
#define JUNKLOAD_TYPES_HPP

#include <string>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/adapted/struct/define_struct.hpp>

#include "VMMLibIncludes.h"

namespace junkload
{

enum type_id
{
	SP_INT_8,
	SP_INT_16,
	SP_INT_32,
	SP_INT_64,
	SP_UINT_8,
	SP_UINT_16,
	SP_UINT_32,
	SP_UINT_64,
	SP_FLOAT_32,
	SP_FLOAT_64,
};

std::size_t size_in_bytes(type_id type);

struct attribute
{
	attribute() :
		type(SP_INT_8), size(1), offset(0)
	{
	}

	attribute(const std::string& name, type_id type, std::size_t size = 1) :
		name(name), type(type), size(size), offset(0)
	{
	}

	std::string name;
	type_id type;
	std::size_t size;
	std::size_t offset;
};

inline std::size_t size_in_bytes(const attribute& a)
{
	return a.size * size_in_bytes(a.type);
}

class element
{
public:
	element(const std::string& name = std::string()) :
		name(name), size(0)
	{
	}

	element(const element& other) :
		name(other.name), size(other.size), attributes(other.attributes)
	{
	}

	~element()
	{
	}

public:

	bool empty() const
	{
		return size == 0;
	}

	void update()
	{
		compute_offsets();
	}

	bool has_attribute(const std::string& name) const;

	junkload::attribute& get_attribute(const std::string& name);
	const junkload::attribute& get_attribute(const std::string& name) const;

	template<typename T>
	attribute& create_attribute(const std::string& name, size_t array_size = 0);

	void create_attribute(const std::string& name, type_id type,
			size_t size = 1);

	void compute_offsets();

public:
	std::string name;
	std::size_t size;
	std::vector<junkload::attribute> attributes;
};

// returns the size of a  point/face/...
inline std::size_t size_in_bytes(const element& e)
{
	std::size_t size = 0;

	std::vector<attribute>::const_iterator it = e.attributes.begin();
	std::vector<attribute>::const_iterator it_end = e.attributes.end();

	for (; it != it_end; ++it)
	{
		size += size_in_bytes(*it);
	}

	return size;
}

// returns the size of the whole data set
inline std::size_t file_size_in_bytes(const element& e)
{
	return e.size * size_in_bytes(e);
}

template<typename T>
struct get_type_id_from_type
{
};

template<>
struct get_type_id_from_type<float>
{
	static const type_id value = SP_FLOAT_32;
};

template<>
struct get_type_id_from_type<double>
{
	static const type_id value = SP_FLOAT_64;
};

template<>
struct get_type_id_from_type<int8_t>
{
	static const type_id value = SP_INT_8;
};

template<>
struct get_type_id_from_type<uint8_t>
{
	static const type_id value = SP_UINT_8;
};

template<>
struct get_type_id_from_type<int16_t>
{
	static const type_id value = SP_INT_16;
};

template<>
struct get_type_id_from_type<uint16_t>
{
	static const type_id value = SP_UINT_16;
};

template<>
struct get_type_id_from_type<int32_t>
{
	static const type_id value = SP_INT_32;
};

template<>
struct get_type_id_from_type<uint32_t>
{
	static const type_id value = SP_UINT_32;
};

template<>
struct get_type_id_from_type<int64_t>
{
	static const type_id value = SP_INT_64;
};

template<>
struct get_type_id_from_type<uint64_t>
{
	static const type_id value = SP_UINT_64;
};

template<typename T>
struct attribute_type
{
	inline attribute* create(const std::string& name)
	{
		return create(name, 1);
	}

	inline attribute* create(const std::string& name, size_t array_size)
	{
		type_id id_ = get_type_id_from_type<T>::value;
		return new attribute(name, id_, array_size);
	}

	inline bool test(const attribute& attribute_)
	{
		return operator()(attribute_);
	}

	inline bool operator()(const attribute& attribute_)
	{
		type_id id_ = get_type_id_from_type<T>::value;
		bool ok = id_ == attribute_.type();
		if (!ok)
		{
			std::cerr << "expected attribute type: " << id_ << std::endl;

			std::cerr << "actual attribute type:   "
					<< attribute_.type() << std::endl;
		}

		return ok;
	}
};

template<>
inline attribute*
attribute_type<vec2f>::create(const std::string& name)
{
	return new attribute(name, SP_FLOAT_32, 2);
}

template<>
inline bool attribute_type<vec2f>::operator()(const attribute& attribute_)
{
	if (attribute_.type != SP_FLOAT_32)
		return false;
	if (attribute_.size != 2)
		return false;
	return true;
}

template<>
inline attribute*
attribute_type<vec3f>::create(const std::string& name)
{
	return new attribute(name, SP_FLOAT_32, 3);
}

template<>
inline bool attribute_type<vec3f>::operator()(const attribute& attribute_)
{
	if (attribute_.type != SP_FLOAT_32)
		return false;
	if (attribute_.size != 3)
		return false;
	return true;
}

template<>
inline attribute* attribute_type<vec3ui>::create(const std::string& name)
{
	return new attribute(name, SP_UINT_32, 3);
}

template<>
inline bool attribute_type<vec3ui>::operator()(const attribute& attribute_)
{
	if (attribute_.type != SP_UINT_32)
		return false;
	if (attribute_.size != 3)
		return false;
	return true;
}

template<>
inline attribute*
attribute_type<vec4f>::create(const std::string& name)
{
	return new attribute(name, SP_FLOAT_32, 4);
}

template<>
inline bool attribute_type<vec4f>::operator()(const attribute& attribute_)
{
	if (attribute_.type != SP_FLOAT_32)
		return false;
	if (attribute_.size != 4)
		return false;
	return true;
}

template<>
inline attribute* attribute_type<vec2d>::create(const std::string& name)
{
	return new attribute(name, SP_FLOAT_64, 2);
}

template<>
inline bool attribute_type<vec2d>::operator()(const attribute& attribute_)
{
	if (attribute_.type != SP_FLOAT_64)
		return false;
	if (attribute_.size != 2)
		return false;
	return true;
}

template<>
inline attribute*
attribute_type<vec3d>::create(const std::string& name)
{
	return new attribute(name, SP_FLOAT_64, 3);
}

template<>
inline bool attribute_type<vec3d>::operator()(const attribute& attribute_)
{
	if (attribute_.type != SP_FLOAT_64)
		return false;
	if (attribute_.size != 3)
		return false;
	return true;
}

template<>
inline attribute* attribute_type<vec4d>::create(const std::string& name)
{
	return new attribute(name, SP_FLOAT_64, 4);
}

template<>
inline bool attribute_type<vec4d>::operator()(const attribute& attribute_)
{
	if (attribute_.type != SP_FLOAT_64)
		return false;
	if (attribute_.size != 4)
		return false;
	return true;
}

template<>
inline attribute*
attribute_type<vec4ub>::create(const std::string& name)
{
	return new attribute(name, SP_UINT_8, 4);
}

template<>
inline bool attribute_type<vec4ub>::operator()(const attribute& attribute_)
{
	if (attribute_.type != SP_UINT_8)
		return false;
	if (attribute_.size != 4)
		return false;
	return true;
}

template<>
inline attribute*
attribute_type<mat3f>::create(const std::string& name)
{
	return new attribute(name, SP_FLOAT_32, 9);
}

template<>
inline bool attribute_type<mat3f>::operator()(const attribute& attribute_)
{
	if (attribute_.type != SP_FLOAT_32)
		return false;
	if (attribute_.size != 9)
		return false;
	return true;
}

template<>
inline attribute*
attribute_type<mat4f>::create(const std::string& name)
{
	return new attribute(name, SP_FLOAT_32, 16);
}

template<>
inline bool attribute_type<mat4f>::operator()(const attribute& attribute_)
{
	if (attribute_.type != SP_FLOAT_32)
		return false;
	if (attribute_.size != 16)
		return false;
	return true;
}

template<>
inline attribute*
attribute_type<mat3d>::create(const std::string& name)
{
	return new attribute(name, SP_FLOAT_64, 9);
}

template<>
inline bool attribute_type<mat3d>::operator()(const attribute& attribute_)
{
	if (attribute_.type != SP_FLOAT_64)
		return false;
	if (attribute_.size != 9)
		return false;
	return true;
}

template<>
inline attribute* attribute_type<mat4d>::create(const std::string& name)
{
	return new attribute(name, SP_FLOAT_64, 16);
}

template<>
inline bool attribute_type<mat4d>::operator()(const attribute& attribute_)
{
	if (attribute_.type != SP_FLOAT_64)
		return false;
	if (attribute_.size != 16)
		return false;
	return true;
}

template<typename T>
attribute& element::create_attribute(const std::string& name,
		size_t array_size)
{
	attribute_type<T> attribute_type_;
	attribute* attr = 0;

	try
	{
		if (array_size == 0)
			attr = attribute_type_.create(name);
		else
			attr = attribute_type_.create(name, array_size);
		assert(attr);
		attributes.push_back(*attr);
	} catch (...)
	{
		delete attr;
		throw;
	}

	return *attr;
}

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
	void update() const
	{
		super::iterator it = elements.begin();
		super::iterator it_end = elements.end();
		for (; it != it_end; ++it)
		{
			it->update();
		}
	}

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

bool load_header(const std::string& filename, header& h);
bool save_header(const std::string& filename, const header& h);

} // namespace junkload

BOOST_FUSION_ADAPT_STRUCT(junkload::attribute,
		(std::string, name)
		(junkload::type_id, type)
		(std::size_t, size)
)

BOOST_FUSION_ADAPT_STRUCT(junkload::element,
		(std::string, name)
		(std::size_t, size)
		(std::vector<junkload::attribute>, attributes)
)

BOOST_FUSION_ADAPT_STRUCT(junkload::header,
		(junkload::mat4d, transform)
		(junkload::vec3d, min)
		(junkload::vec3d, max)
		(bool, big_endian)
		(std::vector<junkload::element>, elements)
)

BOOST_FUSION_ADAPT_STRUCT(junkload::vec3d,
		(double, array[0])
		(double, array[1])
		(double, array[2])
)

BOOST_FUSION_ADAPT_STRUCT(junkload::mat4d,
		(double, array[0])
		(double, array[1])
		(double, array[2])
		(double, array[3])
		(double, array[4])
		(double, array[5])
		(double, array[6])
		(double, array[7])
		(double, array[8])
		(double, array[9])
		(double, array[10])
		(double, array[11])
		(double, array[12])
		(double, array[13])
		(double, array[14])
		(double, array[15])
)

namespace stream_process
{
	using namespace junkload;
}

#endif /* JUNKLOAD_TYPES_HPP */
