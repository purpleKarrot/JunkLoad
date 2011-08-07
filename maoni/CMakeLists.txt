
find_package(Maoni NO_MODULE)
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
