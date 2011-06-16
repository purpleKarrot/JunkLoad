#include <junk/types.hpp>

#include <boost/range/algorithm/find_if.hpp>
using boost::range::find_if;

namespace junk
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

bool has_attribute(const element& e, const std::string& name)
{
	return find_if(e.attributes, match_name(name)) != e.attributes.end();
}

attribute& get_attribute(element& e,const std::string& name)
{
	std::vector<attribute>::iterator it = find_if(e.attributes, match_name(name));
	if (it == e.attributes.end())
	{
		throw std::runtime_error(
				std::string("could not find attribute with name ") + name + ".");
	}

	return *it;
}

const attribute& get_attribute(const element& e, const std::string& name)
{
	std::vector<attribute>::const_iterator it = find_if(e.attributes, match_name(name));
	if (it == e.attributes.end())
	{
		throw std::runtime_error(
				std::string("could not find attribute with name ") + name + ".");
	}

	return *it;
}

// creates an empty attribute with the specified name
void create_attribute(element& e, const std::string& name, typid type, size_t size)
{
	e.attributes.push_back(attribute(type, name, size, 0));
}

void compute_offsets(element& e)
{
	size_t offset = 0;
	std::vector<attribute>::iterator it = e.attributes.begin();
	std::vector<attribute>::iterator end = e.attributes.end();

	for (; it != end; ++it)
	{
		attribute& attr = *it;
		attr.offset = offset;
		offset += size_in_bytes(attr);
	}
}

} // namespace junk
