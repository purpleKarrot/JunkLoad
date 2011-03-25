#ifndef __STREAM_PROCESS__MAPPED_DATA_ELEMENT__HPP__
#define __STREAM_PROCESS__MAPPED_DATA_ELEMENT__HPP__

#include <stream_process/data_element.hpp>

#include <stream_process/stream_data_iterators.hpp>

#include <boost/iostreams/device/mapped_file.hpp>

namespace stream_process
{

class mapped_data_element
{
public:
	typedef boost::iostreams::mapped_file mapped_file;
	typedef boost::iostreams::mapped_file_params mapped_file_params;

	typedef stream_data_iterator iterator;
	typedef stream_data_const_iterator const_iterator;

	mapped_data_element(data_element& element_);
	mapped_data_element(data_element& element_,
			const std::string& filename_base, bool create_new_file = false);

	~mapped_data_element()
	{
	}

	inline iterator begin();
	inline iterator end();

	inline const_iterator begin() const;
	inline const_iterator end() const;

	inline size_t size() const;

	// note: using iterators for sequential access is faster and preferred to
	// using operator[]
	inline stream_data* operator[](size_t index);
	inline const stream_data* operator[](size_t index) const;

	void open(const std::string& filename_base, bool create_new_file = false);
	inline void close();
	inline bool is_open() const;

	const data_element& get_element() const;

	char* data();
	const char* data() const;

protected:
	void _update();

	boost::iostreams::mapped_file _mapped_file;
	data_element& _element;

}; // class mapped_data_element


inline stream_data*
mapped_data_element::operator[](size_t index)
{
	assert(index < _element.get_size());
	assert(_mapped_file.data());
	return reinterpret_cast<stream_data*> (data() + index
			* _element.get_size_in_bytes());
}

inline const stream_data*
mapped_data_element::operator[](size_t index) const
{
	assert(index < _element.get_size());
	assert(_mapped_file.data());
	return reinterpret_cast<const stream_data*> (data() + index
			* _element.get_size_in_bytes());
}

inline mapped_data_element::iterator mapped_data_element::begin()
{
	assert(_mapped_file.is_open());
	assert(_mapped_file.data());
	return iterator(data(), _element.get_size_in_bytes());
}

inline mapped_data_element::iterator mapped_data_element::end()
{
	assert(_mapped_file.is_open());
	assert(_mapped_file.data());
	return iterator(data() + _element.get_file_size_in_bytes(),
			_element.get_size_in_bytes());
}

inline mapped_data_element::const_iterator mapped_data_element::begin() const
{
	assert(_mapped_file.is_open() == true);
	assert(_mapped_file.data() != 0);
	return const_iterator(data(), _element.get_size_in_bytes());
}

inline mapped_data_element::const_iterator mapped_data_element::end() const
{
	assert(_mapped_file.is_open());
	assert(_mapped_file.data());
	return const_iterator(data() + _element.get_file_size_in_bytes(),
			_element.get_size_in_bytes());
}

inline void mapped_data_element::close()
{
	_mapped_file.close();
}

inline bool mapped_data_element::is_open() const
{
	return _mapped_file.is_open();
}

inline size_t mapped_data_element::size() const
{
	return _element.get_size();
}

} // namespace stream_process

#endif

