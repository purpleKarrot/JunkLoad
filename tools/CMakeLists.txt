
add_executable(fix_scale
  fix_scale.cpp
  )

target_link_libraries(fix_scale
  junkload
  )

boost_link_libraries(fix_scale STATIC
  iostreams
  )
