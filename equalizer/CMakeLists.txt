# Copyright (c) 2011 Daniel Pfeifer <daniel@pfeifer-mail.de>

#find_package(Equalizer NO_MODULE)
#include("${Equalizer_USE_FILE}")

include(StringifyShaders)

include_directories(include ${CMAKE_CURRENT_BINARY_DIR})

stringify_shaders(SHADERS
  vertexShader.glsl
  fragmentShader.glsl
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
  plyfile.cpp
  ply.h
  typedefs.h
  vertexBufferBase.cpp
  vertexBufferBase.h
  vertexBufferData.h
  vertexBufferLeaf.cpp
  vertexBufferLeaf.h
  vertexBufferNode.cpp
  vertexBufferNode.h
  vertexBufferRoot.cpp
  vertexBufferRoot.h
  vertexBufferState.h
  vertexData.cpp
  vertexData.h
  vertexBufferDist.cpp
  vertexBufferDist.h
  view.cpp
  view.h
  window.cpp
  window.h
  )

target_link_libraries(eqJunk
  Collage
  Equalizer
  EqualizerAdmin
# ${EQUALIZER_LIBRARIES}
  )
