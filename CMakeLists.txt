cmake_minimum_required(VERSION 2.8)
project(stream_process C CXX Fortran)

find_package(LAPACK REQUIRED)

find_package(Boost COMPONENTS iostreams thread filesystem NO_MODULE)
include(${Boost_USE_FILE})

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(stream_process
  main.cpp
  src/exception.cpp
  src/attribute.cpp
  src/data_types.cpp
  src/stream_structure.cpp
  src/option.cpp
  src/options_base.cpp
  src/options_map.cpp
  src/options_parser.cpp
  src/stream_config.cpp
  src/stream_point_configurator.cpp
  src/data_set_header.cpp
  src/data_element.cpp
  src/data_set_header_io.cpp
  src/mapped_data_set.cpp
  src/mapped_data_element.cpp
  src/mode_timer.cpp
  src/sort_data_set.cpp
  src/preprocess.cpp
  src/ply_converter.cpp
  src/reindex_faces.cpp
  src/plyfile.cpp
  src/stream_options.cpp
  )

include(CMakeParseArguments)
boost_link_libraries(stream_process STATIC
  iostreams
  thread
  filesystem
  )

target_link_libraries(stream_process
  pthread
  ${LAPACK_LIBRARIES}
  )
