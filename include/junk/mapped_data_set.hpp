#ifndef __STREAM_PROCESS__MAPPED_DATA_SET__HPP__
#define __STREAM_PROCESS__MAPPED_DATA_SET__HPP__

#include <junk/types.hpp>
#include <junk/mapped_data_element.hpp>
#include <junk/stream_iterator.hpp>
#include <junk/attribute_accessor.hpp>

#include <boost/iostreams/device/mapped_file.hpp>

#include <string>

namespace junk
{

class mapped_data_set
{
public:
	typedef boost::iostreams::mapped_file mapped_file;
	typedef boost::iostreams::mapped_file_params mapped_file_params;

	typedef stream_iterator iterator;
	typedef const_stream_iterator const_iterator;

	mapped_data_set(const std::string& filename, bool new_file = false);

	~mapped_data_set();

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
		assert(_vertex_map);
		return *_vertex_map;
	}

	const mapped_data_element& vertex_map() const
	{
		assert(_vertex_map);
		return *_vertex_map;
	}

	mapped_data_element& face_map()
	{
		assert(_face_map);
		return *_face_map;
	}

	const mapped_data_element& face_map() const
	{
		assert(_face_map);
		return *_face_map;
	}

	void _setup(bool new_file);

protected:
	junk::header header_;
	mapped_data_element* _vertex_map;
	mapped_data_element* _face_map;

	std::string filename_;
};

} // namespace junk

#endif
