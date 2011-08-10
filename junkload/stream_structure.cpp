#include <junk/types.hpp>

#include <stdexcept>
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

} // namespace junk
