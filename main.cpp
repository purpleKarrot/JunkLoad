//

#include <iostream>
#include <boost/thread.hpp>
#include <stream_process/preprocess.hpp>

int main(int argc, char* argv[])
{
	std::cout << "stream processor 0.8 (c) jonas boesch, renato pajarola\n";
	std::cout << "architecture: " << sizeof(void*) * 8 << "bit, precision: mixed.\n";
	std::cout << std::endl;

	stream_process::preprocessor::params ppp;
	ppp.source_file = argc > 1 ? argv[1] : "bunny.ply";
	ppp.result_file = "bunny.stream";
	ppp.do_optimal_transform = true;

	ppp.number_of_threads = boost::thread::hardware_concurrency();
	if (ppp.number_of_threads < 1)
		ppp.number_of_threads = 1;

	stream_process::preprocessor pp(ppp);
}
