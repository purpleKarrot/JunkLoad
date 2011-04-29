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

#include <Maoni.hpp>

struct JunkModel:   Model
{
	JunkModel(const char* filename, int myrank, int ranks);

	void draw() const;

	GLuint vao;
	GLuint vbuffer;
	GLuint ibuffer;
};

//MESH_LOADER(junk, Junk Files)
//{
//	std::cout << glGetError() << std::endl;
//
//	model.reset(new JunkModel(filename, myrank, ranks));
//}

RENDER_ALGORITHM(JunkRender, )
{
	static const JunkModel m(0, 0, 0);

	m.draw();
}

struct MyVertex
{
	//! position
	float x, y, z;

	//! normal
	float nx, ny, nz;

	//! texture coordinates
	float s, t;
};

JunkModel::JunkModel(const char* filename, int myrank, int ranks)
{
	MyVertex vertex[3];

	vertex[0].x = 0.0;
	vertex[0].y = 0.0;
	vertex[0].z = 0.0;
	vertex[0].nx = 0.0;
	vertex[0].ny = 0.0;
	vertex[0].nz = 1.0;
	vertex[0].s = 0.0;
	vertex[0].t = 0.0;

	vertex[1].x = 1.0;
	vertex[1].y = 0.0;
	vertex[1].z = 0.0;
	vertex[1].nx = 0.0;
	vertex[1].ny = 0.0;
	vertex[1].nz = 1.0;
	vertex[1].s = 1.0;
	vertex[1].t = 0.0;

	vertex[2].x = 0.0;
	vertex[2].y = 1.0;
	vertex[2].z = 0.0;
	vertex[2].nx = 0.0;
	vertex[2].ny = 0.0;
	vertex[2].nz = 1.0;
	vertex[2].s = 0.0;
	vertex[2].t = 1.0;

	ushort index[3];
	index[0] = 0;
	index[1] = 1;
	index[2] = 2;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MyVertex) * 3, &vertex[0].x, GL_STATIC_DRAW);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(MyVertex), (void*) offsetof(MyVertex, x));

//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MyVertex), (void*) offsetof(MyVertex, x));
//	glEnableVertexAttribArray(1);
//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MyVertex), (void*) offsetof(MyVertex, nx));
//	glEnableVertexAttribArray(2);
//	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MyVertex), (void*) offsetof(MyVertex, s));

	glGenBuffers(1, &ibuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ushort) * 3, index, GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void JunkModel::draw() const
{
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, (void*) 0);
}
