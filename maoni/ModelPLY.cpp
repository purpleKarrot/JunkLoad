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
#include "ModelPLY.hpp"
#include "ply_io.h"
#include <boost/shared_ptr.hpp>
#include <stdexcept>
#include <vector>

#include <boost/la/all.hpp>
using namespace boost::la;

struct Vertex
{
	float position[3];
	float normal[3];
};

static void readVertices(PlyFile* file, std::vector<Vertex>& vertices)
{
	PlyProperty vertexProps[] =
	{
		{ (char*) "x", Float32, Float32, offsetof(Vertex, position[0]), 0, 0, 0, 0 },
		{ (char*) "y", Float32, Float32, offsetof(Vertex, position[1]), 0, 0, 0, 0 },
		{ (char*) "z", Float32, Float32, offsetof(Vertex, position[2]), 0, 0, 0, 0 },
	};

	for (int i = 0; i < 3; ++i)
		setup_property_ply(file, &vertexProps[i]);

	for (std::vector<Vertex>::iterator v = vertices.begin(); v != vertices.end(); ++v)
		get_element_ply(file, static_cast<void*> (&*v));
}

static void readTriangles(PlyFile* file, std::vector<unsigned int>& indices)
{
	struct Face
	{
		unsigned char size;
		unsigned int* vertices;
	} face;

	PlyProperty faceProps[] =
	{
		{ (char*) "vertex_indices", Uint32, Uint32, offsetof(Face, vertices), 1, Uint8, Uint8, offsetof(Face, size) }
	};

	ply_get_property(file, (char*) "face", &faceProps[0]);

	for (int i = 0, k = 0; i < indices.size(); i += 3)
	{
		ply_get_element(file, static_cast<void*> (&face));
		if(face.size != 3)
		{
			free(face.vertices);
			throw std::runtime_error("Encountered a face which does not have three vertices.");
		}

		indices[k++] = face.vertices[0];
		indices[k++] = face.vertices[1];
		indices[k++] = face.vertices[2];

		free(face.vertices);
	}
}

static void calc_normals(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
{
	for (std::size_t i = 0; i < vertices.size(); ++i)
		vref(vertices[i].normal) = zero_vector<float, 3>();

	// iterate over all triangles and add their normals to adjacent vertices
	for (size_t i = 0; i < indices.size(); i += 3)
	{
		std::size_t i0 = indices[i + 0];
		std::size_t i1 = indices[i + 1];
		std::size_t i2 = indices[i + 2];

		Vec3 p1 = vref(vertices[i0].position);
		Vec3 p2 = vref(vertices[i1].position);
		Vec3 p3 = vref(vertices[i2].position);
		Vec3 normal = cross(p2 - p1, p3 - p1);

		vref(vertices[i0].normal) += normal;
		vref(vertices[i1].normal) += normal;
		vref(vertices[i2].normal) += normal;
	}

	// normalize all the normals
	for (size_t i = 0; i < vertices.size(); ++i)
		vref(vertices[i].normal) /= magnitude(vertices[i].normal);
}

static void fix_scale(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
{
	Vec3 lower_left = vref(vertices[0].position);
	Vec3 upper_right = vref(vertices[0].position);

	// calculate bounding box
	for (size_t v = 1; v < vertices.size(); ++v)
	{
		Vec3 pos = vref(vertices[v].position);
		for (size_t i = 0; i < 3; ++i)
		{
			lower_left.data[i] = std::min(lower_left[i], pos[i]);
			upper_right.data[i] = std::max(upper_right[i], pos[i]);
		}
	}

	// find largest dimension and determine scale factor
	float factor = 0.0f;
	for (size_t i = 0; i < 3; ++i)
		factor = std::max(factor, upper_right[i] - lower_left[i]);

	factor = 2.f / factor;

	// determine scale offset
	Vec3 offset;
	for (size_t i = 0; i < 3; ++i)
		offset.data[i] = (lower_left[i] + upper_right[i]) * 0.5f;

	// scale the data
	for (size_t v = 0; v < vertices.size(); ++v)
	{
		for (size_t i = 0; i < 3; ++i)
		{
			vertices[v].position[i] -= offset[i];
			vertices[v].position[i] *= factor;
		}
	}
}

ModelPLY::ModelPLY(const char* filename) :
		Path(filename), vbuffer(0), ibuffer(0)
{
}

ModelPLY::~ModelPLY()
{
}

void do_fclose(FILE* file)
{
	if (file)
		fclose(file);
}

void ModelPLY::read_file() const
{
	glGenBuffers(1, &vbuffer);
	glGenBuffers(1, &ibuffer);

	std::vector<Vertex> vertices;    // (nVertices);
	std::vector<unsigned int> indices; // (nFaces * 3);

	boost::shared_ptr<FILE> file(fopen(path().c_str(), "r"), do_fclose);
	if (!file)
		throw std::runtime_error("unabple to open " + path());

	boost::shared_ptr<PlyFile> ply(read_ply(file.get()), free_ply);
	if (!ply)
		throw std::runtime_error("unabple to load " + path());

	for (int i = 0; i < ply->num_elem_types; ++i)
	{
		int elem_count;
		char* elem_name = setup_element_read_ply(ply.get(), i, &elem_count);

	    if (equal_strings((char*) "vertex", elem_name))
		{
	    	vertices.resize(elem_count);
			readVertices(ply.get(), vertices);
		}
		else if (equal_strings((char*) "face", elem_name))
		{
			faces = elem_count;
			indices.resize(elem_count * 3);
			readTriangles(ply.get(), indices);
		}
	}

	calc_normals(vertices, indices);
	fix_scale(vertices, indices);

	glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ModelPLY::draw(int myrank, int ranks) const
{
	if (!vbuffer)
		read_file();

	glColor3f(1.f, 0.5f, 0.5f);

	glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (const GLvoid*) offsetof(Vertex, position));

	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, sizeof(Vertex), (const GLvoid*) offsetof(Vertex, normal));

//	glEnableClientState(GL_COLOR_ARRAY);
//	glColorPointer(3, GL_UNSIGNED_BYTE, sizeof(Vertex), (const GLvoid*) offsetof(Vertex, r));

	glDrawRangeElements(GL_TRIANGLES, 0, faces*3 -1, faces*3, GL_UNSIGNED_INT, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
//	glDisableClientState(GL_COLOR_ARRAY);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ModelPLY::reset()
{
	glDeleteBuffers(1, &vbuffer);
	vbuffer = 0;

	glDeleteBuffers(1, &ibuffer);
	ibuffer = 0;
}

const char* const ModelPLY::filter() const
{
	return "PLY files (*.ply)";
}
