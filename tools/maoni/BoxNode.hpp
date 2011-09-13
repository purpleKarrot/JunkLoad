/*
 * BoxNode.hpp
 *
 *  Created on: Sep 13, 2011
 *      Author: dan
 */

#ifndef BOX_NODE_HPP
#define BOX_NODE_HPP

#include <Maoni.hpp>
#include <frustum/frustum.hpp>

class BoxModel;

class BoxNode
{
public:
	BoxNode(const BoxModel& model, unsigned int index);

	~BoxNode();

	void render(const frustum::Frustum& frustum, bool full);

private:
	const BoxModel* model;
	unsigned int index;

	GLuint vbuffer;
	GLuint ibuffer;
	GLsizei faces;

	frustum::AlignedBox box;

	std::vector<BoxNode> children;
};

#endif /* BOX_NODE_HPP */
