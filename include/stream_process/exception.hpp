#ifndef __STREAM_PROCESS__EXCEPTION__H__
#define __STREAM_PROCESS__EXCEPTION__H__

#include <stdexcept>
#include <string>
#include <sstream>

#define ERROR( desc ) throw( exception( desc, __FUNCTION__, __FILE__, __LINE__ ) )
#define STREAM_EXCEPT( desc ) throw( exception( desc, __FUNCTION__, __FILE_WITHOUT_PATH__, __LINE__ ) )

#define SPROCESS_HERE ( stream_process::except_here( __FILE__, __LINE__ ) )

namespace stream_process
{

struct except_here
{
	except_here(const char* file_, int line_) :
		file(file_), line(line_)
	{
	}
	const char* file;
	int line;
}; // struct except_here

class exception: public std::exception
{
public:
	exception(const char* desc, const except_here& here);
	exception(const std::string& desc, const except_here& here);
	exception(const std::stringstream& desc, const char* function,
			const char* file, size_t line);
	exception(const std::string& desc, const char* function, const char* file,
			size_t line);
	virtual ~exception() throw ();
	virtual const char* what() const throw ();
	virtual void
			get_info(std::string& what_, std::string& where_) const throw ();

protected:
	std::string _description;
	const char* _function;
	std::string _file;
	size_t _line;

private:
	exception();

}; // class exception


} // namespace stream_process

#endif
