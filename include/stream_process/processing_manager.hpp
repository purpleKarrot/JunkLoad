#ifndef __STREAM_PROCESS__PROCESSING_MANAGER__HPP__
#define __STREAM_PROCESS__PROCESSING_MANAGER__HPP__

#include "mapped_point_data.hpp"
#include "stream_processing_options.hpp"
#include <boost/thread.hpp>

namespace stream_process
{

/**
*
*   this class is responsible for managing the parallel execution of 
*   preprocessing and the actual stream processing.
*   
*/

class preprocessing_manager;
class chain_manager;
class processing_manager
{
public:
    processing_manager( int argc, const char* argv[] );
    ~processing_manager();
    
    void run();
    
protected:
    void _setup_stage_0();
    void _setup_stage_1();
    
    void _run();
    void _run_single_threaded();
    void _run_preprocessing_only();
    
    void _setup_internals();
    
    bool _does_require_preprocessing();
    
    chain_manager*          _chain_manager;
    preprocessing_manager*  _preprocessing_manager;
    mapped_point_data*      _mapped_point_data;
    
    stream_processing_options   _options;

    std::list< std::string > _preprocessing_suffixes;
    
    bool                    _do_preprocessing;
    bool                    _do_streamprocessing;

}; // class processing_manager

} // namespace stream_process

#endif

