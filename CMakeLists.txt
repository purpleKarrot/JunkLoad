cmake_minimum_required(VERSION 2.8)
project(JunkLoad)

include(CMakeParseArguments)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

include_directories(include)

find_package(Boost
  COMPONENTS
    iostreams
    thread
    filesystem
    program_options
  NO_MODULE
  )
include(${Boost_USE_FILE})

# loading library
add_subdirectory(junkload)

# preprocessing tools
add_subdirectory(tools)

# data
add_subdirectory(data)

# viewers
add_subdirectory(equalizer)
add_subdirectory(maoni)
