#ifndef __STREAM_PROCESS__VISITOR__H__
#define __STREAM_PROCESS__VISITOR__H__

namespace stream_process
{
template< typename node_t >
class visitor
{
public:
    virtual inline void visit( node_t* node ) = 0;

}; // class visitor


} // namespace stream_process

#endif
