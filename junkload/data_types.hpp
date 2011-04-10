#ifndef __STREAM_PROCESS__DATA_TYPES__HPP__
#define __STREAM_PROCESS__DATA_TYPES__HPP__

#include <string>
#include <map>
#include "VMMLibIncludes.h"
#include <boost/cstdint.hpp>

namespace stream_process
{

enum data_type_id
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

template<typename T>
struct get_data_type_id_from_type
{
};

template<>
struct get_data_type_id_from_type<float>
{
	static const data_type_id value = SP_FLOAT_32;
};

template<>
struct get_data_type_id_from_type<double>
{
	static const data_type_id value = SP_FLOAT_64;
};

template<>
struct get_data_type_id_from_type<int8_t>
{
	static const data_type_id value = SP_INT_8;
};

template<>
struct get_data_type_id_from_type<uint8_t>
{
	static const data_type_id value = SP_UINT_8;
};

template<>
struct get_data_type_id_from_type<int16_t>
{
	static const data_type_id value = SP_INT_16;
};

template<>
struct get_data_type_id_from_type<uint16_t>
{
	static const data_type_id value = SP_UINT_16;
};

template<>
struct get_data_type_id_from_type<int32_t>
{
	static const data_type_id value = SP_INT_32;
};

template<>
struct get_data_type_id_from_type<uint32_t>
{
	static const data_type_id value = SP_UINT_32;
};

template<>
struct get_data_type_id_from_type<int64_t>
{
	static const data_type_id value = SP_INT_64;
};

template<>
struct get_data_type_id_from_type<uint64_t>
{
	static const data_type_id value = SP_UINT_64;
};

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
};

} // namespace stream_process

#endif
