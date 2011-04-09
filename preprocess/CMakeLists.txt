cmake_minimum_required(VERSION 2.8)
project(stream_process)

include(CMakeParseArguments)

add_executable(stream_process
  main.cpp
  data_set_header_io.cpp
  data_set_header.cpp
  mapped_data_element.cpp
  mapped_data_set.cpp
  reindex_faces.cpp
  sort_data_set.cpp

  # TODO: make this a library
  ply/convert.cpp
  ply/ply_io.c
  )

boost_link_libraries(stream_process STATIC
  iostreams
  thread
  filesystem
  )

target_link_libraries(stream_process
  pthread
  junkload
  )
