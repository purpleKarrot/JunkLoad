//

#include <string>
#include <vector>
#include <iostream>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <junk/mapped_data_set.hpp>

void convert(const std::string& ply_file, junk::mapped_data_set& junk, bool, bool);
void setup_header(junk::header& header, bool normal, bool color);
void fix_scale(junk::mapped_data_set& junk);
void calc_normals(junk::mapped_data_set& junk);

int main(const int argc, char* argv[])
{
	std::string input;
	std::string output;

	po::options_description desc("Allowed options");
	desc.add_options()
		("help",          "produce this help message")
		("input,i",       po::value(&input),  "input ply file")
		("output,o",      po::value(&output), "output junk file")
		("use-color,c",   "extract colors")
		("use-normal,n",  "extract normals")
		("auto-normal,a", "calculate normals")
		("fix-scale,f",   "fix scale and offset")
		;

	po::positional_options_description p;
	p.add("input-file", -1);

	po::variables_map vm;
	try
	{
		store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
		notify(vm);
	}
	catch (std::exception&e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		std::cout << desc << std::endl;
		return -1;
	}

	if (vm.count("help"))
	{
		std::cout << desc << std::endl;
		return 0;
	}

	if (input.empty())
	{
		std::cout << "An input file is required!" << std::endl;
		return -1;
	}

	if (output.empty())
		output = input + ".junk";

	bool use_color = vm.count("use-color");
	bool use_normal = vm.count("use-normal");
	bool auto_normal = vm.count("auto-normal");

//	bool fix_scale = vm.count("fix-scale");
//	std::cout << auto_normal << "  " << fix_scale << std::endl;

	junk::mapped_data_set junk(output, true);
	junk::header& header = junk.get_header();

	setup_header(header, use_normal || auto_normal, use_color);

	convert(input, junk, use_normal, use_color);

	if (vm.count("fix-scale"))
		fix_scale(junk);

	if (auto_normal)
		calc_normals(junk);

	junk::save_header(output, header);
	return 0;
}
