
set(data_output)

foreach(model bunny rockerArm screwdriver trico)
  set(ply_input   "${CMAKE_CURRENT_SOURCE_DIR}/${model}.ply")
  set(ply_output  "${CMAKE_CURRENT_BINARY_DIR}/${model}.ply")
  set(junk_output "${CMAKE_CURRENT_BINARY_DIR}/${model}.junk")

  add_custom_command(OUTPUT ${ply_output}
    COMMAND
      ${CMAKE_COMMAND} -E copy ${ply_input} ${ply_output}
    DEPENDS
      ${ply_input}
    )

  add_custom_command(OUTPUT ${junk_output}
    COMMAND
      $<TARGET_FILE:ply_convert> ${ply_input} ${junk_output}
    DEPENDS
      ${ply_input}
    )

  list(APPEND models_output ${ply_output} ${junk_output})
endforeach(model)

foreach(shader color)
  set(shader_input   "${CMAKE_CURRENT_SOURCE_DIR}/${shader}.shader")
  set(shader_output  "${CMAKE_CURRENT_BINARY_DIR}/${shader}.shader")
  
  add_custom_command(OUTPUT ${shader_output}
    COMMAND
      ${CMAKE_COMMAND} -E copy ${shader_input} ${shader_output}
    DEPENDS
      ${shader_input}
    )

  list(APPEND data_output ${shader_output})
endforeach(shader)

add_custom_target(data ALL
  DEPENDS
    ${data_output}
  )
