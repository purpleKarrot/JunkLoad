#ifndef __STREAM_PROCESS__SORT_DATA_SET__HPP__
#define __STREAM_PROCESS__SORT_DATA_SET__HPP__

#include "mapped_data_set.hpp"
#include "intro_sort.hpp"
#include "preprocess_types.hpp"
#include "special_accessors.hpp"

#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/filesystem.hpp>

#include <string>
#include <cstddef>

namespace stream_process
{

class sort_data_set
{
public:
	struct params
	{
		std::string in_name;
		std::string out_name;
		std::string tmp_name;
		bool build_index_map;
		size_t number_of_threads;
		std::string sort_attribute;
	};

	sort_data_set(const params& params_);

protected:
	template<typename value_t, typename index_t>
	void _sort_vertices(size_t index = 2); // default to z-sort

	template<typename value_t, typename index_t>
	void _sort_faces();

	// warning: only call this function with types that are
	// of the same size ( sizeof(Tvalue) == sizeof(Tindex) )! - FIXME sfinae
	template<typename value_t, typename index_t, typename value_accessor_t>
	void _sort_file(const mapped_data_element& source,
			const value_accessor_t& accessor_);

	params _params;

	mapped_data_set _input;

	boost::iostreams::mapped_file _tmp_file;
	boost::iostreams::mapped_file _out_file;
};

template<typename value_t, typename index_t>
void sort_data_set::_sort_vertices(size_t index)
{
	typedef attribute_accessor<value_t> accessor_type;

	element& vertices = _input.get_vertex_element();
	mapped_data_element& vertex_map = _input.get_vertex_map();

	assert(_input.get_vertex_map().is_open());

	const element& vs = vertices; //.get_structure();
	const attribute& sort_attr = vs.get_attribute(_params.sort_attribute);

	size_t base_offset = sort_attr.offset;
	base_offset += index * sizeof(value_t);

	accessor_type get_attr(base_offset);
	_sort_file<value_t, index_t, accessor_type> (vertex_map, get_attr);
}

template<typename value_t, typename index_t>
void sort_data_set::_sort_faces()
{
	typedef smallest_component_accessor<3, value_t> accessor_type;

	const header& in_header = _input.get_header();
	const element& fs = in_header.face();
	const attribute& sort_attr = fs.get_attribute(_params.sort_attribute);

	size_t size = in_header.face().size();

	accessor_type get_attr(sort_attr.offset);

	_sort_file<value_t, index_t, accessor_type> (_input.get_face_map(),
			get_attr);
}

template<typename value_t, typename index_t, typename value_accessor_t>
void sort_data_set::_sort_file(const mapped_data_element& source_,
		const value_accessor_t& accessor)
{
	typedef preprocess::sort_reference<value_t, index_t> sort_ref;
	typedef preprocess::index_reference<index_t> index_ref;

	assert(sizeof(value_t) == sizeof(index_t));
	assert(sizeof(sort_ref) == sizeof(index_ref));

	const size_t number_of_elements = source_.size();
	const element& source = source_.get_element();
	const mapped_data_element& source_map = source_;

	assert(source_.is_open());

	// setup sort-file
	boost::iostreams::mapped_file_params tmp_params;
	tmp_params.path = _params.tmp_name;
	tmp_params.new_file_size = sizeof(sort_ref) * number_of_elements;
	tmp_params.mode = std::ios_base::in | std::ios_base::out;

	_tmp_file.open(tmp_params);
	if (!_tmp_file.is_open())
	{
		throw std::runtime_error(
				_params.tmp_name + " could not be created/opened.");
	}

	// copy comparison-value and index into sort-file
	sort_ref* begin = reinterpret_cast<sort_ref*> (_tmp_file.data());
	sort_ref* end = begin + number_of_elements;

	size_t index = 0;

	assert(source_map.is_open());

	mapped_data_element::const_iterator sit = source_map.begin();
	for (sort_ref* it = begin; it != end; ++it, ++index, ++sit)
	{
		sort_ref& ref = *it;
		const stream_data* point = *sit;

		ref.value = accessor(point);
		ref.index = index;
	}

	// do the actual sorting
	{
		typedef trip::intro_sort<sort_ref, sort_ref*, std::less<sort_ref> >
				intro_sort_type;

		boost::threadpool::pool threadpool(_params.number_of_threads);
		intro_sort_type is(threadpool);
		is(begin, end);
	}

	// setup result file
	boost::iostreams::mapped_file_params out_params;
	out_params.path = source.get_filename(_params.out_name);
	out_params.mode = std::ios_base::in | std::ios_base::out;
	out_params.new_file_size = file_size_in_bytes(source);

	_out_file.open(out_params);

	if (!_out_file.is_open())
	{
		throw std::runtime_error("output file could not be created/opened.");
	}

	char* out_data = _out_file.data();

	// copy data to result file
	const size_t in_point_size = size_in_bytes(source);
	for (sort_ref* it = begin; it != end; ++it, out_data += in_point_size)
	{
		sort_ref& sr = *it;
		const stream_data* src = source_map[sr.index];

		memcpy(out_data, src, in_point_size);
	}

	// build reindex map (for reindexing faces)
	if (_params.build_index_map)
	{
		// we overwrite the value part of the sort_ref to create a 2-way
		// reindex map
		index_ref* ibegin = reinterpret_cast<index_ref*> (_tmp_file.data());
		index_ref* iend = ibegin + number_of_elements;
		index_ref* it = ibegin;
		for (size_t index = 0; it != iend; ++it, ++index)
		{
			index_ref& iref = *it;
			ibegin[iref.old_index].new_index = index;
		}
	}
	else
	{
		_tmp_file.close();
		boost::filesystem::remove(tmp_params.path);
	}

	_input.get_header().write_to_file(_params.out_name);
}

} // namespace stream_process

#endif
