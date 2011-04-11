#include "data_set_header.hpp"
#include "data_set_header_io.hpp"

#include <limits>

namespace stream_process
{

void header::read_from_file(const std::string& filename_base)
{
	data_set_header_io io;
	io.load(filename_base, *this);

	for (super::iterator it = elements.begin(), it_end = elements.end(); it != it_end; ++it)
	{
		it->update();
	}
}

void header::write_to_file(const std::string& filename_base) const
{
	data_set_header_io io;
	io.save(filename_base, *this);
}

void header::update() const
{
	for (super::iterator it = elements.begin(), it_end = elements.end(); it != it_end; ++it)
	{
		it->update();
	}
}

} // namespace stream_process
