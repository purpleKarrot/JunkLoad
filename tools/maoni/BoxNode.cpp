/*
 * BoxNode.cpp
 *
 *  Created on: Sep 13, 2011
 *      Author: dan
 */

#include "BoxNode.hpp"
#include "BoxModel.hpp"
#include <boost/bind.hpp>
#include <boost/range/algorithm/for_each.hpp>

BoxNode::BoxNode(const BoxModel& model, unsigned int index) :
		model(&model), index(index), vbuffer(0), ibuffer(0), faces(0)
{
	model.get_box(index, box);
}

BoxNode::~BoxNode()
{
	model->unload(index, vbuffer, ibuffer, faces);
}

void BoxNode::render(const frustum::Frustum& frustum, bool full)
{
	if (!full)
	{
		frustum::Frustum::Visibility visibility = frustum.test(box);

		if (visibility == frustum::Frustum::none)
		{
			model->unload(index, vbuffer, ibuffer, faces);
			children.clear();
			return;
		}

		if (visibility == frustum::Frustum::full)
		{
			full = true;
		}
	}

	if (model->is_leaf(index))
	{
		model->load(index, vbuffer, ibuffer, faces);

		if (vbuffer && ibuffer)
		{
			glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);

			glVertexPointer(3, GL_FLOAT, 24, (const GLvoid*) 0);
			glNormalPointer(GL_FLOAT, 24, (const GLvoid*) 12);
			glDrawElements(GL_TRIANGLES, faces, GL_UNSIGNED_INT, 0);
		}

		return;
	}

	if (children.empty())
	{
		uint32_t fanout = model->fanout();
		for (uint32_t i = 1; i <= fanout; ++i)
			children.push_back(BoxNode(*model, index * fanout + i));
	}

	boost::range::for_each(children,
			boost::bind(&BoxNode::render, _1, frustum, full));
}
