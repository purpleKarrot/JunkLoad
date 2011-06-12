cmake_minimum_required(VERSION 2.8)
project(stream_process)

include(CMakeParseArguments)

#

add_executable(optimize
  optimize/main.cpp
  )

target_link_libraries(optimize
  junkload
  )

boost_link_libraries(optimize STATIC
  iostreams
  filesystem
  )

#

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
