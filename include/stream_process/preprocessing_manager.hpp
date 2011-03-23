#ifndef __STREAM_PROCESS__PREPROCESSING_MANAGER__HPP__
#define __STREAM_PROCESS__PREPROCESSING_MANAGER__HPP__

#include "mapped_point_data.hpp"
#include "preprocess_shared_data.hpp"
#include "options.h"
#include "sorting_controller.hpp"

namespace stream_process
{

class optimal_transform;
class preprocessing_manager
{
public:
    preprocessing_manager();
    ~preprocessing_manager();
    
    void setup(
        const std::string& in_file,
        const std::string& sorted_file,
        bool apply_ot = true
        );
    // do preprocessing stuff and synch preprocess_shared_data max index
    void process_multi_threaded();
    // do preprocessing stuff without synchronization
    void process_single_threaded();

    inline void operator()();

    preprocess_shared_data& get_preprocess_shared_data();

    void set_mapped_point_data( mapped_point_data* mapped_point_data_ );
    
    
protected:
    mapped_point_data*          _mapped_point_data;
    preprocess_shared_data      _preprocess_shared_data;
    optimal_transform*          _optimal_transform;
    sorting_controller*         _sorting_controller;

}; // class preprocessing_manager


void
preprocessing_manager::operator()()
{
    process_multi_threaded();
}


} // namespace stream_process

#endif

