/*
 * node.hpp
 *
 *  Created on: 24.08.2011
 *      Author: daniel
 */

#ifndef EQ_MESH_NODE_HPP
#define EQ_MESH_NODE_HPP

#include <eq/eq.h>
#include "aabb.hpp"
#include <boost/scoped_ptr.hpp>

namespace mesh
{

typedef vmml::vector<3, GLfloat> Pos3;

class Node
{
public:
	Node(int min_z, int max_z);
	~Node();

	void load();
	void unload();

	void is_loaded();

	void render();

private:
	int min_z;
	int max_z;

	boost::scoped_ptr<Node> left;
	boost::scoped_ptr<Node> right;

	GLuint vbuffer;
	GLuint ibuffer;
	GLsizei faces;

	mesh::aabb aabb;
};

} // namespace mesh

#endif /* EQ_MESH_NODE_HPP */
