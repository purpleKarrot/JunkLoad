#include "data_set_header_io.hpp"
#include "file_suffix_helper.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

namespace stream_process
{

bool load_header(std::istream& in, header& h);
bool save_header(std::ostream& out, const header& h);

void data_set_header_io::read_from_file(const std::string& filename_base,
		header& hdr_)
{
	_open_file(filename_base, std::fstream::in);

//	_working_copy = hdr_;
	_input_offset = 0;
	_input_face_offset = 0;

	_parse_header(hdr_);
//	hdr_ = _working_copy;

//	_fstream.close();
}

void data_set_header_io::write_to_file(const std::string& filename_base,
		const header& hdr_)
{
	// self test
	{
		save_header(std::cout, hdr_);

		{
			std::ofstream file("temp.junk");
			if (!save_header(file, hdr_))
				std::cout << "\n\nerror saving header\n" << std::endl;
		}

		{
			header h;

			std::ifstream file("temp.junk");
			if (!load_header(file, h))
				std::cout << "\n\nerror loading header\n" << std::endl;

			std::cout << "######################" << std::endl;

			std::cout << h.elements.size() << std::endl;
//			std::cout << h.elements[1].name() << std::endl;

			save_header(std::cerr, h);
		}
	}

	_open_file(filename_base, std::fstream::out | std::fstream::trunc);

	_fstream << "# stream_process data set header\n\n";

	_fstream << "# == " << "globals" << " ==\n\n";

	vec3d aabb = hdr_.min;
	_fstream << "min ";
	for (size_t index = 0; index < 3; ++index)
	{
		_fstream << aabb[index] << " ";
	}
	_fstream << "\n";

	aabb = hdr_.max;
	_fstream << "max ";
	for (size_t index = 0; index < 3; ++index)
	{
		_fstream << aabb[index] << " ";
	}
	_fstream << "\n\n";

	const mat4d& transform_ = hdr_.transform;
	if (transform_ != mat4d::IDENTITY)
	{
		_fstream << "\ntransform ";
		for (size_t row = 0; row < 4; ++row)
		{
			for (size_t col = 0; col < 4; ++col)
			{
				_fstream << transform_(row, col) << " ";
			}
		}
		_fstream << "\n\n";
	}

	header::super::const_iterator it = hdr_.elements.begin(), it_end = hdr_.elements.end();
	for (; it != it_end; ++it)
	{
		const element& elem = *it;
		if (!elem.empty())
		{
			_fstream << "# == " << elem.get_name() << " == \n\n";
			_fstream << elem.to_header_string() << "\n";
		}
	}

	_fstream.close();
}

void data_set_header_io::_open_file(const std::string& filename_base,
		std::fstream::openmode mode_)
{
	_filename = filename_base + file_suffix_helper::get_header_suffix();

	assert(!_fstream.is_open());

	_fstream.open(_filename.c_str(), mode_);
	if (!_fstream.is_open())
	{

		throw std::runtime_error(
				std::string("opening point header ") + _filename + " failed.");
	}
}

void data_set_header_io::_parse_header(header& _working_copy)
{
	std::string line;
	size_t offset;
	std::deque<std::string> tokens;

	while (std::getline(_fstream, line))
	{
		// check for comments and remove them
		offset = line.find_first_of('#');
		line = line.substr(0, offset);
		// remove whitespace
		boost::algorithm::trim(line);
		if (line.empty())
			continue;

		// split string into tokens
		tokens.clear();
		boost::split(tokens, line, boost::algorithm::is_any_of(" "),
				boost::token_compress_on);

		if (!_parse_line(tokens, _working_copy))
		{
			throw std::runtime_error(
					std::string("parsing header ") + _filename + " failed. "
							+ "could not parse line '" + line + "'.");
		}
	}
}

bool data_set_header_io::_parse_line(std::deque<std::string>& tokens, header& _working_copy)
{
	if (tokens.empty() || tokens[0].empty())
		return true;

	try
	{
		if (tokens[0] == "element")
		{
			if (tokens.size() < 2)
				return false;
			// TODO create element if not vertex or face
		}
		if (tokens[0] == "vertex")
		{
			tokens.pop_front();
			_working_copy.vertex().set_from_strings(tokens);
		}

		if (tokens[0] == "face")
		{
			tokens.pop_front();
			_working_copy.face().set_from_strings(tokens);
		}

		if (tokens[0] == "min")
		{
			if (tokens.size() < 4)
				return false;

			vec3d aabb;
			for (size_t index = 0; index < 3; ++index)
			{
				aabb[index] = boost::lexical_cast<double>(tokens[index + 1]);
			}

			_working_copy.min = aabb;
			return true;
		}
		if (tokens[0] == "max")
		{
			if (tokens.size() < 4)
				return false;
			vec3d aabb;
			for (size_t index = 0; index < 3; ++index)
			{
				aabb[index] = boost::lexical_cast<double>(tokens[index + 1]);
			}
			_working_copy.max = aabb;
			return true;
		}
		if (tokens[0] == "transform")
		{
			if (tokens.size() < 17)
				return false;
			mat4d transform_;
			for (size_t row = 0, index = 1; row < 4; ++row)
			{
				for (size_t col = 0; col < 4; ++col, ++index)
				{
					transform_(row, col) = boost::lexical_cast<double>(
							tokens[index]);
				}
			}
			_working_copy.transform = transform_;
			return true;
		}
		if (tokens[0] == "endian")
		{
			if (tokens.size() < 2)
				return false;

			std::string endianess = tokens[1];
			boost::algorithm::to_lower(endianess);

			if (endianess == "big")
				_working_copy.big_endian = true;
			else
				_working_copy.big_endian = false;
			return true;
		}
		// ignore other tokens
		return true;
	} catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return false;
	}

}

} // namespace stream_process
