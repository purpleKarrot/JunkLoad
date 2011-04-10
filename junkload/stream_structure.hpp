#ifndef __STREAM_PROCESS__STREAM_STRUCTURE__HPP__
#define __STREAM_PROCESS__STREAM_STRUCTURE__HPP__

#include "attribute.hpp"
#include "attribute_type.hpp"
#include <boost/noncopyable.hpp>

#include <iostream>

#include <map>
#include <list>

namespace stream_process
{

class stream_structure: private boost::noncopyable
{
public:
	typedef std::vector<attribute> super;
	super attributes;

	stream_structure()
	{
	}

	~stream_structure()
	{
	}

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

	attribute& create_custom_attribute(const std::string& name,
			size_t element_size_in_bytes, size_t array_size = 1);

	typedef std::map<std::string, attribute*> named_container;

	typedef named_container::iterator named_iterator;

	typedef named_container::const_iterator const_named_iterator;

	attribute* find(const std::string& name) const;

	// string stuff
	std::string to_string() const;

	// creates the string in 'header-format'
	std::string to_header_string() const;

	void print(std::ostream& os) const;

	friend std::ostream& operator<<(std::ostream& os,
			const stream_structure& ds)
	{
		return os << ds.to_string() << std::endl;
	}

	size_t compute_size_in_bytes() const;

	size_t compute_out_size_in_bytes() const;

	void compute_offsets();

	std::string get_name() const;

private:
	void _add_attribute(attribute& attr);

	std::map<std::string, attribute*> _by_name;
};

template<typename T>
attribute& stream_structure::create_attribute(const std::string& name,
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
