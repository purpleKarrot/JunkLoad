#include "attribute.hpp"

#include <sstream>
#include <iomanip>
#include <limits>

#include <boost/algorithm/string.hpp>

namespace stream_process
{

attribute::attribute() :
	type(SP_INT_8), size(1), offset(0)
{
}

attribute::attribute(const std::string& name, data_type_id type, size_t size) :
	name(name), type(type), size(size), offset(0)
{
}

} // namespace stream_process
