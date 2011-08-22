
find_package(Maoni QUIET NO_MODULE)
if(NOT MAONI_FOUND)
  message(STATUS "skipping maoni")
  return()
endif()

include_directories(${Maoni_INCLUDE_DIRS})

add_executable(massive_maoni
  main.cpp
  Model.cpp
  Shader.cpp
  )

target_link_libraries(massive_maoni
  ${MAONI_LIBRARIES}
  junkload
  )

boost_link_libraries(massive_maoni STATIC
  iostreams
  )
