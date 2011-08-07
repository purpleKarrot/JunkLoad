#include <junk/data_set.hpp>

#include <iostream>

namespace junk
{

data_set::data_set(const std::string& filename, bool new_file) :
		filename_(filename)
{
	if (!new_file)
	{
		junk::load_header(filename, header_);
		_setup(false);
	}
}

void data_set::_setup(bool new_file)
{
	try
	{
		vertex_map_.open(header_.vertex(), filename_, new_file);

		if (new_file || header_.face().size != 0)
			face_map_.open(header_.face(), filename_, new_file);

	} catch (std::exception& e)
	{
		std::string msg("Opening point data set ");
		msg += filename_;
		msg += " failed.";
		throw std::runtime_error(msg);
	}
}

} // namespace junk
