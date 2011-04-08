# Copyright (c) 2011 Daniel Pfeifer <daniel@pfeifer-mail.de>

find_package(Equalizer NO_MODULE)
include("${Equalizer_USE_FILE}")

include(StringifyShaders)

if(MSVC)
  add_definitions(/wd4996)
endif(MSVC)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

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
  pathAround.txt
  pathExample.txt
  pipe.cpp
  pipe.h
  plyfile.cpp
  ply.h
  tracker.cpp
  tracker.h
  typedefs.h
  vertexBufferBase.cpp
  vertexBufferBase.h
  vertexBufferData.h
  vertexBufferDist.cpp
  vertexBufferDist.h
  vertexBufferLeaf.cpp
  vertexBufferLeaf.h
  vertexBufferNode.cpp
  vertexBufferNode.h
  vertexBufferRoot.cpp
  vertexBufferRoot.h
  vertexBufferState.h
  vertexData.cpp
  vertexData.h
  vertexShader.glsl
  view.cpp
  view.h
  window.cpp
  window.h
  )

target_link_libraries(eqJunk
  ${EQUALIZER_LIBRARIES}
  )
