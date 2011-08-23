# Copyright (c) 2011 Daniel Pfeifer <daniel@pfeifer-mail.de>

find_package(Equalizer NO_MODULE)
#if(NOT EQUALIZER_FOUND)
#  message(STATUS "skipping equalizer")
#  return()
#endif()

include_directories(${Equalizer_INCLUDE_DIRS})

add_library(eqMesh STATIC
  mesh/plyfile.cpp
  mesh/ply.h
  mesh/typedefs.h
  mesh/vertexBufferBase.h
  mesh/vertexBufferData.h
  mesh/vertexBufferLeaf.cpp
  mesh/vertexBufferLeaf.h
  mesh/vertexBufferNode.cpp
  mesh/vertexBufferNode.h
  mesh/vertexBufferRoot.cpp
  mesh/vertexBufferRoot.h
  mesh/vertexBufferState.h
  mesh/vertexData.cpp
  mesh/vertexData.h
  )

add_executable(eqJunk
  channel.cpp
  channel.h
  config.cpp
  configEvent.cpp
  configEvent.h
  config.h
  eqPly.cpp
  eqPly.h
  error.cpp
  error.h
  frameData.cpp
  frameData.h
  initData.cpp
  initData.h
  localInitData.cpp
  localInitData.h
  main.cpp
  node.cpp
  node.h
  pipe.cpp
  pipe.h
  view.cpp
  view.h
  window.cpp
  window.h
  )

target_link_libraries(eqJunk
  eqMesh
  ${EQUALIZER_LIBRARIES}
  )

boost_link_libraries(eqJunk
  program_options
  )
