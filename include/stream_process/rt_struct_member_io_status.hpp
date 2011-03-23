#ifndef __STREAM_PROCESS__RT_STRUCT_MEMBER_IO_STATUS__HPP__
#define __STREAM_PROCESS__RT_STRUCT_MEMBER_IO_STATUS__HPP__

namespace stream_process
{

typedef size_t IO_STATUS;
static const size_t IO_WRITE_TO_STREAM  = 0;
static const size_t IO_READ_FROM_INPUT  = 1;
static const size_t IO_WRITE_TO_OUTPUT  = 2;
static const size_t IO_READ_WRITE       = 3;

} // namespace stream_process

#endif

