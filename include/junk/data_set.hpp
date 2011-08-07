#ifndef PPK_DATA_SET_HPP
#define PPK_DATA_SET_HPP

#include <string>
#include <junk/types.hpp>
#include <junk/mapped_data_element.hpp>
#include <boost/iostreams/device/mapped_file.hpp>

namespace junk
{

class data_set
{
public:
	data_set(const std::string& filename, bool new_file = false);

	~data_set()
	{
	}

	junk::header& header()
	{
		return header_;
	}

	const junk::header& header() const
	{
		return header_;
	}

	mapped_data_element& vertex_map()
	{
		return vertex_map_;
	}

	const mapped_data_element& vertex_map() const
	{
		return vertex_map_;
	}

	mapped_data_element& face_map()
	{
		return face_map_;
	}

	const mapped_data_element& face_map() const
	{
		return face_map_;
	}

	void _setup(bool new_file);

protected:
	std::string filename_;
	junk::header header_;

	mapped_data_element vertex_map_;
	mapped_data_element face_map_;
};

} // namespace junk

#endif /* PPK_DATA_SET_HPP */
