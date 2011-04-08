cmake_minimum_required(VERSION 2.8)
project(stream_process)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake)

include_directories(include)

find_package(Boost COMPONENTS iostreams thread filesystem NO_MODULE)
include(${Boost_USE_FILE})

# loading library
add_subdirectory(junkload)

# preprocessor
add_subdirectory(preprocess)

## viewers
#add_subdirectory(equalizer)
#add_subdirectory(maoni)
