/*
 * node.cpp
 *
 *  Created on: 24.08.2011
 *      Author: daniel
 */

#include "node.hpp"

namespace mesh
{

Node::Node(int min_z, int max_z) :
		min_z(min_z),
		max_z(max_z),
		vbuffer(0),
		ibuffer(0),
		faces(0),
		min_bbox(0, 0, 0),
		max_bbox(0, 0, 0)
{
}

Node::~Node()
{
}

void Node::load(/* dataset */)
{
	// read min face index
	// read max face index
	// calculate number of faces
	// if small
	{
		// load
	}
	// else
	{
		int mask = (min_z ^ max_z) >> 1;

		left.reset(new Node(min_z, min_z | mask));
		right.reset(new Node(max_z & ~mask, max_z));

		left->load();
		right->load();
	}
}

void Node::unload()
{
	left.reset();
	right.reset();

	glDeleteBuffers(1, &vbuffer);
	vbuffer = 0;

	glDeleteBuffers(1, &ibuffer);
	ibuffer = 0;

	faces = 0;
}

void Node::render()
{
	// if not culling test
	// return;

	if (left && right)
	{
		left->render();
		right->render();
	}

	if (vbuffer && ibuffer)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);
		glDrawElements(GL_TRIANGLES, faces, GL_UNSIGNED_INT, 0);
	}
}

} // namespace mesh
