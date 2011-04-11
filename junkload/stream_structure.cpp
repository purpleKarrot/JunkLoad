#include "types.hpp"

#include <boost/range/algorithm/find_if.hpp>
using boost::range::find_if;

namespace junkload
{

struct match_name
{
	match_name(const std::string& name) :
		name(name)
	{
	}

	bool operator()(const attribute& attr)
	{
		return attr.name == name;
	}

	const std::string& name;
};

bool element::has_attribute(const std::string& name) const
{
	return find_if(attributes, match_name(name)) != attributes.end();
}

attribute& element::get_attribute(const std::string& name)
{
	std::vector<attribute>::iterator it = find_if(attributes, match_name(name));
	if (it == attributes.end())
	{
		throw std::runtime_error(
				std::string("could not find attribute with name ") + name + ".");
	}

	return *it;
}

const attribute& element::get_attribute(const std::string& name) const
{
	std::vector<attribute>::const_iterator it = find_if(attributes, match_name(name));
	if (it == attributes.end())
	{
		throw std::runtime_error(
				std::string("could not find attribute with name ") + name + ".");
	}

	return *it;
}

// creates an empty attribute with the specified name
void element::create_attribute(const std::string& name, type_id type,
		size_t size)
{
	attributes.push_back(attribute(name, type, size));
}

void element::compute_offsets()
{
	size_t offset = 0;
	for (std::vector<attribute>::iterator it = attributes.begin(), it_end =
			attributes.end(); it != it_end; ++it)
	{
		attribute& attr = *it;
		attr.offset = offset;
		offset += size_in_bytes(attr);
	}
}

} // namespace junkload
