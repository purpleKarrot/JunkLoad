//

#include <string>
#include <vector>
#include <iostream>

#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <junk/data_set.hpp>

void convert(const std::vector<std::string>& input, junk::data_set& junk, bool, bool);
void setup_header(junk::data_set& data_set, bool normal, bool color);
void fix_scale(junk::data_set& junk);
void calc_normals(junk::data_set& junk);

int main(const int argc, char* argv[])
{
	std::vector<std::string> input;
	std::string output;

	po::options_description desc("Allowed options");
	desc.add_options()
		("help",          "produce this help message")
		("input,i",       po::value(&input),  "input ply files")
		("output,o",      po::value(&output), "output junk file")
		("use-color,c",   "extract colors")
		("use-normal,n",  "extract normals")
		("auto-normal,a", "calculate normals")
		("fix-scale,f",   "fix scale and offset")
		;

	po::positional_options_description p;
	p.add("input", -1);

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
	{
		std::cout << "An output file is required!" << std::endl;
		return -1;
	}

	bool use_color = vm.count("use-color");
	bool use_normal = vm.count("use-normal");
	bool auto_normal = vm.count("auto-normal");

	std::vector<std::string> ply_files;
	BOOST_FOREACH(const std::string& in, input)
	{
		boost::filesystem::path path(in);
		if(path.extension() == ".ply")
		{
			ply_files.push_back(in);
			continue;
		}

		typedef boost::filesystem::recursive_directory_iterator rec_iterator;
		rec_iterator start(in), end;
		for (rec_iterator it = start; it != end; ++it)
		{
			if(it->path().extension() == ".ply")
				ply_files.push_back(it->path().string());
		}
	}

	junk::data_set junk(output, true);

	setup_header(junk, use_normal || auto_normal, use_color);

	convert(ply_files, junk, use_normal, use_color);

	if (vm.count("fix-scale"))
		fix_scale(junk);

	if (auto_normal)
		calc_normals(junk);

	junk.safe_header();
	return 0;
}
