/*
 * assign.hpp
 *
 *  Created on: 10.09.2011
 *      Author: daniel
 */

#ifndef BOX_ASSIGN_HPP
#define BOX_ASSIGN_HPP

#include <cassert>
#include <jnk/data_set.hpp>
#include <boost/qvm/vec.hpp>

typedef boost::qvm::vec<float, 3> position;
typedef boost::qvm::vec<uint32_t, 3> triangle;

struct Box
{
	uint32_t min_vertex;
	uint32_t max_vertex;
	uint32_t min_face;
	uint32_t max_face;
	position min_bbox;
	position max_bbox;
};

class Assign
{
public:
	Assign(int fanout, int height, junk::data_set& data);

	const Box& process(const uint32_t index);

	int total_nodes() const
	{
		return branches + leaves;
	}

	bool is_leaf(int index) const
	{
		assert(index < total_nodes());
		return index >= branches;
	}

private:
	int fanout;
	int height;
	junk::data_set& data;

	int branches;
	int leaves;

	std::size_t face_max;

	junk::accessor<position> get_position;
	junk::accessor<triangle> get_triangle;
};

#endif /* BOX_ASSIGN_HPP */
