#include <stream_process/file_suffix_helper.hpp>

namespace stream_process
{

file_suffix_helper* file_suffix_helper::_singleton_instance = 0;

file_suffix_helper&
file_suffix_helper::get_singleton()
{
    if ( ! _singleton_instance )
    {
        _singleton_instance = new file_suffix_helper();
    }
    return *_singleton_instance;
}


file_suffix_helper*
file_suffix_helper::get_singleton_ptr()
{
    if ( ! _singleton_instance )
    {
        _singleton_instance = new file_suffix_helper();
    }
    return _singleton_instance;
}


} // namespace stream_process

