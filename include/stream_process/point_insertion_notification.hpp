#ifndef __STREAM_PROCESS__POINT_INSERTION_NOTIFICATION__HPP__
#define __STREAM_PROCESS__POINT_INSERTION_NOTIFICATION__HPP__

namespace stream_process
{

class point_insertion_notification_target
{
public:
    virtual ~point_insertion_notification_target() {};
    virtual void on_insertion( stream_point* point ) = 0;
    
protected:

}; // class point_insertion_notification

} // namespace stream_process

#endif

