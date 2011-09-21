/*
 * BoxModel.cpp
 *
 *  Created on: Sep 13, 2011
 *      Author: dan
 */

#include "BoxModel.hpp"
#include <boost/range/algorithm/for_each.hpp>
#include <boost/phoenix/operator/arithmetic.hpp>
#include <boost/phoenix/core/argument.hpp>

using namespace boost::phoenix::arg_names;

struct Box
{
	uint32_t min_vertex;
	uint32_t max_vertex;
	uint32_t min_face;
	uint32_t max_face;
	frustum::Vertex min_bbox;
	frustum::Vertex max_bbox;
};

BoxModel::BoxModel(const char* filename) :
		Path(filename),
		fanout_(4),
		height_(3),
		leaves_(std::pow<double>(fanout_, height_)),
		branches_((leaves_ - 1) / (fanout_ - 1)),
		loaded_nodes(leaves_)
{
}

BoxModel::~BoxModel()
{
}

void BoxModel::draw(int nodes) const
{
	if(!root)
		read_file();

	allowed_nodes = nodes;

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	frustum::Matrix projection, modelview;
	glGetFloatv(GL_PROJECTION_MATRIX, projection);
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

	frustum::Frustum frustum(projection * modelview);
	root->render(frustum, false);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}

void BoxModel::reset()
{
	root.reset();
	data_set.reset();
	loaded_nodes.reset();
}

void BoxModel::read_file() const
{
	data_set.reset(new junk::data_set(path()));
	root.reset(new BoxNode(*this, 0));
}

const char* const BoxModel::filter() const
{
	return "JUNK files (*.junk)";
}

void BoxModel::get_box(uint32_t index, frustum::AlignedBox& abox) const
{
	junk::const_stream_range box_range = data_set->stream_range(2);
	const Box& box = *reinterpret_cast<const Box*>(box_range[index]);

	abox.min = box.min_bbox;
	abox.max = box.max_bbox;
}

void BoxModel::load(uint32_t index, GLuint& vbuffer, GLuint& ibuffer,
		GLsizei& faces) const
{
	assert(is_leaf(index));

	if (allowed_nodes <= loaded_nodes.count())
		return;

	std::cout << "loading " << index << std::endl;

	junk::const_stream_range vtx_range = data_set->stream_range(0);
	junk::const_stream_range idx_range = data_set->stream_range(1);
	junk::const_stream_range box_range = data_set->stream_range(2);
	const Box& box = *reinterpret_cast<const Box*>(box_range[index]);

	// load vetices directly
	{
		uint32_t vertices = 1 + box.max_vertex - box.min_vertex;
		const GLvoid* data = vtx_range[box.min_vertex];
		GLsizei size = vertices * 24;

		glGenBuffers(1, &vbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	// recalc face offsets
	{
		faces = 1 + box.max_face - box.min_face;
		std::vector<uint32_t> face_data(
				reinterpret_cast<const uint32_t*>(idx_range[box.min_face]),
				reinterpret_cast<const uint32_t*>(idx_range[box.max_face]) + 3);

		boost::range::for_each(face_data, arg1 -= box.min_vertex);

		glGenBuffers(1, &ibuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, face_data.size() * 4,
				&face_data[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	loaded_nodes.set(index - branches_);
}

void BoxModel::unload(uint32_t index, GLuint& vbuffer, GLuint& ibuffer,
		GLsizei& faces) const
{
	if (!vbuffer && !ibuffer)
		return;

	std::cout << "unloading " << index << std::endl;

	glDeleteBuffers(1, &vbuffer);
	glDeleteBuffers(1, &ibuffer);
	vbuffer = ibuffer = faces = 0;

	if (is_leaf(index))
		loaded_nodes.reset(index - branches_);
}
