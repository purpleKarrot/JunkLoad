#include <junk/mapped_data_element.hpp>

namespace junk
{

void mapped_data_element::open(const junk::element& element,
		const std::string& filename, bool create)
{
	boost::iostreams::mapped_file_params params;
	params.path = filename + '.' + element.name_pl;
	params.mode = std::ios_base::in | std::ios_base::out;

	if (create)
	{
		params.new_file_size = file_size_in_bytes(element);
		assert(params.new_file_size);
	}

	try
	{
		mapped_file.open(params);
	}
	catch (std::exception& e)
	{
		mapped_file.close();
	}

	if (!mapped_file.is_open())
	{
		throw std::runtime_error(
				std::string("opening file '") + params.path + "' failed.");
	}

	if (create)
	{
		std::cout << "created file '" << params.path << "' with a size of "
				<< (double) params.new_file_size / 1073741824.0
				<< " gigabytes." << std::endl;
	}

	num_elements = element.size;
	element_size = size_in_bytes(element);
	this->element = &element;
}

} // namespace junk
