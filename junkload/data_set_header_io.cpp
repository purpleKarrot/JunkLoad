#include "data_set_header_io.hpp"
#include "file_suffix_helper.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

namespace stream_process
{

bool load_header(std::istream& in, header& h);
bool save_header(std::ostream& out, const header& h);

void data_set_header_io::load(const std::string& filename, header& h)
{
	std::ifstream file(filename.c_str());
	load_header(file, h);
}

void data_set_header_io::save(const std::string& filename, const header& h)
{
	std::ofstream file(filename.c_str());
	save_header(file, h);
}

} // namespace stream_process
