
set(data_output)

foreach(model bunny rockerArm screwdriver trico)
  set(input   "${CMAKE_CURRENT_SOURCE_DIR}/${model}.ply")
  set(output  "${CMAKE_CURRENT_BINARY_DIR}/${model}.junk")

  add_custom_command(OUTPUT ${output}
    COMMAND
      $<TARGET_FILE:jnk_convert> -af -i  ${input} -o ${output}
    DEPENDS
      ${input}
    )

  list(APPEND data_output ${output})
endforeach(model)

foreach(shader blinn_phong color)
  set(input   "${CMAKE_CURRENT_SOURCE_DIR}/${shader}.shader")
  set(output  "${CMAKE_CURRENT_BINARY_DIR}/${shader}.shader")

  add_custom_command(OUTPUT ${output}
    COMMAND
      ${CMAKE_COMMAND} -E copy ${input} ${output}
    DEPENDS
      ${input}
    )

  list(APPEND data_output ${output})
endforeach(shader)

add_custom_target(data ALL
  DEPENDS
    ${data_output}
  )
