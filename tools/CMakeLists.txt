
function(add_tool name)
  add_executable(${name}
    ${ARGN}
    )
  target_link_libraries(${name}
    junkload
    )
  boost_link_libraries(${name} STATIC
    iostreams
    )
endfunction(add_tool)


add_tool(read_header
  read_header.cpp
  )

add_tool(fix_scale
  fix_scale.cpp
  )

add_tool(optimize
  optimize.cpp
  )

add_tool(ply_convert
  ply/main.cpp
  ply/ply.cpp
  )

add_tool(sort
  sort/main.cpp
  )

boost_link_libraries(sort STATIC
  thread
  filesystem
  )

target_link_libraries(sort
  pthread
  )
