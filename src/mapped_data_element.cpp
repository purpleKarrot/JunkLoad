#include <stream_process/mapped_data_element.hpp>

namespace stream_process
{

mapped_data_element::mapped_data_element(data_element& element_) :
	_element(element_), _mapped_file()
{

}

mapped_data_element::mapped_data_element(data_element& element_,
		const std::string& filename_base, bool create_new_file) :
	_element(element_), _mapped_file()
{
	open(filename_base, create_new_file);
}

void mapped_data_element::open(const std::string& filename_base,
		bool create_new_file)
{
	mapped_file_params params_;
	params_.path = _element.get_filename(filename_base);
	params_.mode = std::ios_base::in | std::ios_base::out;

	if (create_new_file)
	{
		params_.new_file_size = _element.get_file_size_in_bytes();
		assert(params_.new_file_size);
	}

	try
	{
		_mapped_file.open(params_);
	} catch (std::exception& e)
	{
		_mapped_file.close();
	}

	if (!_mapped_file.is_open())
	{
		throw exception(std::string("opening file '") + params_.path
				+ "' failed.", SPROCESS_HERE);
	}

	if (create_new_file)
	{
		std::cout << "created file '" << params_.path << "' with a size of "
				<< (double) params_.new_file_size / 1073741824.0
				<< " gigabytes." << std::endl;
	}
}

const data_element&
mapped_data_element::get_element() const
{
	return _element;
}

char*
mapped_data_element::data()
{
	return _mapped_file.data() + _element.get_offset();
}

const char*
mapped_data_element::data() const
{
	return _mapped_file.data() + _element.get_offset();
}

} // namespace stream_process

