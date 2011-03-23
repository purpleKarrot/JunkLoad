#include <stream_process/exception.hpp>

#include <boost/lexical_cast.hpp>

namespace stream_process
{
// private
exception::exception()
{
}

exception::~exception() throw ()
{
}

exception::exception(const char* desc, const except_here& here) :
	_description(desc), _function("") // FIXME remove
			, _file(here.file), _line(here.line)
{
}

exception::exception(const std::string& desc, const except_here& here) :
	_description(desc), _function("") // FIXME remove
			, _file(here.file), _line(here.line)
{
}

exception::exception(const std::stringstream& desc, const char* function,
		const char* file, size_t line) :
	_description(desc.str()), _function(function), _file(file), _line(line)
{
}

exception::exception(const std::string& desc, const char* function,
		const char* file, size_t line) :
	_description(desc), _function(function), _file(file), _line(line)
{
}

const char* exception::what() const throw ()
{
	return _description.c_str();
}

void exception::get_info(std::string& what_, std::string& where_) const throw ()
{
	what_ = _description;
	where_ = "'";
	where_ += _file + "'";
	try
	{
		where_ += std::string(", line ") + boost::lexical_cast<std::string>(
				_line);
	} catch (...)
	{
	}
}

} // namespace stream_process
