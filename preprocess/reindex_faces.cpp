#include "reindex_faces.hpp"

namespace stream_process
{

reindex_faces::reindex_faces(const params& params_) :
	_params(params_)
{
	_reindex_map.open(_params.reindex_map);

	if (!_reindex_map.is_open())
		throw exception("reindex map could not be opened.", SPROCESS_HERE);

	_faces_file.open(_params.faces_file);

	if (!_faces_file.is_open())
		throw exception("faces file could not be opened.", SPROCESS_HERE);

	switch (_params.index_type)
	{
	case SP_UINT_32:
		_reindex<3, uint32_t> (); // triangles only atm
		break;
	case SP_UINT_64:
		_reindex<3, uint64_t> (); // triangles only atm
		break;
	default:
		throw exception("NOT IMPLEMENTED YET.", SPROCESS_HERE);

	}
}

} // namespace stream_process
