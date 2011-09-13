
find_package(Maoni NO_MODULE)
#if(NOT MAONI_FOUND)
#  message(STATUS "skipping maoni")
#  return()
#endif()

include_directories(${Maoni_INCLUDE_DIRS})

add_executable(massive_maoni
  BoxModel.cpp
  BoxNode.cpp
  main.cpp
  Model.cpp
  Shader.cpp
  )

target_link_libraries(massive_maoni
  ${MAONI_LIBRARIES}
  jnk
  )
