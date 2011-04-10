#include "data_types.hpp"

namespace stream_process
{

data_type_helper* data_type_helper::_instance = 0;

data_type_helper::data_type_helper()
{
	_fill_db();
}

data_type_helper*
data_type_helper::get_singleton_ptr()
{
	if (_instance == 0)
		_instance = new data_type_helper();
	return _instance;
}

data_type_helper&
data_type_helper::get_singleton()
{
	if (_instance == 0)
		_instance = new data_type_helper();
	return *_instance;
}

const std::string&
data_type_helper::get_default_name(data_type_id type) const
{
	std::map<data_type_id, std::string>::const_iterator it =
			_default_name.find(type);
	if (it != _default_name.end())
	{
		return (*it).second;
	}

	throw std::runtime_error("requested name of unknown type.");
}

data_type_id data_type_helper::get_data_type_id(const std::string& name) const
{
	std::map<std::string, data_type_id>::const_iterator it = _type_names.find(
			name);
	if (it != _type_names.end())
	{
		return (*it).second;
	}

	throw std::runtime_error("requested data_type_id of unknown type.");
}

size_t data_type_helper::get_size_in_bytes(data_type_id type) const
{
	std::map<data_type_id, size_t>::const_iterator it =
			_type_size_in_bytes.find(type);
	if (it != _type_size_in_bytes.end())
	{
		return (*it).second;
	}

	throw std::runtime_error("requested data_type_id of unknown type.");
}

void data_type_helper::_fill_db()
{
	_type_names["float"] = SP_FLOAT_32;
	_type_names["float32"] = SP_FLOAT_32;

	_type_names["double"] = SP_FLOAT_64;
	_type_names["float64"] = SP_FLOAT_64;

	_type_names["byte"] = SP_INT_8;
	_type_names["char"] = SP_INT_8;
	_type_names["int8"] = SP_INT_8;
	_type_names["ubyte"] = SP_UINT_8;
	_type_names["uchar"] = SP_UINT_8;
	_type_names["uint8"] = SP_UINT_8;

	_type_names["short"] = SP_INT_16;
	_type_names["int16"] = SP_INT_16;
	_type_names["ushort"] = SP_UINT_16;
	_type_names["uint16"] = SP_UINT_16;

	_type_names["long"] = SP_INT_32;
	_type_names["int"] = SP_INT_32;
	_type_names["int32"] = SP_INT_32;
	_type_names["uint"] = SP_UINT_32;
	_type_names["uint32"] = SP_UINT_32;

	_type_names["int64"] = SP_INT_64;
	_type_names["uint64"] = SP_UINT_64;

	_default_name[SP_FLOAT_32] = "float";
	_default_name[SP_FLOAT_64] = "double";
	_default_name[SP_INT_8] = "int8";
	_default_name[SP_UINT_8] = "uint8";
	_default_name[SP_INT_16] = "int16";
	_default_name[SP_UINT_16] = "uint16";
	_default_name[SP_INT_32] = "int32";
	_default_name[SP_UINT_32] = "uint32";
	_default_name[SP_INT_64] = "int64";
	_default_name[SP_UINT_64] = "uint64";

	_type_size_in_bytes[SP_FLOAT_32] = sizeof(float);
	_type_size_in_bytes[SP_FLOAT_64] = sizeof(double);
	_type_size_in_bytes[SP_INT_8] = sizeof(int8_t);
	_type_size_in_bytes[SP_UINT_8] = sizeof(uint8_t);
	_type_size_in_bytes[SP_INT_16] = sizeof(int16_t);
	_type_size_in_bytes[SP_UINT_16] = sizeof(uint16_t);
	_type_size_in_bytes[SP_INT_32] = sizeof(int32_t);
	_type_size_in_bytes[SP_UINT_32] = sizeof(uint32_t);
	_type_size_in_bytes[SP_INT_64] = sizeof(int64_t);
	_type_size_in_bytes[SP_UINT_64] = sizeof(uint64_t);
	_type_size_in_bytes[SP_UINT_64] = sizeof(bool);
	_type_size_in_bytes[SP_UINT_64] = sizeof(std::string);
}

} // namespace stream_process
