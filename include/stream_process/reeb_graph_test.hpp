#ifndef __STREAM_PROCESS__REEB_GRAPH_TEST__HPP__
#define __STREAM_PROCESS__REEB_GRAPH_TEST__HPP__

#include <stream_process/stream_process_types.hpp>
#include <stream_process/reeb_graph2.hpp>
#include <stream_process/reeb_node2.hpp>
#include <stream_process/reeb_edge2.hpp>

namespace stream_process
{

template< typename sp_types_t >
class reeb_graph_test
{
public:
    STREAM_PROCESS_TYPES

    struct test_point
    {
        vec3        position;
        nbh_type    neighbors;
    };

    int run();

protected: // functions
    void _generate_test_points();

protected: // variables
    std::vector< test_point >       _test_points;


}; // class reeb_graph_test

#define SP_TEMPLATE_TYPES   template< typename sp_types_t >
#define SP_CLASS_NAME       reeb_graph_test< sp_types_t >


SP_TEMPLATE_TYPES
int
SP_CLASS_NAME::run()
{
    _generate_test_points();


    return 0;
}



SP_TEMPLATE_TYPES
void
SP_CLASS_NAME::_generate_test_points()
{

}


#undef SP_TEMPLATE_TYPES
#undef SP_CLASS_NAME

} // namespace stream_process

#endif

