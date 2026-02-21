# embed_resource.cmake — Generate C++ source for a single embedded resource via xxd.
#
# Usage:
#   embed_resource(
#     INPUT  <path-to-data-file>
#     CPP    <output-cpp-path>
#     HPP    <output-hpp-path>
#     NAME   <C++-identifier-for-the-resource>
#   )
#
# This creates a custom command that produces:
#   <NAME>.hpp  — declares extern const unsigned char <NAME>[] and <NAME>_size
#   <NAME>.cpp  — defines the array using xxd -i output

find_program(XXD_EXECUTABLE xxd)
if(NOT XXD_EXECUTABLE)
  message(FATAL_ERROR "xxd not found. Install vim or xxd.")
endif()

function(embed_resource)
  cmake_parse_arguments(ER "" "INPUT;CPP;HPP;NAME" "" ${ARGN})

  if(NOT ER_INPUT OR NOT ER_CPP OR NOT ER_HPP OR NOT ER_NAME)
    message(FATAL_ERROR "embed_resource requires INPUT, CPP, HPP, and NAME arguments")
  endif()

  set(GUARD "EMBEDDED_${ER_NAME}_HPP")
  string(TOUPPER "${GUARD}" GUARD)

  # Generate the .hpp and .cpp with a single cmake -P script invocation.
  # The script runs xxd -i on the input file and wraps the output.
  add_custom_command(
    OUTPUT "${ER_CPP}" "${ER_HPP}"
    COMMAND ${XXD_EXECUTABLE} -i -n "${ER_NAME}" "${ER_INPUT}" "${ER_CPP}"
    COMMAND ${CMAKE_COMMAND} -E echo "//// Auto-generated — do not edit." > "${ER_HPP}"
    COMMAND ${CMAKE_COMMAND} -E echo "#ifndef ${GUARD}" >> "${ER_HPP}"
    COMMAND ${CMAKE_COMMAND} -E echo "#define ${GUARD}" >> "${ER_HPP}"
    COMMAND ${CMAKE_COMMAND} -E echo "" >> "${ER_HPP}"
    COMMAND ${CMAKE_COMMAND} -E echo "#include <cstddef>" >> "${ER_HPP}"
    COMMAND ${CMAKE_COMMAND} -E echo "#include <cstdint>" >> "${ER_HPP}"
    COMMAND ${CMAKE_COMMAND} -E echo "" >> "${ER_HPP}"
    COMMAND ${CMAKE_COMMAND} -E echo "extern unsigned char ${ER_NAME}[];" >> "${ER_HPP}"
    COMMAND ${CMAKE_COMMAND} -E echo "extern unsigned int ${ER_NAME}_len;" >> "${ER_HPP}"
    COMMAND ${CMAKE_COMMAND} -E echo "" >> "${ER_HPP}"
    COMMAND ${CMAKE_COMMAND} -E echo "#endif // ${GUARD}" >> "${ER_HPP}"
    DEPENDS "${ER_INPUT}"
    COMMENT "Embedding resource ${ER_NAME} from ${ER_INPUT}"
    VERBATIM
  )
endfunction()
