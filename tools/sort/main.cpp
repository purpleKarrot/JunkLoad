//

#include <iostream>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include "sort_data_set.hpp"
#include "reindex_faces.hpp"

void sort_vertices(const std::string& unsorted, const std::string& sorted, const std::string& reindex_map)
{
	junk::sort_data_set::params sort_params;
	sort_params.in_name = unsorted;
	sort_params.out_name = sorted;
	sort_params.tmp_name = reindex_map;
	sort_params.build_index_map = true;

	junk::sort_data_set sort(sort_params);

	const junk::header& in_header = sort._input.get_header();
	const junk::element& ps = in_header.vertex();
	const junk::attribute& attr = get_attribute(ps, "position");

	switch (attr.type)
	{
	case junk::SP_FLOAT_32:
		sort._sort_vertices<float, uint32_t> ();
		break;
	case junk::SP_FLOAT_64:
		sort._sort_vertices<double, uint64_t> ();
		break;
	default:
		// FIXME
		assert(!"NOT IMPLEMENTED YET.");
		break;
	}
}

void do_reindex_faces(const junk::element& faces, const std::string& unsorted, const std::string& reindex_map)
{
	const junk::attribute& attr = get_attribute(faces, "vertex_indices");

	junk::reindex_faces::params ri_params;
	ri_params.faces_file = std::string(unsorted) + ".face";
	ri_params.reindex_map = reindex_map;
	ri_params.number_of_faces = faces.size;
	ri_params.index_type = attr.type;

	junk::reindex_faces rif(ri_params);

	rif._reindex_map.open(rif._params.reindex_map);

	if (!rif._reindex_map.is_open())
		throw std::runtime_error("reindex map could not be opened.");

	rif._faces_file.open(rif._params.faces_file);

	if (!rif._faces_file.is_open())
		throw std::runtime_error("faces file could not be opened.");

	switch (rif._params.index_type)
	{
	case junk::SP_UINT_32:
		rif._reindex<3, uint32_t> (); // triangles only atm
		break;
	default:
		throw std::runtime_error("NOT IMPLEMENTED YET.");
	}
}

void sort_faces(const std::string& unsorted, const std::string& sorted)
{
	junk::sort_data_set::params sort_params;
	sort_params.in_name = unsorted;
	sort_params.out_name = sorted;
	sort_params.tmp_name = std::string(unsorted) + ".tmp";
	sort_params.build_index_map = false;

	junk::sort_data_set sort(sort_params);

	const junk::header& in_header = sort._input.get_header();
	const junk::element& fs = in_header.face();
	const junk::attribute& attr = get_attribute(fs, "vertex_indices");

	switch (attr.type)
	{
	case junk::SP_UINT_32:
		sort._sort_faces<uint32_t, uint32_t> ();
		break;
	default:
		// FIXME
		assert(!"NOT IMPLEMENTED YET.");
		break;
	}
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

	sort_vertices(unsorted, sorted, reindex_map);

	if (faces.size != 0)
	{
		do_reindex_faces(faces, unsorted, reindex_map);
		sort_faces(unsorted, sorted);
	}
}
