#ifndef __STREAM_PROCESS__ATTRIBUTE_TYPE_TEST__HPP__
#define __STREAM_PROCESS__ATTRIBUTE_TYPE_TEST__HPP__

#include <stream_process/attribute.hpp>
#include <stream_process/data_types.hpp>
#include <stream_process/VMMLibIncludes.h>

namespace stream_process
{

template<typename T>
struct attribute_type
{
	inline attribute* create(const std::string& name)
	{
		return create(name, 1);
	}

	inline attribute* create(const std::string& name, size_t array_size)
	{
		data_type_id id_ = get_data_type_id_from_type<T> ()();
		if (id_ == SP_UNKNOWN_DATA_TYPE)
		{
			return new attribute(name, sizeof(T), array_size, true);
		}
		else
		{
			return new attribute(name, id_, array_size);
		}
	}

	inline bool test(const attribute& attribute_)
	{
		return operator()(attribute_);
	}

	inline bool operator()(const attribute& attribute_)
	{
		get_data_type_id_from_type<T> get_id;
		bool ok = get_id() == attribute_.get_data_type_id();
		if (!ok)
		{
			std::cerr << "expected attribute type: " << get_id() << std::endl;

			std::cerr << "actual attribute type:   "
					<< attribute_.get_data_type_id() << std::endl;
		}
		return ok;
	}
}; // class attribute_type


template<>
inline attribute*
attribute_type<vec2f>::create(const std::string& name)
{
	return new attribute(name, SP_FLOAT_32, 2);
}
;

template<>
inline bool attribute_type<vec2f>::operator()(const attribute& attribute_)
{
	if (attribute_.get_data_type_id() != SP_FLOAT_32)
		return false;
	if (attribute_.get_number_of_elements() != 2)
		return false;
	return true;

}
; // class attribute_type


template<>
inline attribute*
attribute_type<vec3f>::create(const std::string& name)
{
	return new attribute(name, SP_FLOAT_32, 3);
}
;

template<>
inline bool attribute_type<vec3f>::operator()(const attribute& attribute_)
{
	if (attribute_.get_data_type_id() != SP_FLOAT_32)
		return false;
	if (attribute_.get_number_of_elements() != 3)
		return false;
	return true;

}
; // class attribute_type


template<>
inline attribute*
attribute_type<vec3ui>::create(const std::string& name)
{
	return new attribute(name, SP_UINT_32, 3);
}
;

template<>
inline bool attribute_type<vec3ui>::operator()(const attribute& attribute_)
{
	if (attribute_.get_data_type_id() != SP_UINT_32)
		return false;
	if (attribute_.get_number_of_elements() != 3)
		return false;
	return true;

}
; // class attribute_type


template<>
inline attribute*
attribute_type<vec4f>::create(const std::string& name)
{
	return new attribute(name, SP_FLOAT_32, 4);
}
;

template<>
inline bool attribute_type<vec4f>::operator()(const attribute& attribute_)
{
	if (attribute_.get_data_type_id() != SP_FLOAT_32)
		return false;
	if (attribute_.get_number_of_elements() != 4)
		return false;
	return true;

}
; // class attribute_type


template<>
inline attribute*
attribute_type<vec2d>::create(const std::string& name)
{
	return new attribute(name, SP_FLOAT_64, 2);
}
;

template<>
inline bool attribute_type<vec2d>::operator()(const attribute& attribute_)
{
	if (attribute_.get_data_type_id() != SP_FLOAT_64)
		return false;
	if (attribute_.get_number_of_elements() != 2)
		return false;
	return true;

}
; // class attribute_type


template<>
inline attribute*
attribute_type<vec3d>::create(const std::string& name)
{
	return new attribute(name, SP_FLOAT_64, 3);
}
;

template<>
inline bool attribute_type<vec3d>::operator()(const attribute& attribute_)
{
	if (attribute_.get_data_type_id() != SP_FLOAT_64)
		return false;
	if (attribute_.get_number_of_elements() != 3)
		return false;
	return true;

}
; // class attribute_type


template<>
inline attribute*
attribute_type<vec4d>::create(const std::string& name)
{
	return new attribute(name, SP_FLOAT_64, 4);
}
;

template<>
inline bool attribute_type<vec4d>::operator()(const attribute& attribute_)
{
	if (attribute_.get_data_type_id() != SP_FLOAT_64)
		return false;
	if (attribute_.get_number_of_elements() != 4)
		return false;
	return true;

}
; // class attribute_type


template<>
inline attribute*
attribute_type<vec4ub>::create(const std::string& name)
{
	return new attribute(name, SP_UINT_8, 4);
}
;

template<>
inline bool attribute_type<vec4ub>::operator()(const attribute& attribute_)
{
	if (attribute_.get_data_type_id() != SP_UINT_8)
		return false;
	if (attribute_.get_number_of_elements() != 4)
		return false;
	return true;

}
; // class attribute_type


template<>
inline attribute*
attribute_type<mat3f>::create(const std::string& name)
{
	return new attribute(name, SP_FLOAT_32, 9);
}
;

template<>
inline bool attribute_type<mat3f>::operator()(const attribute& attribute_)
{
	if (attribute_.get_data_type_id() != SP_FLOAT_32)
		return false;
	if (attribute_.get_number_of_elements() != 9)
		return false;
	return true;

}
; // class attribute_type


template<>
inline attribute*
attribute_type<mat4f>::create(const std::string& name)
{
	return new attribute(name, SP_FLOAT_32, 16);
}
;

template<>
inline bool attribute_type<mat4f>::operator()(const attribute& attribute_)
{
	if (attribute_.get_data_type_id() != SP_FLOAT_32)
		return false;
	if (attribute_.get_number_of_elements() != 16)
		return false;
	return true;

}
; // class attribute_type


template<>
inline attribute*
attribute_type<mat3d>::create(const std::string& name)
{
	return new attribute(name, SP_FLOAT_64, 9);
}
;

template<>
inline bool attribute_type<mat3d>::operator()(const attribute& attribute_)
{
	if (attribute_.get_data_type_id() != SP_FLOAT_64)
		return false;
	if (attribute_.get_number_of_elements() != 9)
		return false;
	return true;

}
; // class attribute_type


template<>
inline attribute*
attribute_type<mat4d>::create(const std::string& name)
{
	return new attribute(name, SP_FLOAT_64, 16);
}
;

template<>
inline bool attribute_type<mat4d>::operator()(const attribute& attribute_)
{
	if (attribute_.get_data_type_id() != SP_FLOAT_64)
		return false;
	if (attribute_.get_number_of_elements() != 16)
		return false;
	return true;

}
; // class attribute_type


} // namespace stream_process

#endif

