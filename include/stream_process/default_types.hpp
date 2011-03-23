#ifndef __STREAM_PROCESS__DEFAULT_TYPES__HPP__
#define __STREAM_PROCESS__DEFAULT_TYPES__HPP__

#include <stream_process/stream_process_types.hpp>
#include <stream_process/stream_data.hpp>
#include <stream_process/bounds.hpp>
#include <stream_process/data_slice.hpp>

namespace stream_process
{

typedef std::list< stream_data* >   stream_container_type;

typedef multi_slice< bounds_1d< float >, stream_container_type >    s_slice_type;
typedef stream_process_types< float, float, s_slice_type >          sp_single_precision_type;

typedef multi_slice< bounds_1d< float >, stream_container_type >    mixed_slice_type;
typedef stream_process_types< float, double, mixed_slice_type >     sp_mixed_precision_type;

typedef multi_slice< bounds_1d< double >, stream_container_type >   d_slice_type;
typedef stream_process_types< double, double, d_slice_type >        sp_double_precision_type;

} // namespace stream_process

#endif

