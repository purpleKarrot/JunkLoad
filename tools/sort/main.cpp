//

#include <iostream>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <junk/mapped_data_set.hpp>
#include "intro_sort.hpp"
#include "preprocess_types.hpp"

template<typename T>
class smallest_component_accessor
{
public:
	smallest_component_accessor(std::ptrdiff_t offset, std::size_t size) :
			offset(offset), size(size)
	{
	}

	T operator()(const char* data) const
	{
		const T* begin = reinterpret_cast<const T*> (data + offset);
		return *std::min_element(begin, begin + size);
	}

private:
	std::ptrdiff_t offset;
	std::size_t size;
};

void sort_vertices(const junk::mapped_data_set& input, const std::string& sorted, const std::string& reindex_map)
{
	const junk::header& in_header = input.get_header();
	const junk::element& ps = in_header.vertex();
	const junk::attribute& attr = get_attribute(ps, "position");

	// no double atm
	assert(attr.type == junk::SP_FLOAT_32);

	std::size_t index = 0;

	typedef junk::attribute_accessor<float> accessor_type;

	const junk::element& vertices = input.get_vertex_element();
	const junk::mapped_data_element& vertex_map = input.get_vertex_map();

	assert(input.get_vertex_map().is_open());

	const junk::element& vs = vertices; //.get_structure();
	const junk::attribute& sort_attr = get_attribute(vs, "position");

	std::size_t base_offset = sort_attr.offset;
	base_offset += index * sizeof(float);

	accessor_type get_attr(base_offset);

	typedef float value_t;
	typedef uint32_t index_t;
	typedef accessor_type value_accessor_t;
	const junk::mapped_data_element& source_ = vertex_map;
	const value_accessor_t& accessor = get_attr;

	typedef junk::preprocess::sort_reference<value_t, index_t> sort_ref;
	typedef junk::preprocess::index_reference<index_t> index_ref;

	assert(sizeof(value_t) == sizeof(index_t));
	assert(sizeof(sort_ref) == sizeof(index_ref));

	const size_t number_of_elements = source_.size();
	const junk::element& source = source_.get_element();
	const junk::mapped_data_element& source_map = source_;

	assert(source_.is_open());

	// setup sort-file
	boost::iostreams::mapped_file_params tmp_params;
	tmp_params.path = reindex_map;
	tmp_params.new_file_size = sizeof(sort_ref) * number_of_elements;
	tmp_params.mode = std::ios_base::in | std::ios_base::out;

	boost::iostreams::mapped_file _tmp_file;
	_tmp_file.open(tmp_params);
	if (!_tmp_file.is_open())
		throw std::runtime_error(reindex_map + " could not be created/opened.");

	// copy comparison-value and index into sort-file
	sort_ref* begin = reinterpret_cast<sort_ref*> (_tmp_file.data());
	sort_ref* end = begin + number_of_elements;

	index = 0;

	assert(source_map.is_open());

	junk::mapped_data_element::const_iterator sit = source_map.begin();
	for (sort_ref* it = begin; it != end; ++it, ++index, ++sit)
	{
		sort_ref& ref = *it;
		const char* point = *sit;

		ref.value = accessor(point);
		ref.index = index;
	}

	// do the actual sorting
	trip::intro_sort(begin, end, std::less<sort_ref>());

	// setup result file
	boost::iostreams::mapped_file_params out_params;
	out_params.path = sorted + '.' + source.name;
	out_params.mode = std::ios_base::in | std::ios_base::out;
	out_params.new_file_size = file_size_in_bytes(source);

	boost::iostreams::mapped_file _out_file;
	_out_file.open(out_params);

	if (!_out_file.is_open())
		throw std::runtime_error("output file could not be created/opened.");

	char* out_data = _out_file.data();

	// copy data to result file
	const size_t in_point_size = size_in_bytes(source);
	for (sort_ref* it = begin; it != end; ++it, out_data += in_point_size)
		memcpy(out_data, source_map[it->index], in_point_size);

	// overwrite the value part of the sort_ref to create a 2-way reindex map
	index_ref* ibegin = reinterpret_cast<index_ref*> (_tmp_file.data());
	index_ref* iend = ibegin + number_of_elements;
	index_ref* it = ibegin;
	for (size_t index = 0; it != iend; ++it, ++index)
	{
		index_ref& iref = *it;
		ibegin[iref.old_index].new_index = index;
	}
}

void reindex_faces(const junk::element& faces_, const std::string& unsorted, const std::string& reindex_map_)
{
	const junk::attribute& attr = get_attribute(faces_, "vertex_indices");

	// uint32_t only atm
	assert(attr.type == junk::SP_UINT_32);

	boost::iostreams::mapped_file_source _reindex_map(reindex_map_);
	if (!_reindex_map.is_open())
		throw std::runtime_error("reindex map could not be opened.");

	boost::iostreams::mapped_file _faces_file(std::string(unsorted) + ".face");
	if (!_faces_file.is_open())
		throw std::runtime_error("faces file could not be opened.");

	typedef junk::preprocess::index_reference<uint32_t> ref_type;
	const ref_type* reindex_map = reinterpret_cast<const ref_type*> (_reindex_map.data());

	uint32_t* begin = reinterpret_cast<uint32_t*> (_faces_file.data());
	uint32_t* end = begin + faces_.size * attr.size;

	for (uint32_t* it = begin; it != end; ++it)
		*it = reindex_map[*it].new_index;
}

void sort_faces(const junk::mapped_data_set& input, const std::string& sorted)
{
	const junk::header& in_header = input.get_header();
	const junk::element& fs = in_header.face();
	const junk::attribute& attr = get_attribute(fs, "vertex_indices");

	// uint32_t only atm
	assert(attr.type == junk::SP_UINT_32);

	smallest_component_accessor<uint32_t> get_attr(attr.offset, attr.size);

	typedef uint32_t value_t;
	typedef uint32_t index_t;
	const junk::mapped_data_element& source_ = input.get_face_map();

	typedef junk::preprocess::sort_reference<value_t, index_t> sort_ref;
	typedef junk::preprocess::index_reference<index_t> index_ref;

	assert(sizeof(value_t) == sizeof(index_t));
	assert(sizeof(sort_ref) == sizeof(index_ref));

	const size_t number_of_elements = source_.size();
	const junk::element& source = source_.get_element();
	const junk::mapped_data_element& source_map = source_;

	assert(source_.is_open());

	// setup sort-file
	boost::iostreams::mapped_file_params tmp_params;
	tmp_params.path = std::string(sorted) + ".tmp";
	tmp_params.new_file_size = sizeof(sort_ref) * number_of_elements;
	tmp_params.mode = std::ios_base::in | std::ios_base::out;

	boost::iostreams::mapped_file _tmp_file;
	_tmp_file.open(tmp_params);
	if (!_tmp_file.is_open())
		throw std::runtime_error(tmp_params.path + " could not be created/opened.");

	// copy comparison-value and index into sort-file
	sort_ref* begin = reinterpret_cast<sort_ref*> (_tmp_file.data());
	sort_ref* end = begin + number_of_elements;

	size_t index = 0;

	assert(source_map.is_open());

	junk::mapped_data_element::const_iterator sit = source_map.begin();
	for (sort_ref* it = begin; it != end; ++it, ++index, ++sit)
	{
		sort_ref& ref = *it;
		const char* point = *sit;

		ref.value = get_attr(point);
		ref.index = index;
	}

	// do the actual sorting
	trip::intro_sort(begin, end, std::less<sort_ref>());

	// setup result file
	boost::iostreams::mapped_file_params out_params;
	out_params.path = sorted + '.' + source.name;
	out_params.mode = std::ios_base::in | std::ios_base::out;
	out_params.new_file_size = file_size_in_bytes(source);

	boost::iostreams::mapped_file _out_file;
	_out_file.open(out_params);

	if (!_out_file.is_open())
		throw std::runtime_error("output file could not be created/opened.");

	char* out_data = _out_file.data();

	// copy data to result file
	const std::size_t in_point_size = size_in_bytes(source);
	for (sort_ref* it = begin; it != end; ++it, out_data += in_point_size)
		memcpy(out_data, source_map[it->index], in_point_size);

	_tmp_file.close();
	boost::filesystem::remove(tmp_params.path);
}

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		std::cerr << "usage: sort <unsorted> <sorted>" << std::endl;
		return 0;
	}

	const char* unsorted = argv[1];
	const char* sorted = argv[2];
	std::string reindex_map = std::string(sorted) + ".reindex_map";

	junk::mapped_data_set input(argv[1]);
	const junk::element& faces = input.get_header().face();

	sort_vertices(input, sorted, reindex_map);

	if (faces.size != 0)
	{
		reindex_faces(faces, unsorted, reindex_map);
		sort_faces(input, sorted);
	}

	junk::save_header(sorted, input.get_header());
}
