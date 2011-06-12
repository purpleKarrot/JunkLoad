
#find_package(Maoni NO_MODULE)
#include("${Maoni_USE_FILE}")
set(Maoni_LIBRARIES Maoni MaoniMain)

add_executable(massive_maoni
  main.cpp
  Model.cpp
  ModelPLY.cpp
  ply_io.c
  )

target_link_libraries(massive_maoni
  ${Maoni_LIBRARIES}
  junkload
  )

boost_link_libraries(massive_maoni STATIC
  iostreams
  )
