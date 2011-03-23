#ifndef __STREAM_PROCESS__WORKER_THREAD__HPP__
#define __STREAM_PROCESS__WORKER_THREAD__HPP__

namespace stream_process
{

template< class controller_t >
class worker_thread
{
public:
    static void set_controller( controller_t* controller_ );
    static controller_t* get_controller_ptr();
protected:
    static controller_t*   _controller;

}; // class worker_thread


template< typename controller_t >
controller_t* worker_thread< controller_t >::_controller = 0;


template< typename controller_t >
void
worker_thread< controller_t >::set_controller( controller_t* controller_ )
{
    _controller = controller_;
}



template< typename controller_t >
controller_t*
worker_thread< controller_t >::get_controller_ptr()
{
    return _controller;
}





} // namespace stream_process

#endif

