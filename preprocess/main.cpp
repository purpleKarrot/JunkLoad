//

#include <iostream>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include "ply/convert.hpp"
#include "sort_data_set.hpp"
#include "reindex_faces.hpp"
#include "find_optimal_transform.hpp"

namespace sp = stream_process;

static void _apply_optimal_transform(const std::string& filename,
		bool full_optimal_transform)
{
	sp::mapped_data_set data_set_(filename);
	bool has_faces = data_set_.get_header().has_faces();

	sp::attribute_accessor<sp::vec3f> get_position(
			data_set_.get_vertex_element().get_attribute("position").offset());

	const sp::data_set_header& h = data_set_.get_header();

	if (full_optimal_transform)
	{
		std::cout << "preprocessor: optimal transform in progress..."
				<< std::endl;

		sp::attribute_accessor<sp::vec3f> get_position(
				data_set_.get_vertex_element().get_attribute("position").offset());

		sp::optimal_transform<sp::vec3f, sp::attribute_accessor<sp::vec3f>,
				sp::mapped_data_element> ot;

		ot.set_accessor(get_position);
		ot.analyze(data_set_.get_vertex_map());
		ot.apply(data_set_.get_vertex_map());

		data_set_.get_header().set_transform(ot.get_transformation_matrix());

		std::cout << "transform " << ot.get_transformation_matrix()
				<< std::endl;
	}
	else
	{
		std::cout << "preprocessor: no optimal transform step." << std::endl;

		sp::vec3f min = h.get_aabb_min<float> ();
		sp::vec3f max = h.get_aabb_max<float> ();

		sp::vec3f diag = max - min;
		size_t index = diag.find_max_index();

		if (index != 2)
		{
			std::cout << "swapping axes " << index << " and 2 " << std::endl;
			sp::mapped_data_element& vertices = data_set_.get_vertex_map();
			sp::mapped_data_element::iterator vit = vertices.begin(), vit_end =
					vertices.end();
			for (; vit != vit_end; ++vit)
			{
				sp::vec3f& v = get_position(*vit);
				//std::cout << "pre  " << v << std::endl;
				std::swap(v[index], v[2]);
				//std::cout << "post " << v << std::endl;
			}
		}
	}

	data_set_.compute_aabb();
	data_set_.get_header().write_to_file(filename);
}

int main(int argc, char* argv[])
{
	std::string input = argc > 1 ? argv[1] : "bunny.ply";
	std::string output = argc > 2 ? argv[2] : input + ".stream";

	int number_of_threads = boost::thread::hardware_concurrency();
	if (number_of_threads < 1)
		number_of_threads = 1;

	std::string unsorted = input;

	// if source is ply, convert...
	if (boost::algorithm::iends_with(input, ".ply"))
	{
		unsorted = output + ".unsorted";
		sp::ply_convert(input.c_str(), unsorted);
	}

	std::string reindex_map = output + ".reindex_map";

	_apply_optimal_transform(unsorted, true);

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
	if (data_set_.get_header().has_faces())
	{
		const sp::data_set_header& h = data_set_.get_header();
		const sp::element& fs = h.get_face_structure();
		const size_t num_faces = h.get_number_of_faces();

		const sp::attribute& attr = fs.get_attribute("vertex_indices");

		sp::reindex_faces::params ri_params;
		ri_params.faces_file = unsorted + ".faces";
		ri_params.reindex_map = reindex_map;
		ri_params.number_of_faces = num_faces;
		ri_params.index_type = attr.type();

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
