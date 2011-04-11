cmake_minimum_required(VERSION 2.8)
project(stream_process)

include(CMakeParseArguments)

add_executable(ply_convert
  ply/main.cpp
  ply/ply_io.c
  )

target_link_libraries(ply_convert
  junkload
  )

boost_link_libraries(ply_convert STATIC
  iostreams
  filesystem
  )

add_executable(stream_process
  main.cpp
  reindex_faces.cpp
  sort_data_set.cpp
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
