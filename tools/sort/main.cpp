//

#include <iostream>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <boost/iostreams/device/mapped_file.hpp>

#include <jnk/accessor.hpp>
#include <jnk/data_set.hpp>
#include "intro_sort.hpp"
#include <zix/z_order.hpp>

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

void sort_vertices(const junk::data_set& input, const std::string& sorted,
		const std::string& reindex_map)
{
	junk::const_stream_range vertex_map = input.stream_range(0);
	junk::accessor<pos> get_pos = input.get_accessor<pos>("vertex", "position");

	typedef sort_reference<pos, uint32_t, pos_order> sort_ref;

	std::size_t number_of_elements = vertex_map.size();
	const junk::element& source = input.get_element("vertex");

	// setup sort-file
	boost::iostreams::mapped_file_params tmp_params;
	tmp_params.path = reindex_map;
	tmp_params.new_file_size = sizeof(sort_ref) * number_of_elements;
	tmp_params.mode = std::ios_base::in | std::ios_base::out;
	boost::iostreams::mapped_file _tmp_file(tmp_params);

	// copy comparison-value and index into sort-file
	sort_ref* begin = reinterpret_cast<sort_ref*> (_tmp_file.begin());
	sort_ref* end   = reinterpret_cast<sort_ref*> (_tmp_file.end()  );

	std::size_t index = 0;
	junk::const_stream_iterator sit = vertex_map.begin();

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

	char* out_data = _out_file.data();

	// copy data to result file
	const size_t in_point_size = size_in_bytes(source);
	for (sort_ref* it = begin; it != end; ++it, out_data += in_point_size)
		memcpy(out_data, vertex_map[it->index], in_point_size);

	// overwrite the value part of the sort_ref to create a 2-way reindex map
	index = 0;
	for (sort_ref* it = begin; it != end; ++it, ++index)
		begin[it->index].new_index = index;
}

void reindex_faces(const std::string& unsorted, const std::string& reindex_map_)
{
	boost::iostreams::mapped_file_source _reindex_map(reindex_map_);
	boost::iostreams::mapped_file _faces_file(std::string(unsorted) + ".faces");

	typedef sort_reference<pos, uint32_t> sort_ref;
	const sort_ref* reindex_map = reinterpret_cast<const sort_ref*> (_reindex_map.data());

	uint32_t* begin = reinterpret_cast<uint32_t*> (_faces_file.begin());
	uint32_t* end   = reinterpret_cast<uint32_t*> (_faces_file.end()  );

	for (uint32_t* it = begin; it != end; ++it)
		*it = reindex_map[*it].new_index;
}

void sort_faces(const junk::data_set& input, const std::string& sorted)
{
	const junk::attribute& attr = input.get_attribute("face", "indices");

	// uint32_t only atm
	assert(attr.type == junk::u_int_32);
	smallest_component_accessor<uint32_t> get_attr(attr.offset, attr.size);

	junk::const_stream_range source_ = input.stream_range(1);

	typedef sort_reference<uint32_t, uint32_t> sort_ref;

	const size_t number_of_elements = source_.size();
	const junk::element& source = input.get_element("face");
	junk::const_stream_range source_map = source_;

	// setup sort-file
	boost::iostreams::mapped_file_params tmp_params;
	tmp_params.path = std::string(sorted) + ".tmp";
	tmp_params.new_file_size = sizeof(sort_ref) * number_of_elements;
	tmp_params.mode = std::ios_base::in | std::ios_base::out;
	boost::iostreams::mapped_file _tmp_file(tmp_params);

	// copy comparison-value and index into sort-file
	sort_ref* begin = reinterpret_cast<sort_ref*> (_tmp_file.begin());
	sort_ref* end   = reinterpret_cast<sort_ref*> (_tmp_file.end()  );

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

	const junk::data_set input(argv[1]);
	const junk::element& faces = input.get_element("face");

	sort_vertices(input, sorted, reindex_map);
	reindex_faces(unsorted, reindex_map);

	sort_faces(input, sorted);

	boost::filesystem::copy_file(unsorted, sorted);
}
