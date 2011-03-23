//#include <stream_process/stream_processor.hpp>
//
//#include <deque>
//#include <string>
//
//#include <stream_process/default_types.hpp>
//#include <stream_process/stream_manager.hpp>
//#include <stream_process/op_database.hpp>
//#include <stream_process/options_map.hpp>
//
//namespace stream_process
//{
//
//int stream_processor::run(int argc, const char* argv[])
//{
//	std::deque < std::string > modes;
//	modes.push_back("process");
//
//	options_map omap;
//	size_t mode = omap.parse(modes, argc, argv);
//
//	std::cout << "stream processor 0.8 (c) jonas boesch, renato pajarola\n";
//	std::cout << "architecture: " << sizeof(void*) * 8 << "bit, precision: mixed.\n";
//	std::cout << std::endl;
//
//	op_database odb;
//	stream_manager<sp_mixed_precision_type> sm;
//	odb.insert_ops(sm.get_op_manager());
//	sm.setup2(argc, argv, false);
//	return sm.process2();
//}
//
//int stream_processor::_process(int argc, const char* argv[])
//{
//}
//
//} // namespace stream_process
