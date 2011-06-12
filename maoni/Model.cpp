/*
 * libMaoni - common viewing framework
 *
 * Copyright (C) 2011 Daniel Pfeifer
 * Visualization and Multimedia Lab, University of Zurich
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <Maoni/glew.h>
#include "Model.hpp"

Model::Model(const char* filename) :
		Path(filename), vbuffer(0), ibuffer(0)
{
}

Model::~Model()
{
}

void Model::read_file() const
{
	glGenBuffers(1, &vbuffer);
	glGenBuffers(1, &ibuffer);

	data_set.reset(new junk::mapped_data_set(path()));

	junk::mapped_data_element& vertices = data_set->get_vertex_map();
	junk::mapped_data_element& indices = data_set->get_face_map();

	glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.data_size(), vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.data_size(), indices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Model::draw(int myrank, int ranks) const
{
	if (!vbuffer)
		read_file();

	glColor3f(1.f, 0.5f, 0.5f);

	glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);

	junk::header& header = data_set->get_header();
	junk::element& vertex = header.get_element("vertex");
	junk::attribute& position = vertex.attributes[0];
	assert(position.name == "position");
	assert(position.size == 3); // x, y, z
	assert(gl_type(position.type) == GL_FLOAT);
	assert(position.offset == 0);

	faces = header.get_element("face").size;

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(position.size, gl_type(position.type), size_in_bytes(vertex), (const GLvoid*) position.offset);

//	glEnableClientState(GL_NORMAL_ARRAY);
//	glNormalPointer(GL_FLOAT, sizeof(Vertex), (const GLvoid*) offsetof(Vertex, normal));

//	glEnableClientState(GL_COLOR_ARRAY);
//	glColorPointer(3, GL_UNSIGNED_BYTE, sizeof(Vertex), (const GLvoid*) offsetof(Vertex, r));

	glDrawRangeElements(GL_TRIANGLES, 0, faces * 3 - 1, faces * 3, GL_UNSIGNED_INT, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
//	glDisableClientState(GL_NORMAL_ARRAY);
//	glDisableClientState(GL_COLOR_ARRAY);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Model::reset()
{
	glDeleteBuffers(1, &vbuffer);
	vbuffer = 0;

	glDeleteBuffers(1, &ibuffer);
	ibuffer = 0;

	data_set.reset();
}

const char* const Model::filter() const
{
	return "JUNK files (*.junk)";
}
