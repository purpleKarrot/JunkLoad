#ifndef __STREAM_PROCESS__POINT_RELEASE_NOTIFICATION__HPP__
#define __STREAM_PROCESS__POINT_RELEASE_NOTIFICATION__HPP__

namespace stream_process
{

class point_release_notification_target
{
public:
    virtual ~point_release_notification_target() {};
	virtual void on_release( stream_point* point ) = 0;

}; // class point_release_notification

} // namespace stream_process

#endif

