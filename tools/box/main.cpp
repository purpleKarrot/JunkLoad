/*
 * main.cpp
 *
 *  Created on: Aug 22, 2011
 *      Author: daniel
 */

#include "assign.hpp"

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cerr << "usage: box <data_set>" << std::endl;
		return 0;
	}

	junk::data_set data_set(argv[1]);
	Assign assign(4, 5, data_set);

	data_set.add_element("box", "boxes");
	data_set.set_size("box", assign.total_nodes());
	data_set.add_attribute<uint32_t>("box", "min_vertex");
	data_set.add_attribute<uint32_t>("box", "max_vertex");
	data_set.add_attribute<uint32_t>("box", "min_face");
	data_set.add_attribute<uint32_t>("box", "max_face");
	data_set.add_attribute<position>("box", "min_bbox");
	data_set.add_attribute<position>("box", "max_bbox");

	data_set.reload();
	assign.process(0);

	return 0;
}
