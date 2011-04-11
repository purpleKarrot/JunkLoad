#include "element.hpp"

#include "file_suffix_helper.hpp"

#include <boost/lexical_cast.hpp>

namespace stream_process
{

void element::update()
{
	compute_offsets();
}

std::string element::get_filename(const std::string& base_filename) const
{
	return base_filename + file_suffix_helper::get_suffix(_name);
}

} // namespace stream_process
