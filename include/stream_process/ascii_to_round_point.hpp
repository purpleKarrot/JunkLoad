#ifndef __STREAM_PROCESS__ASCII_TO_ROUND_POINT__HPP__
#define __STREAM_PROCESS__ASCII_TO_ROUND_POINT__HPP__

#include "ascii_to_point.hpp"

namespace stream_process
{

class ascii_to_round_point : public ascii_to_point
{
public:
    virtual void setup_point_info( point_info& point_info_ );
    virtual char* write_point( char* point_begin,
        const std::vector< std::string >& tokens );

protected:

}; // class ascii_to_round_point

} // namespace stream_process

#endif

