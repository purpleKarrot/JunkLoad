#ifndef __STREAM_PROCESS__REINDEX_FACES__HPP__
#define __STREAM_PROCESS__REINDEX_FACES__HPP__

#include "preprocess_types.hpp"
#include "special_accessors.hpp"
#include "attribute_type.hpp"

#include <boost/filesystem.hpp>
#include <boost/iostreams/device/mapped_file.hpp>

#include <string>

namespace stream_process
{

namespace io = boost::iostreams;
namespace fs = boost::filesystem;

class reindex_faces
{
public:
	struct params
	{
		std::string faces_file;
		std::string reindex_map;
		size_t number_of_faces;
		data_type_id index_type;
	};

	reindex_faces(const params& params_);

protected:
	template<size_t M, typename T>
	void _reindex();

	params _params;
	io::mapped_file_source _reindex_map;
	io::mapped_file _faces_file;

}; // class reindex_faces


template<size_t M, typename T>
void reindex_faces::_reindex()
{
	typedef vmml::vector<M, T> vector_type;
	typedef preprocess::index_reference<T> ref_type;

	const char* ref_ = _reindex_map.data();

	const ref_type* reindex_map = reinterpret_cast<const ref_type*> (ref_);

	char* faces = _faces_file.data();

	vector_type* begin = reinterpret_cast<vector_type*> (faces);
	vector_type* end = begin + _params.number_of_faces;

	for (vector_type* it = begin; it != end; ++it)
	{
		vector_type& face = *it;

		for (size_t index = 0; index < M; ++index)
		{
			T& vertex_index = face[index];
			const ref_type& ref = reindex_map[vertex_index];

			vertex_index = ref.new_index;
		}
	}
}

} // namespace stream_process

#endif
