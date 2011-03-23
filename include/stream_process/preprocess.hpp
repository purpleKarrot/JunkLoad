#ifndef __STREAM_PROCESS__PREPROCESS__HPP__
#define __STREAM_PROCESS__PREPROCESS__HPP__

#include <string>
#include <cstddef>

namespace stream_process
{

class preprocessor
{
public:
    struct params
    {
        params() : do_optimal_transform( true ), number_of_threads( 8 ) {}
        std::string     source_file; // full name if not stream_process data set
        std::string     result_file; // base_name
        bool            do_optimal_transform;
        size_t          number_of_threads;
    };

    preprocessor( const params& params_ );
    

protected:
    void    _apply_optimal_transform( const std::string& filename, 
        bool full_optimal_transform ); // full transform or just axis swawp

    params  _params;

}; // class preprocessor

} // namespace stream_process

#endif

