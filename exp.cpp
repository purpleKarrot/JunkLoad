//

#include <cmath>
#include <iostream>

int main(int argc, char* argv[])
{
	int exp;
	double sig;

	for (double x = -1.0; x <= 1.0; x += 0.1)
	{
		sig = std::frexp(x, &exp);

		std::cout << sig << '\t' << exp << std::endl;
	}

	return 0;
}
