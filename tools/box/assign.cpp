#include "assign.hpp"
#include <iostream>

void box::Assign::process(int index)
{
	std::cout << "processing " << index << std::endl;

	if (is_leaf(index))
	{
		// range of faces
		// range of vertices
		// axis aligned box

		return;
	}

	for (int i = 1; i <= fanout; ++i)
	{
		process(index * fanout + i);

		// range of faces
		// range of vertices
		// axis aligned box
	}
}

int main(int argc, char* argv[])
{
	box::Assign assign(5, 2);

	assign.process(0);

	std::cout << "prossing total " << assign.total_nodes() << std::endl;

	return 0;
}
