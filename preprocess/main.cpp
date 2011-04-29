//

#include <iostream>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include "sort_data_set.hpp"
#include "reindex_faces.hpp"


int main(int argc, char* argv[])
{
	std::string input = argc > 1 ? argv[1] : "bunny.ply";
	std::string output = argc > 2 ? argv[2] : input + ".stream";

	int number_of_threads = boost::thread::hardware_concurrency();
	if (number_of_threads < 1)
		number_of_threads = 1;

	std::string unsorted = input;

	std::string reindex_map = output + ".reindex_map";

	// sort file
	{
		junk::sort_data_set::params sort_params;
		sort_params.in_name = unsorted;
		sort_params.out_name = output;
		sort_params.tmp_name = reindex_map;
		sort_params.build_index_map = true;
		sort_params.number_of_threads = number_of_threads;
		sort_params.sort_attribute = "position";

		junk::sort_data_set sort(sort_params);
	}

	junk::mapped_data_set data_set_(unsorted);
	if (data_set_.get_header().face().size != 0)
	{
		const junk::header& h = data_set_.get_header();
		const junk::element& fs = h.face();
		const size_t num_faces = h.face().size;

		const junk::attribute& attr = get_attribute(fs, "vertex_indices");

		junk::reindex_faces::params ri_params;
		ri_params.faces_file = unsorted + ".faces";
		ri_params.reindex_map = reindex_map;
		ri_params.number_of_faces = num_faces;
		ri_params.index_type = attr.type;

		junk::reindex_faces rif(ri_params);

		junk::sort_data_set::params sort_params;
		sort_params.in_name = unsorted;
		sort_params.out_name = output;
		sort_params.tmp_name = unsorted + ".tmp";
		sort_params.build_index_map = false;
		sort_params.number_of_threads = number_of_threads;
		sort_params.sort_attribute = "vertex_indices";

		junk::sort_data_set sort(sort_params);
	}
}
