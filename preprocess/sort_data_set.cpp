#include "sort_data_set.hpp"

namespace stream_process
{

sort_data_set::sort_data_set(const params& params_) :
	_params(params_), _input(_params.in_name)
{
	const data_set_header& in_header = _input.get_header();

	const std::string& sort_attr_name = _params.sort_attribute;

	const attribute* attr;
	if (sort_attr_name == "position")
	{
		const stream_structure& ps = in_header.get_vertex_structure();
		attr = &ps.get_attribute(sort_attr_name);
		data_type_id id_ = attr->get_data_type_id();
		switch (id_)
		{
		case SP_FLOAT_32:
			_sort_vertices<float, uint32_t> ();
			break;
		case SP_FLOAT_64:
			_sort_vertices<double, uint64_t> ();
			break;
		default:
			// FIXME
			assert(!"NOT IMPLEMENTED YET.");
			break;
		}
	}
	else if (sort_attr_name == "vertex_indices")
	{
		const stream_structure& fs = in_header.get_face_structure();
		attr = &fs.get_attribute(sort_attr_name);
		data_type_id id_ = attr->get_data_type_id();
		switch (id_)
		{
		case SP_UINT_32:
			_sort_faces<uint32_t, uint32_t> ();
			break;
		case SP_UINT_64:
			_sort_faces<uint64_t, uint64_t> ();
		default:
			// FIXME
			assert(!"NOT IMPLEMENTED YET.");
			break;
		}
	}
	else
	{
		assert(!"NOT IMPLEMENTED YET.");
	}
}

} // namespace stream_process
