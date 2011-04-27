//

#include <iostream>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include "sort_data_set.hpp"
#include "reindex_faces.hpp"

namespace sp = stream_process;


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
		sp::sort_data_set::params sort_params;
		sort_params.in_name = unsorted;
		sort_params.out_name = output;
		sort_params.tmp_name = reindex_map;
		sort_params.build_index_map = true;
		sort_params.number_of_threads = number_of_threads;
		sort_params.sort_attribute = "position";

		sp::sort_data_set sort(sort_params);
	}

	sp::mapped_data_set data_set_(unsorted);
	if (data_set_.get_header().face().size != 0)
	{
		const sp::header& h = data_set_.get_header();
		const sp::element& fs = h.face();
		const size_t num_faces = h.face().size;

		const sp::attribute& attr = get_attribute(fs, "vertex_indices");

		sp::reindex_faces::params ri_params;
		ri_params.faces_file = unsorted + ".faces";
		ri_params.reindex_map = reindex_map;
		ri_params.number_of_faces = num_faces;
		ri_params.index_type = attr.type;

		sp::reindex_faces rif(ri_params);

		sp::sort_data_set::params sort_params;
		sort_params.in_name = unsorted;
		sort_params.out_name = output;
		sort_params.tmp_name = unsorted + ".tmp";
		sort_params.build_index_map = false;
		sort_params.number_of_threads = number_of_threads;
		sort_params.sort_attribute = "vertex_indices";

		sp::sort_data_set sort(sort_params);
	}
}
