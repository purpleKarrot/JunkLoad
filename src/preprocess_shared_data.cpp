#include "preprocess_shared_data.hpp"

namespace stream_process
{

preprocess_shared_data::preprocess_shared_data()
    : _min_unsorted_index( 0 )
    , _index_at_last_notify( 0 )
{}



size_t  
preprocess_shared_data::get_min_unsorted_index()
{
    boost::mutex::scoped_lock lock( _mutex );
    return _min_unsorted_index;
}



void
preprocess_shared_data::set_min_unsorted_index( size_t min_unsorted_index_ )
{
    boost::mutex::scoped_lock lock( _mutex );
    _min_unsorted_index = min_unsorted_index_;
    /*
    if ( _index_at_last_notify < _min_unsorted_index )
    {
        _index_at_last_notify = _min_unsorted_index;
        _condition.notify_one();
    }
    */
}



void
preprocess_shared_data::set_transformation( const vec3f& translation, 
    const mat3f& orientation )
{
    #ifdef SPROCESS_OLD_VMMLIB
    _transform.set3x3SubMatrix( orientation );
    #else
    _transform.setSubMatrix( orientation );
    #endif

    _transform.setTranslation( translation );

/*    std::cout << "pp transform\n" 
        << _transform << "\n"
        << orientation << "\n"
        << translation << "\n"
        << std::endl;
*/
}


const mat4f&
preprocess_shared_data::get_transformation() const
{
    return _transform;
}



void
preprocess_shared_data::set_transformation( const mat4f& transform )
{
    _transform = transform;
}


} // namespace stream_process

