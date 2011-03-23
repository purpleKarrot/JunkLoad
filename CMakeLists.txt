cmake_minimum_required(VERSION 2.8)
project(stream_process)

find_package(Boost COMPONENTS iostreams thread filesystem NO_MODULE)
include(${Boost_USE_FILE})

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(stream_process
  main.cpp
  src/attribute.cpp
  src/data_element.cpp
  src/data_set_header_io.cpp
  src/data_set_header.cpp
  src/data_types.cpp
  src/exception.cpp
  src/mapped_data_element.cpp
  src/mapped_data_set.cpp
  src/ply_converter.cpp
  src/plyfile.cpp
  src/preprocess.cpp
  src/reindex_faces.cpp
  src/sort_data_set.cpp
  src/stream_structure.cpp
  )

include(CMakeParseArguments)
boost_link_libraries(stream_process STATIC
  iostreams
  thread
  filesystem
  )

target_link_libraries(stream_process
  pthread
  )

return()

set(depend "${CMAKE_BINARY_DIR}/CMakeFiles/stream_process.dir/depend.internal")
file(STRINGS "${depend}" files REGEX "^ ../include/")
string(REPLACE "../include/" "" files "${files}")
string(REPLACE " " ";" files "${files}")

set(needed)
foreach(file ${files})
  list(APPEND needed ${file})
endforeach()

list(SORT needed)
list(REMOVE_DUPLICATES needed)

file(GLOB available RELATIVE "${CMAKE_SOURCE_DIR}/include"
  "${CMAKE_SOURCE_DIR}/include/stream_process/*.hpp"
  )

list(SORT available)
list(REMOVE_ITEM available ${needed})

foreach(file ${available})
  message(STATUS "${file}")
# execute_process(COMMAND git rm ${CMAKE_SOURCE_DIR}/include/${file})
endforeach()
