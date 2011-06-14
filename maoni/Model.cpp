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
#include <boost/foreach.hpp>

static void set_color(float x)
{
	assert(x >= 0.f);
	assert(x <= 1.f);

	float i;
	float f = std::modf(x * 3, &i);

	switch (int(i))
	{
	case 0:
		glColor3f(f, 0.f, 0.f);
		break;
	case 1:
		glColor3f(1.f, f, 0.f);
		break;
	default:
		glColor3f(1.f, 1.f, f);
		break;
	}
}

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

void Model::draw(int ranges) const
{
	if (!vbuffer)
		read_file();

	glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);

	junk::header& header = data_set->get_header();
	junk::element& vertex = header.get_element("vertex");

	bool vertex_array = false;
	bool normal_array = false;
	bool color_array = false;

	BOOST_FOREACH(junk::attribute& attr, vertex.attributes)
	{
		if (attr.name == "position")
		{
			vertex_array = true;
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(attr.size, gl_type(attr.type), size_in_bytes(vertex), (const GLvoid*) attr.offset);
		}
		else if (attr.name == "normal")
		{
			normal_array = true;
			glEnableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(gl_type(attr.type), size_in_bytes(vertex), (const GLvoid*) attr.offset);
		}
		else if (attr.name == "color")
		{
			color_array = true;
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(attr.size, gl_type(attr.type), size_in_bytes(vertex), (const GLvoid*) 12 /*attr.offset*/);
		}
	}

	std::size_t faces = header.get_element("face").size;

	for (int i = 0; i < ranges; ++i)
	{
		int frag = faces / ranges;
		int size = ((i + 1) * (frag + 1) * 3) - (i * frag * 3);
		GLvoid* index = (char*) NULL + (i * frag * 3 * sizeof(unsigned int));

		set_color(float(i) / float(ranges));
		glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, index);
	}

	if (vertex_array)
		glDisableClientState(GL_VERTEX_ARRAY);

	if (normal_array)
		glDisableClientState(GL_NORMAL_ARRAY);

	if (color_array)
		glDisableClientState(GL_COLOR_ARRAY);

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
