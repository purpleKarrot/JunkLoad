//

#include <deque>
#include <string>

#include <stream_process/default_types.hpp>
#include <stream_process/stream_manager.hpp>
#include <stream_process/op_database.hpp>
#include <stream_process/options_map.hpp>

int main(int argc, char* argv_[])
{
	const char** argv = const_cast<const char**> (argv_);

	std::deque < std::string > modes;
	modes.push_back("process");

	stream_process::options_map omap;
	size_t mode = omap.parse(modes, argc, argv);

	std::cout << "stream processor 0.8 (c) jonas boesch, renato pajarola\n";
	std::cout << "architecture: " << sizeof(void*) * 8 << "bit, precision: mixed.\n";
	std::cout << std::endl;

	stream_process::op_database odb;
	stream_process::stream_manager<stream_process::sp_mixed_precision_type> sm;
	odb.insert_ops(sm.get_op_manager());
	sm.setup2(argc, argv, false);
	return sm.process2();
}
