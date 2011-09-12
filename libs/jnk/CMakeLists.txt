
add_library(jnk
  types.cpp
  generator.cpp
  parser.cpp
  data_set.cpp
  )
boost_link_libraries(jnk
  iostreams
  filesystem
  )
