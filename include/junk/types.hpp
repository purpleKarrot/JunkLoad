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

#include <junk/VMMLibIncludes.h>

namespace junk
{

enum typid
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

} // namespace junk

BOOST_FUSION_DEFINE_STRUCT((junk), attribute,
	(std::string, name)
	(junk::typid, type)
	(std::size_t, size)
	(std::size_t, offset)
)

BOOST_FUSION_DEFINE_STRUCT((junk), element,
	(std::string, name)
	(std::size_t, size)
	(std::vector<junk::attribute>, attributes)
)

namespace junk
{

std::size_t size_in_bytes(typid type);

inline std::size_t size_in_bytes(const attribute& a)
{
	return a.size * size_in_bytes(a.type);
}

void compute_offsets(element& e);

bool has_attribute(const element& e, const std::string& name);

attribute& get_attribute(element& e, const std::string& name);
const attribute& get_attribute(const element& e, const std::string& name);

template<typename T>
void create_attribute(element& e, const std::string& name);

void create_attribute(element& e, const std::string& name, typid type,
		size_t size = 1);

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
struct get_typid_from_type
{
};

template<>
struct get_typid_from_type<float>
{
	static const typid value = SP_FLOAT_32;
};

template<>
struct get_typid_from_type<double>
{
	static const typid value = SP_FLOAT_64;
};

template<>
struct get_typid_from_type<int8_t>
{
	static const typid value = SP_INT_8;
};

template<>
struct get_typid_from_type<uint8_t>
{
	static const typid value = SP_UINT_8;
};

template<>
struct get_typid_from_type<int16_t>
{
	static const typid value = SP_INT_16;
};

template<>
struct get_typid_from_type<uint16_t>
{
	static const typid value = SP_UINT_16;
};

template<>
struct get_typid_from_type<int32_t>
{
	static const typid value = SP_INT_32;
};

template<>
struct get_typid_from_type<uint32_t>
{
	static const typid value = SP_UINT_32;
};

template<>
struct get_typid_from_type<int64_t>
{
	static const typid value = SP_INT_64;
};

template<>
struct get_typid_from_type<uint64_t>
{
	static const typid value = SP_UINT_64;
};

template<typename T, class Enable = void>
struct attribute_type
{
	static attribute create(const std::string& name)
	{
		return attribute(name, get_typid_from_type<T>::value, 1);
	}

	static bool test(const attribute& attr)
	{
		return attr.type == get_typid_from_type<T>::value && attr.size == 1;
	}
};

template<class V>
struct attribute_type<V, typename boost::enable_if<boost::qvm::is_v<V> >::type>
{
	typedef typename boost::qvm::v_traits<V>::scalar_type scalar_type;

	static const int size = boost::qvm::v_traits<V>::dim;
	static const int type = get_typid_from_type<scalar_type>::value;

	static attribute create(const std::string& name)
	{
		return attribute(name, type, size);
	}

	static bool test(const attribute& attr)
	{
		return attr.type == type && attr.size == size;
	}
};

template<>
inline attribute attribute_type<mat3f>::create(const std::string& name)
{
	return attribute(name, SP_FLOAT_32, 9, 0);
}

template<>
inline bool attribute_type<mat3f>::test(const attribute& attribute_)
{
	if (attribute_.type != SP_FLOAT_32)
		return false;
	if (attribute_.size != 9)
		return false;
	return true;
}

template<>
inline attribute attribute_type<mat4f>::create(const std::string& name)
{
	return attribute(name, SP_FLOAT_32, 16, 0);
}

template<>
inline bool attribute_type<mat4f>::test(const attribute& attribute_)
{
	if (attribute_.type != SP_FLOAT_32)
		return false;
	if (attribute_.size != 16)
		return false;
	return true;
}

template<>
inline attribute attribute_type<mat3d>::create(const std::string& name)
{
	return attribute(name, SP_FLOAT_64, 9, 0);
}

template<>
inline bool attribute_type<mat3d>::test(const attribute& attribute_)
{
	if (attribute_.type != SP_FLOAT_64)
		return false;
	if (attribute_.size != 9)
		return false;
	return true;
}

template<>
inline attribute attribute_type<mat4d>::create(const std::string& name)
{
	return attribute(name, SP_FLOAT_64, 16, 0);
}

template<>
inline bool attribute_type<mat4d>::test(const attribute& attribute_)
{
	if (attribute_.type != SP_FLOAT_64)
		return false;
	if (attribute_.size != 16)
		return false;
	return true;
}

template<typename T>
void create_attribute(element& e, const std::string& name)
{
	e.attributes.push_back(attribute_type<T>::create(name));
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
			compute_offsets(*it);
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
		element vertex;
		vertex.name = "vertex";
		elements.push_back(vertex);

		element face;
		face.name = "face";
		elements.push_back(face);

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
		element vertex;
		vertex.name = "vertex";
		elements.push_back(vertex);

		element face;
		face.name = "face";
		elements.push_back(face);

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

} // namespace junk

BOOST_FUSION_ADAPT_STRUCT(junk::header,
		(junk::mat4d, transform)
		(junk::vec3d, min)
		(junk::vec3d, max)
		(bool, big_endian)
		(std::vector<junk::element>, elements)
)

BOOST_FUSION_ADAPT_STRUCT(junk::vec3d,
		(double, array[0])
		(double, array[1])
		(double, array[2])
)

BOOST_FUSION_ADAPT_STRUCT(junk::mat4d,
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

#endif /* JUNKLOAD_TYPES_HPP */
