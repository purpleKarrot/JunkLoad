
add_executable(junk
  main.cpp
  convert.cpp
  ply/ply_parser.cpp
  fix_scale.cpp
  calc_normals.cpp
  )
target_link_libraries(junk
  junkload
  pthread
  )
boost_link_libraries(junk STATIC
  filesystem
  iostreams
  program_options
  thread
  )

add_executable(sort
  sort/main.cpp
  )
target_link_libraries(sort
  junkload
  pthread
  )
boost_link_libraries(sort STATIC
  filesystem
  iostreams
  thread
  )
