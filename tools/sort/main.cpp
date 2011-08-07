//

#include <iostream>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <boost/iostreams/device/mapped_file.hpp>

#include <junk/attribute_accessor.hpp>
#include <junk/data_set.hpp>
#include "intro_sort.hpp"
#include "z_order.hpp"

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

template<typename Value, typename Index, typename Less = std::less<Value> >
struct sort_reference
{
	union
	{
		Value value;
		Index new_index;
	};

	Index index;

	bool operator<(const sort_reference& other) const
	{
		return Less()(value, other.value);
	}
};

typedef boost::qvm::vec<float, 3> pos;

struct pos_order
{
	bool operator()(const pos& a, const pos& b)
	{
		return zorder_less(a, b);

		//using namespace boost::qvm;
		//return a%Y > b%Y;
	}
};

void sort_vertices(const junk::data_set& input, const std::string& sorted, const std::string& reindex_map)
{
	const junk::header& in_header = input.header();
	const junk::element& ps = in_header.vertex();
	const junk::attribute& attr = get_attribute(ps, "position");

	// no double atm
	assert(attr.type == junk::SP_FLOAT_32);

	std::size_t index = 0;

	const junk::element& vertices = input.header().vertex();
	junk::const_stream_range vertex_map = input.stream_range(0);

	const junk::element& vs = vertices;
	const junk::attribute& pos_attr = get_attribute(vs, "position");

//	std::size_t base_offset = sort_attr.offset;
//	base_offset += index * sizeof(float);

	junk::attribute_accessor<pos> get_pos(pos_attr.offset);

	junk::const_stream_range source_ = vertex_map;

	typedef sort_reference<pos, uint32_t, pos_order> sort_ref;

	const size_t number_of_elements = source_.size();
	const junk::element& source = input.header().vertex();
	junk::const_stream_range source_map = source_;

	// setup sort-file
	boost::iostreams::mapped_file_params tmp_params;
	tmp_params.path = reindex_map;
	tmp_params.new_file_size = sizeof(sort_ref) * number_of_elements;
	tmp_params.mode = std::ios_base::in | std::ios_base::out;

	boost::iostreams::mapped_file _tmp_file(tmp_params);
	if (!_tmp_file.is_open())
		throw std::runtime_error(reindex_map + " could not be created/opened.");

	// copy comparison-value and index into sort-file
	sort_ref* begin = reinterpret_cast<sort_ref*> (_tmp_file.data());
	sort_ref* end = begin + number_of_elements;

	index = 0;

	junk::const_stream_iterator sit = source_map.begin();
	for (sort_ref* it = begin; it != end; ++it, ++index, ++sit)
	{
		it->value = get_pos(*sit);
		it->index = index;
	}

	// do the actual sorting
	trip::intro_sort(begin, end);

	// setup result file
	boost::iostreams::mapped_file_params out_params;
	out_params.path = sorted + '.' + source.name_pl;
	out_params.mode = std::ios_base::in | std::ios_base::out;
	out_params.new_file_size = file_size_in_bytes(source);

	boost::iostreams::mapped_file _out_file(out_params);
	if (!_out_file.is_open())
		throw std::runtime_error("output file could not be created/opened.");

	char* out_data = _out_file.data();

	// copy data to result file
	const size_t in_point_size = size_in_bytes(source);
	for (sort_ref* it = begin; it != end; ++it, out_data += in_point_size)
		memcpy(out_data, source_map[it->index], in_point_size);

	// overwrite the value part of the sort_ref to create a 2-way reindex map
	sort_ref* ibegin = reinterpret_cast<sort_ref*> (_tmp_file.data());
	sort_ref* iend = ibegin + number_of_elements;
	sort_ref* it = ibegin;
	for (size_t index = 0; it != iend; ++it, ++index)
		ibegin[it->index].new_index = index;
}

void reindex_faces(const junk::element& faces_, const std::string& unsorted, const std::string& reindex_map_)
{
	const junk::attribute& attr = get_attribute(faces_, "indices");

	// uint32_t only atm
	assert(attr.type == junk::SP_UINT_32);

	boost::iostreams::mapped_file_source _reindex_map(reindex_map_);
	if (!_reindex_map.is_open())
		throw std::runtime_error("reindex map could not be opened.");

	boost::iostreams::mapped_file _faces_file(std::string(unsorted) + ".faces");
	if (!_faces_file.is_open())
		throw std::runtime_error("faces file could not be opened.");

	typedef sort_reference<pos, uint32_t> sort_ref;
	const sort_ref* reindex_map = reinterpret_cast<const sort_ref*> (_reindex_map.data());

	uint32_t* begin = reinterpret_cast<uint32_t*> (_faces_file.data());
	uint32_t* end = begin + faces_.size * attr.size;

	for (uint32_t* it = begin; it != end; ++it)
		*it = reindex_map[*it].new_index;
}

void sort_faces(const junk::data_set& input, const std::string& sorted)
{
	const junk::header& in_header = input.header();
	const junk::element& fs = in_header.face();
	const junk::attribute& attr = get_attribute(fs, "indices");

	// uint32_t only atm
	assert(attr.type == junk::SP_UINT_32);
	smallest_component_accessor<uint32_t> get_attr(attr.offset, attr.size);

	junk::const_stream_range source_ = input.stream_range(1);

	typedef sort_reference<uint32_t, uint32_t> sort_ref;

	const size_t number_of_elements = source_.size();
	const junk::element& source = input.header().face();
	junk::const_stream_range source_map = source_;

	// setup sort-file
	boost::iostreams::mapped_file_params tmp_params;
	tmp_params.path = std::string(sorted) + ".tmp";
	tmp_params.new_file_size = sizeof(sort_ref) * number_of_elements;
	tmp_params.mode = std::ios_base::in | std::ios_base::out;

	boost::iostreams::mapped_file _tmp_file(tmp_params);
	if (!_tmp_file.is_open())
		throw std::runtime_error(tmp_params.path + " could not be created/opened.");

	// copy comparison-value and index into sort-file
	sort_ref* begin = reinterpret_cast<sort_ref*> (_tmp_file.data());
	sort_ref* end = begin + number_of_elements;

	size_t index = 0;

	junk::const_stream_iterator sit = source_map.begin();
	for (sort_ref* it = begin; it != end; ++it, ++index, ++sit)
	{
		it->value = get_attr(*sit);
		it->index = index;
	}

	// do the actual sorting
	trip::intro_sort(begin, end);

	// setup result file
	boost::iostreams::mapped_file_params out_params;
	out_params.path = sorted + '.' + source.name_pl;
	out_params.mode = std::ios_base::in | std::ios_base::out;
	out_params.new_file_size = file_size_in_bytes(source);

	boost::iostreams::mapped_file _out_file(out_params);
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

	junk::data_set input(argv[1]);
	const junk::element& faces = input.header().face();

	sort_vertices(input, sorted, reindex_map);

	if (faces.size != 0)
	{
		reindex_faces(faces, unsorted, reindex_map);
		sort_faces(input, sorted);
	}

	junk::save_header(sorted, input.header());
}
