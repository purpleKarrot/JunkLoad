/*
 * assign.hpp
 *
 *  Created on: 10.09.2011
 *      Author: daniel
 */

#ifndef BOX_ASSIGN_HPP
#define BOX_ASSIGN_HPP

#include <cmath>
#include <cassert>

namespace box
{

class Assign
{
public:
	Assign(int fanout, int height/*, data_set& data*/) :
			fanout(fanout),
			height(height)
//			data(data)
	{
		leaves = std::pow(fanout, height);
		branches = (leaves - 1) / (fanout - 1);
	}

	void process(int index);

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
//	data_set& data;

	int branches;
	int leaves;
};

} // namespace box

#endif /* BOX_ASSIGN_HPP */
