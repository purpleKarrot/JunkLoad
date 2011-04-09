#include "data_set_header_io.hpp"
#include "file_suffix_helper.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

namespace stream_process
{

void data_set_header_io::read_from_file(const std::string& filename_base,
		data_set_header& hdr_)
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
		const data_set_header& hdr_)
{
	_open_file(filename_base, std::fstream::out | std::fstream::trunc);

	_fstream << "# stream_process data set header\n\n";

	_fstream << "# == " << "globals" << " ==\n\n";

	vec3d aabb = hdr_.get_aabb_min<double> ();
	_fstream << "min ";
	for (size_t index = 0; index < 3; ++index)
	{
		_fstream << boost::lexical_cast<std::string, double>(aabb[index])
				<< " ";
	}
	_fstream << "\n";

	aabb = hdr_.get_aabb_max<double> ();
	_fstream << "max ";
	for (size_t index = 0; index < 3; ++index)
	{
		_fstream << boost::lexical_cast<std::string, double>(aabb[index])
				<< " ";
	}
	_fstream << "\n\n";

	const mat4d& transform_ = hdr_.get_transform<double> ();
	if (transform_ != mat4d::IDENTITY)
	{
		_fstream << "\ntransform ";
		for (size_t row = 0; row < 4; ++row)
		{
			for (size_t col = 0; col < 4; ++col)
			{
				_fstream << boost::lexical_cast<std::string, double>(
						transform_(row, col)) << " ";
			}
		}
		_fstream << "\n\n";
	}

	data_set_header::const_iterator it = hdr_.begin(), it_end = hdr_.end();
	for (; it != it_end; ++it)
	{
		const data_element& elem = **it;
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

void data_set_header_io::_parse_header(data_set_header& _working_copy)
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

bool data_set_header_io::_parse_line(std::deque<std::string>& tokens, data_set_header& _working_copy)
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
			_working_copy.get_vertex_element().set_from_strings(tokens);
		}

		if (tokens[0] == "face")
		{
			tokens.pop_front();
			_working_copy.get_face_element().set_from_strings(tokens);
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

			_working_copy.set_aabb_min(aabb);
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
			_working_copy.set_aabb_max(aabb);
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
			_working_copy.set_transform(transform_);
			return true;
		}
		if (tokens[0] == "endian")
		{
			if (tokens.size() < 2)
				return false;

			std::string endianess = tokens[1];
			boost::algorithm::to_lower(endianess);

			if (endianess == "big")
				_working_copy.set_data_is_big_endian(true);
			else
				_working_copy.set_data_is_big_endian(false);
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
