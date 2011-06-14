# Copyright (c) 2011 Daniel Pfeifer <daniel@pfeifer-mail.de>

#find_package(Equalizer NO_MODULE)
#include("${Equalizer_USE_FILE}")

include(StringifyShaders)

include_directories(include ${CMAKE_CURRENT_BINARY_DIR})

stringify_shaders(SHADERS
  vertexShader.glsl
  fragmentShader.glsl
  )

add_library(eqMesh STATIC
  mesh/plyfile.cpp
  mesh/ply.h
  mesh/typedefs.h
  mesh/vertexBufferBase.cpp
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
  mesh/vertexBufferDist.cpp
  mesh/vertexBufferDist.h
  )

add_executable(eqJunk
  ${SHADERS}
  cameraAnimation.cpp
  cameraAnimation.h
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
  modelAssigner.h
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
  Collage
  Equalizer
  EqualizerAdmin
# ${EQUALIZER_LIBRARIES}
  )
