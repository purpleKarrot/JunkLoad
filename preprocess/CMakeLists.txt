cmake_minimum_required(VERSION 2.8)
project(stream_process)

include(CMakeParseArguments)

add_executable(stream_process
  main.cpp
  attribute.cpp
  data_element.cpp
  data_set_header_io.cpp
  data_set_header.cpp
  data_types.cpp
  exception.cpp
  mapped_data_element.cpp
  mapped_data_set.cpp
  ply_converter.cpp
  plyfile.cpp
  reindex_faces.cpp
  sort_data_set.cpp
  stream_structure.cpp
  )

boost_link_libraries(stream_process STATIC
  iostreams
  thread
  filesystem
  )

target_link_libraries(stream_process
  pthread
  )
