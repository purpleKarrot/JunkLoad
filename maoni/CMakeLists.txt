
#find_package(Maoni NO_MODULE)
#include("${Maoni_USE_FILE}")
set(Maoni_LIBRARIES Maoni MaoniMain)

add_executable(massive_maoni
  main.cpp
  )

target_link_libraries(massive_maoni
  ${Maoni_LIBRARIES}
  )
