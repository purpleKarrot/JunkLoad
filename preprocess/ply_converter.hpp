#ifndef __STREAM_PROCESS__PLY_CONVERTER_HPP__
#define __STREAM_PROCESS__PLY_CONVERTER_HPP__

#include "preprocess_types.hpp"
#include "mapped_data_set.hpp"

#include "ply.h"

#include <vector> 
#include <string>

namespace stream_process
{

class ply_converter
{
public:
	struct params
	{
		// one of the two files must be ending in '.ply'.
		std::string source_file;
		std::string target_file;
	};

	ply_converter(const params& params_);

protected:
	void _from_ply();
	//void    _to_ply();

	void _setup_header_from_vertex_properties(data_set_header& h);
	void _setup_header_from_face_properties(data_set_header& h);

	void _read_meta_data();

	void _read_vertex_data();
	void _read_face_data();

	void _init_ply_sp_type_map();

	params _params;
	mapped_data_set* _data_set;

	std::vector<std::string> _color_names;

	// ply-reader related members
	std::vector<std::pair<std::string, size_t> > _other_properties;
	std::vector<size_t> _ply_sp_type_map;

	PlyProperty _create_ply_property(const std::string& name,
			int internal_type, int offset, int count_internal = 0,
			int count_offset = 0);

	PlyFile* _ply_file;

	PlyProperty** _vertex_properties;
	int _vertex_count;
	int _vertex_property_count;

	PlyProperty** _face_properties;
	int _face_count;
	int _face_property_count;

	int _element_count;
	char** _element_names;
	int _file_type; // file type, either ascii or binary
	float _version;

}; // class ply_loader

} // namespace stream_process

#endif
