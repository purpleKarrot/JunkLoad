
# convert PLY to JNK
add_executable(jnk_convert
  convert/main.cpp
  convert/convert.cpp
  convert/ply/ply_parser.cpp
  convert/fix_scale.cpp
  convert/calc_normals.cpp
  )
target_link_libraries(jnk_convert
  jnk
  pthread
  )
boost_link_libraries(jnk_convert
  program_options
  thread
  )

# sort JNK file according to z-index
add_executable(sort
  sort/main.cpp
  )
target_link_libraries(sort
  jnk
  pthread
  )
boost_link_libraries(sort
  thread
  )

# augment a sorted JNK file with a bbox-stream
add_executable(box
  box/main.cpp
  box/assign.cpp
  )
target_link_libraries(box
  jnk
  )
