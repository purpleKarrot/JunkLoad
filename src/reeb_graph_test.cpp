#include <stream_process/reeb_graph_test.hpp>

#include <stream_process/default_types.hpp>
#include <iostream>

int
main( int argc, const char* argv[] )
{
    using namespace stream_process;

    std::cout << "reeb graph test" << std::endl;

    reeb_graph_test< sp_mixed_precision_type >  test;
    return test.run();
}

