#ifndef __STREAM_PROCESS__PREPROCESS_SHARED_DATA__HPP__
#define __STREAM_PROCESS__PREPROCESS_SHARED_DATA__HPP__

#include "VMMLibIncludes.h"

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

namespace stream_process
{

class preprocess_shared_data
{
public:
    preprocess_shared_data();
    
    size_t  get_min_unsorted_index();
    void    set_min_unsorted_index( size_t min_unsorted_index_ );
    
    void set_transformation( const vec3f& translation, 
        const mat3f& orientation );
    
    const mat4f& get_transformation() const;
    void set_transformation( const mat4f& transform );
    
    
protected:
    // for process synchronization
    size_t              _min_unsorted_index;
    size_t              _index_at_last_notify;
    boost::mutex        _mutex;
    boost::mutex        _signalling_mutex;
    
    // transform data from pre-preprocess optimal_transform
    mat4f               _transform;

}; // class preprocess_progress_data

} // namespace stream_process

#endif

