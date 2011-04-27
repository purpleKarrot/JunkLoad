##
# Copyright (c) 2010-2011 Daniel Pfeifer <daniel@pfeifer-mail.de>
##

set(stringify_shaders_script "${CMAKE_CURRENT_LIST_FILE}")

function(stringify_shaders source_var)
  set(sources)
  foreach(file ${ARGN})
    set(input ${CMAKE_CURRENT_SOURCE_DIR}/${file})
    set(output ${CMAKE_CURRENT_BINARY_DIR}/${file})
    set(output_files ${output}.h ${output}.cpp)
    add_custom_command(OUTPUT ${output_files}
      COMMAND ${CMAKE_COMMAND} -DSTRINGIFY_SHADERS_PROCESSING_MODE=ON
        -DINPUT="${input}" -DOUTPUT="${output}"
        -P ${stringify_shaders_script}
      DEPENDS ${input}
      )
    list(APPEND sources ${output_files})
  endforeach(file)
  set(${source_var} ${sources} PARENT_SCOPE)
endfunction(stringify_shaders)

if(NOT STRINGIFY_SHADERS_PROCESSING_MODE)
  return()
endif(NOT STRINGIFY_SHADERS_PROCESSING_MODE)

#

get_filename_component(FILENAME ${INPUT} NAME)
string(REGEX REPLACE "[.]" "_" NAME ${FILENAME})

file(STRINGS ${INPUT} LINES)

file(WRITE ${OUTPUT}.h
  "/* Generated file, do not edit! */\n\n"
  "extern char const* const ${NAME};\n"
  )

file(WRITE ${OUTPUT}.cpp
  "/* Generated file, do not edit! */\n\n"
  "#include \"${FILENAME}.h\"\n\n"
  "char const* const ${NAME} = \n"
  )

foreach(LINE ${LINES})
  string(REPLACE "\"" "\\\"" LINE "${LINE}")
  file(APPEND ${OUTPUT}.cpp "   \"${LINE}\\n\"\n")
endforeach(LINE)

file(APPEND ${OUTPUT}.cpp "   ;\n")
