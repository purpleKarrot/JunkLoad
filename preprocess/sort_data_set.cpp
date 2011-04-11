#include "sort_data_set.hpp"

namespace stream_process
{

sort_data_set::sort_data_set(const params& params_) :
	_params(params_), _input(_params.in_name)
{
	const header& in_header = _input.get_header();

	const std::string& sort_attr_name = _params.sort_attribute;

	const attribute* attr;
	if (sort_attr_name == "position")
	{
		const element& ps = in_header.vertex();
		attr = &ps.get_attribute(sort_attr_name);
		data_type_id id_ = attr->type;
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
		const element& fs = in_header.face();
		attr = &fs.get_attribute(sort_attr_name);
		data_type_id id_ = attr->type;
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
