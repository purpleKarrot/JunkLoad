#ifndef __STREAM_PROCESS__DATA_ELEMENT__HPP__
#define __STREAM_PROCESS__DATA_ELEMENT__HPP__

#include "attribute.hpp"
#include "attribute_type.hpp"
#include <boost/noncopyable.hpp>

#include <iostream>

#include <map>
#include <list>

#include <boost/lexical_cast.hpp>
#include <string>

namespace stream_process
{

class element
{
public:
	element(const std::string& name = std::string()) :
		_name(name), _size(0)
	{
	}

	element(const element& other) :
		_name(other._name), _size(other._size), attributes_(other.attributes_)
	{
	}

	~element()
	{
	}

public: // getter and setter
	std::string name() const
	{
		return _name;
	}

	void name(const std::string& value)
	{
		_name = value;
	}

	std::size_t size() const
	{
		return _size;
	}

	void size(std::size_t value)
	{
		_size = value;
	}

	const std::vector<attribute>& attributes() const
	{
		return attributes_;
	}

	void attributes(const std::vector<attribute>& value)
	{
		attributes_ = value;
	}

public:

	bool empty() const
	{
		return _size == 0;
	}

	void update();

	std::string get_name() const;

public:
	std::string get_filename(const std::string& base_filename) const;

	size_t get_number_of_attributes() const;

	bool has_attribute(const std::string& name) const;

	bool has_attribute(const std::string& name, data_type_id id_,
			size_t array_size) const;

	attribute& get_attribute(const std::string& name);

	const attribute& get_attribute(const std::string& name) const;

	void create_attribute(const attribute& attr);

	template<typename T>
	attribute& create_attribute(const std::string& name, size_t array_size = 0);

	attribute& create_attribute(const std::string& name,
			data_type_id data_type_id_, size_t array_size = 1);

	const attribute* find(const std::string& name) const;

	void compute_offsets();

	void _add_attribute(attribute& attr);

public:
	std::string _name;
	std::size_t _size;
	std::vector<attribute> attributes_;
};

// returns the size of a  point/face/...
inline std::size_t size_in_bytes(const element& e)
{
	std::size_t size = 0;

	std::vector<attribute>::const_iterator it = e.attributes_.begin();
	std::vector<attribute>::const_iterator it_end = e.attributes_.end();

	for (; it != it_end; ++it)
	{
		size += size_in_bytes(*it);
	}

	return size;
}

// returns the size of the whole data set
inline std::size_t file_size_in_bytes(const element& e)
{
	return e._size * size_in_bytes(e);
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
		_add_attribute(*attr);
	} catch (...)
	{
		delete attr;
		throw;
	}

	return *attr;
}

} // namespace stream_process

#endif
