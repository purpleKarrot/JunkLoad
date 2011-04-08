#ifndef __STREAM_PROCESS__DATA_TYPES__HPP__
#define __STREAM_PROCESS__DATA_TYPES__HPP__

#include <string>
#include <map>
#include "VMMLibIncludes.h"
#include <boost/cstdint.hpp>

namespace stream_process
{

typedef boost::uint32_t data_type_id;

static const data_type_id SP_UNKNOWN_DATA_TYPE = 1;

static const data_type_id SP_FLOAT_32 = 1 << 1;
static const data_type_id SP_FLOAT_64 = 1 << 2;

static const data_type_id SP_INT_8 = 1 << 8;
static const data_type_id SP_UINT_8 = 1 << 9;
static const data_type_id SP_INT_16 = 1 << 10;
static const data_type_id SP_UINT_16 = 1 << 11;
static const data_type_id SP_INT_32 = 1 << 12;
static const data_type_id SP_UINT_32 = 1 << 13;
static const data_type_id SP_INT_64 = 1 << 14;
static const data_type_id SP_UINT_64 = 1 << 15;
static const data_type_id SP_INT_128 = 1 << 16;
static const data_type_id SP_UINT_128 = 1 << 17;

static const data_type_id SP_BOOL = 1 << 24;
static const data_type_id SP_STRING = 1 << 25;

template<typename T>
struct get_data_type_id_from_type
{
	data_type_id operator()()
	{
		return SP_UNKNOWN_DATA_TYPE;
	}
	static std::string get_name_string()
	{
		return "unimplemented type.";
	}

}; // strcut get_data_type_id_from_type


class data_type_helper
{
public:
	static data_type_helper* get_singleton_ptr();
	static data_type_helper& get_singleton();

	const std::string& get_default_name(data_type_id type) const;
	data_type_id get_data_type_id(const std::string& name) const;

	// WARNING: does not take array flag into account
	size_t get_size_in_bytes(data_type_id type) const;

protected:
	void _fill_db();

	std::map<std::string, data_type_id> _type_names;
	std::map<data_type_id, size_t> _type_size_in_bytes;
	std::map<data_type_id, std::string> _default_name;

	static data_type_helper* _instance;
private:
	data_type_helper();

}; // class data_type_helper


template<>
struct get_data_type_id_from_type<float>
{
	data_type_id operator()()
	{
		return SP_FLOAT_32;
	}
}; // strcut get_data_type_id_from_type


template<>
struct get_data_type_id_from_type<double>
{
	data_type_id operator()()
	{
		return SP_FLOAT_64;
	}
}; // strcut get_data_type_id_from_type


template<>
struct get_data_type_id_from_type<int8_t>
{
	data_type_id operator()()
	{
		return SP_INT_8;
	}
}; // strcut get_data_type_id_from_type


template<>
struct get_data_type_id_from_type<uint8_t>
{
	data_type_id operator()()
	{
		return SP_UINT_8;
	}
}; // strcut get_data_type_id_from_type


template<>
struct get_data_type_id_from_type<int16_t>
{
	data_type_id operator()()
	{
		return SP_INT_16;
	}
}; // strcut get_data_type_id_from_type


template<>
struct get_data_type_id_from_type<uint16_t>
{
	data_type_id operator()()
	{
		return SP_UINT_16;
	}
}; // strcut get_data_type_id_from_type


template<>
struct get_data_type_id_from_type<int32_t>
{
	data_type_id operator()()
	{
		return SP_INT_32;
	}
}; // strcut get_data_type_id_from_type


template<>
struct get_data_type_id_from_type<uint32_t>
{
	data_type_id operator()()
	{
		return SP_UINT_32;
	}
}; // strcut get_data_type_id_from_type


//// FIXME TODO this should be caught by one of the uint defines, it's not.
//// INVESTIGATE
//template<>
//struct get_data_type_id_from_type< std::size_t >
//{
//    data_type_id operator()()
//    {
//        if ( sizeof( void* ) == 4 )
//            return SP_UINT_32;
//        else
//            return SP_UINT_64;
//    }
//}; // strcut get_data_type_id_from_type


template<>
struct get_data_type_id_from_type<int64_t>
{
	data_type_id operator()()
	{
		return SP_INT_64;
	}
}; // strcut get_data_type_id_from_type


template<>
struct get_data_type_id_from_type<uint64_t>
{
	data_type_id operator()()
	{
		return SP_UINT_64;
	}
}; // strcut get_data_type_id_from_type


template<>
struct get_data_type_id_from_type<std::string>
{
	data_type_id operator()()
	{
		return SP_STRING;
	}
}; // strcut get_data_type_id_from_type


} // namespace stream_process

#endif

